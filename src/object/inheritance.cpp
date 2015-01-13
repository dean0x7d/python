// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/python/object/inheritance.hpp>
#include <boost/python/type_id.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/property_map/property_map.hpp>
#include <queue>
#include <vector>
#include <functional>

//
// Procedure:
//
//      The search is a BFS over the space of (type,address) pairs
//      guided by the edges of the casting graph whose nodes
//      correspond to classes, and whose edges are traversed by
//      applying associated cast functions to an address. We use
//      vertex distance to the goal node in the cast_graph to rate the
//      paths. The vertex distance to any goal node is calculated on
//      demand and outdated by the addition of edges to the graph.

namespace boost {
namespace
{
  enum edge_cast_t { edge_cast = 8010 };
  template <class T> inline void unused_variable(const T&) { }
}

// Install properties
BOOST_INSTALL_PROPERTY(edge, cast);

namespace
{
  typedef void*(*cast_function)(void*);
  
  //
  // Here we put together the low-level data structures of the
  // casting graph representation.
  //
  using class_id = python::type_info;

  // represents a graph of available casts
  
  using cast_graph = adjacency_list<vecS,vecS, bidirectionalS, no_property,
      // edge index property allows us to look up edges in the connectivity matrix
      property<edge_index_t, std::size_t,
                 // The function which casts a void* from the edge's source type
                 // to its destination type.
                 property<edge_cast_t, cast_function>
      >
  >;

  using vertex_t = cast_graph::vertex_descriptor;
  using edge_t = cast_graph::edge_descriptor;
  
  struct smart_graph
  {
      typedef std::vector<std::size_t>::const_iterator node_distance_map;
      
      typedef std::pair<cast_graph::out_edge_iterator
                        , cast_graph::out_edge_iterator> out_edges_t;
      
      // Return a map of the distances from any node to the given
      // target node
      node_distance_map distances_to(vertex_t target) const
      {
          std::size_t n = num_vertices(m_topology);
          if (m_distances.size() != n * n)
          {
              m_distances.clear();
              m_distances.resize(n * n, (std::numeric_limits<std::size_t>::max)());
              m_known_vertices = n;
          }
          
          std::vector<std::size_t>::iterator to_target = m_distances.begin() + n * target;

          // this node hasn't been used as a target yet
          if (to_target[target] != 0)
          {
              typedef reverse_graph<cast_graph> reverse_cast_graph;
              reverse_cast_graph reverse_topology(m_topology);
              
              to_target[target] = 0;
              
              breadth_first_search(
                  reverse_topology, target
                  , visitor(
                      make_bfs_visitor(
                          record_distances(
                              make_iterator_property_map(
                                  to_target
                                  , get(vertex_index, reverse_topology)
                                  )
                              , on_tree_edge()
                              ))));
          }

          return to_target;
      }

      cast_graph& topology() { return m_topology; }
      cast_graph const& topology() const { return m_topology; }

      smart_graph()
          : m_known_vertices(0)
      {}
      
   private:
      cast_graph m_topology;
      mutable std::vector<std::size_t> m_distances;
      mutable std::size_t m_known_vertices;
  };
  
  smart_graph& full_graph()
  {
      static smart_graph x;
      return x;
  }
  
  smart_graph& up_graph()
  {
      static smart_graph x;
      return x;
  }

  //
  // Our index of class types
  //
  using boost::python::objects::dynamic_id_function;
  using index_entry = std::tuple<
      class_id,            // static type
      vertex_t,            // corresponding vertex
      dynamic_id_function  // dynamic_id if polymorphic, or 0
  >;
  enum { ksrc_static_t, kvertex, kdynamic_id };
  
  using type_index_t = std::vector<index_entry>;

  
  type_index_t& type_index()
  {
      static type_index_t x;
      return x;
  }

  // map a type to a position in the index
  inline type_index_t::iterator type_position(class_id type)
  {
    return std::find_if(
        type_index().begin(), type_index().end(), 
        [&type](index_entry const& entry) {
            return std::get<ksrc_static_t>(entry) >= type;
        }
    );
  }

  inline index_entry* seek_type(class_id type)
  {
      type_index_t::iterator p = type_position(type);
      if (p == type_index().end() || std::get<ksrc_static_t>(*p) != type)
          return 0;
      else
          return &*p;
  }
  
  // Get the entry for a type, inserting if necessary
  inline type_index_t::iterator demand_type(class_id type)
  {
      type_index_t::iterator p = type_position(type);

      if (p != type_index().end() && std::get<ksrc_static_t>(*p) == type)
          return p;

      vertex_t v = add_vertex(full_graph().topology());
      vertex_t v2 = add_vertex(up_graph().topology());
      unused_variable(v2);
      assert(v == v2);
      return type_index().insert(p, std::make_tuple(type, v, dynamic_id_function(0)));
  }

  // Map a two types to a vertex in the graph, inserting if necessary
  using type_index_iterator_pair = std::pair<type_index_t::iterator, type_index_t::iterator>;
  
  inline type_index_iterator_pair demand_types(class_id t1, class_id t2)
  {
      // be sure there will be no reallocation
      type_index().reserve(type_index().size() + 2);
      type_index_t::iterator first = demand_type(t1);
      type_index_t::iterator second = demand_type(t2);
      if (first == second)
          ++first;
      return std::make_pair(first, second);
  }

  struct q_elt
  {
      q_elt(std::size_t distance
            , void* src_address
            , vertex_t target
            , cast_function cast
            )
          : distance(distance)
          , src_address(src_address)
          , target(target)
          , cast(cast)
      {}
      
      std::size_t distance;
      void* src_address;
      vertex_t target;
      cast_function cast;

      bool operator<(q_elt const& rhs) const
      {
          return distance < rhs.distance;
      }
  };

  // Optimization:
  //
  // Given p, src_t, dst_t
  //
  // Get a pointer pd to the most-derived object
  //    if it's polymorphic, dynamic_cast to void*
  //    otherwise pd = p
  //
  // Get the most-derived typeid src_td
  //
  // ptrdiff_t offset = p - pd
  //
  // Now we can keep a cache, for [src_t, offset, src_td, dst_t] of
  // the cast transformation function to use on p and the next src_t
  // in the chain.  src_td, dst_t don't change throughout this
  // process. In order to represent unreachability, when a pair is
  // found to be unreachable, we stick a 0-returning "dead-cast"
  // function in the cache.
  
  // This is needed in a few places below
  inline void* identity_cast(void* p)
  {
      return p;
  }

  void* search(smart_graph const& g, void* p, vertex_t src, vertex_t dst)
  {
      // I think this test was thoroughly bogus -- dwa
      // If we know there's no path; bail now.
      // if (src > g.known_vertices() || dst > g.known_vertices())
      //    return 0;
      
      smart_graph::node_distance_map d(g.distances_to(dst));

      if (d[src] == (std::numeric_limits<std::size_t>::max)())
          return 0;

      typedef property_map<cast_graph,edge_cast_t>::const_type cast_map;
      cast_map casts = get(edge_cast, g.topology());
      
      typedef std::pair<vertex_t,void*> search_state;
      typedef std::vector<search_state> visited_t;
      visited_t visited;
      std::priority_queue<q_elt> q;
      
      q.push(q_elt(d[src], p, src, identity_cast));
      while (!q.empty())
      {
          q_elt top = q.top();
          q.pop();
          
          // Check to see if we have a real state
          void* dst_address = top.cast(top.src_address);
          if (dst_address == 0)
              continue;

          if (top.target == dst)
              return dst_address;
          
          search_state s(top.target,dst_address);

          visited_t::iterator pos = std::lower_bound(
              visited.begin(), visited.end(), s);

          // If already visited, continue
          if (pos != visited.end() && *pos == s)
              continue;
          
          visited.insert(pos, s); // mark it

          // expand it:
          smart_graph::out_edges_t edges = out_edges(s.first, g.topology());
          for (cast_graph::out_edge_iterator p = edges.first
                   , finish = edges.second
                   ; p != finish
                   ; ++p
              )
          {
              edge_t e = *p;
              q.push(q_elt(
                         d[target(e, g.topology())]
                         , dst_address
                         , target(e, g.topology())
                         , boost::get(casts, e)));
          }
      }
      return 0;
  }

  struct cache_element
  {
      using key_type = std::tuple<
          class_id,           // source static type
          class_id,           // target type
          std::ptrdiff_t,     // offset within source object
          class_id            // source dynamic type
      >;

      cache_element(key_type const& k)
          : key(k)
          , offset(0)
      {}
      
      key_type key;
      std::ptrdiff_t offset;

      static constexpr std::ptrdiff_t not_found = std::numeric_limits<std::ptrdiff_t>::min();
      
      bool operator<(cache_element const& rhs) const
      {
          return this->key < rhs.key;
      }

      bool unreachable() const
      {
          return offset == not_found;
      }
  };
  
  enum { kdst_t = ksrc_static_t + 1, koffset, ksrc_dynamic_t };
  using cache_t = std::vector<cache_element>;

  cache_t& cache()
  {
      static cache_t x;
      return x;
  }

  inline void* convert_type(void* const p, class_id src_t, class_id dst_t, bool polymorphic)
  {
      // Quickly rule out unregistered types
      index_entry* src_p = seek_type(src_t);
      if (src_p == 0)
          return 0;

      index_entry* dst_p = seek_type(dst_t);
      if (dst_p == 0)
          return 0;
    
      // Look up the dynamic_id function and call it to get the dynamic
      // info
      boost::python::objects::dynamic_id_t dynamic_id = polymorphic
          ? std::get<kdynamic_id>(*src_p)(p)
          : std::make_pair(p, src_t);
    
      // Look in the cache first for a quickie address translation
      std::ptrdiff_t offset = (char*)p - (char*)dynamic_id.first;

      cache_element seek(std::make_tuple(src_t, dst_t, offset, dynamic_id.second));
      cache_t& c = cache();
      cache_t::iterator const cache_pos
          = std::lower_bound(c.begin(), c.end(), seek);
                      

      // if found in the cache, we're done
      if (cache_pos != c.end() && cache_pos->key == seek.key)
      {
          return cache_pos->offset == cache_element::not_found
              ? 0 : (char*)p + cache_pos->offset;
      }

      // If we are starting at the most-derived type, only look in the up graph
      smart_graph const& g = polymorphic && dynamic_id.second != src_t
          ? full_graph() : up_graph();
    
      void* result = search(
          g, p, std::get<kvertex>(*src_p)
          , std::get<kvertex>(*dst_p));

      // update the cache
      c.insert(cache_pos, seek)->offset
          = (result == 0) ? cache_element::not_found : (char*)result - (char*)p;

      return result;
  }
}

namespace python { namespace objects {

BOOST_PYTHON_DECL void* find_dynamic_type(void* p, class_id src_t, class_id dst_t)
{
    return convert_type(p, src_t, dst_t, true);
}

BOOST_PYTHON_DECL void* find_static_type(void* p, class_id src_t, class_id dst_t)
{
    return convert_type(p, src_t, dst_t, false);
}

BOOST_PYTHON_DECL void add_cast(
    class_id src_t, class_id dst_t, cast_function cast, bool is_downcast)
{
    // adding an edge will invalidate any record of unreachability in
    // the cache.
    static std::size_t expected_cache_len = 0;
    cache_t& c = cache();
    if (c.size() > expected_cache_len)
    {
        c.erase(std::remove_if(
                    c.begin(), c.end(),
                    std::mem_fn(&cache_element::unreachable))
                , c.end());

        // If any new cache entries get added, we'll have to do this
        // again when the next edge is added
        expected_cache_len = c.size();
    }
    
    type_index_iterator_pair types = demand_types(src_t, dst_t);
    vertex_t src = std::get<kvertex>(*types.first);
    vertex_t dst = std::get<kvertex>(*types.second);

    cast_graph* const g[2] = { &up_graph().topology(), &full_graph().topology() };
    
    for (cast_graph*const* p = g + (is_downcast ? 1 : 0); p < g + 2; ++p)
    {
        edge_t e;
        bool added;

        tie(e, added) = add_edge(src, dst, **p);
        assert(added);

        put(get(edge_cast, **p), e, cast);
        put(get(edge_index, **p), e, num_edges(full_graph().topology()) - 1);
    }
}

BOOST_PYTHON_DECL void register_dynamic_id_aux(
    class_id static_id, dynamic_id_function get_dynamic_id)
{
    std::get<kdynamic_id>(*demand_type(static_id)) = get_dynamic_id;
}

}}} // namespace boost::python::objects

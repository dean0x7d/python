// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef KEYWORDS_DWA2002323_HPP
# define KEYWORDS_DWA2002323_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/args_fwd.hpp>
# include <boost/python/object_core.hpp>

# include <algorithm>

namespace boost { namespace python {

using arg = detail::keywords<1>;

namespace detail
{
  template <std::size_t nkeywords>
  struct keywords_base
  {
      static constexpr auto size = nkeywords;
      
      keyword_range range() const
      {
          return keyword_range(elements, elements + nkeywords);
      }

      keyword elements[nkeywords];

      keywords<nkeywords+1>
      operator,(python::arg const &k) const;

      keywords<nkeywords + 1>
      operator,(char const *name) const;
  };
  
  template <std::size_t nkeywords>
  struct keywords : keywords_base<nkeywords>
  {
  };

  template <>
  struct keywords<1> : keywords_base<1>
  {
      explicit keywords(char const *name)
      {
          elements[0].name = name;
      }
    
      template <class T>
      python::arg& operator=(T const& value)
      {
          object z(value);
          elements[0].default_value = handle<>(python::borrowed(object(value).ptr()));
          return *this;
      }
    
      operator detail::keyword const&() const
      {
          return elements[0];
      }
  };

  template <std::size_t nkeywords>
  inline
  keywords<nkeywords+1>
  keywords_base<nkeywords>::operator,(python::arg const &k) const
  {
      keywords<nkeywords> const& l = *static_cast<keywords<nkeywords> const*>(this);
      python::detail::keywords<nkeywords+1> res;
      std::copy(l.elements, l.elements+nkeywords, res.elements);
      res.elements[nkeywords] = k.elements[0];
      return res;
  }

  template <std::size_t nkeywords>
  inline
  keywords<nkeywords + 1>
  keywords_base<nkeywords>::operator,(char const *name) const
  {
      return this->operator,(python::arg(name));
  }

  template<typename T>
  struct is_keywords {
      static constexpr bool value = false;
  };

  template<std::size_t nkeywords>
  struct is_keywords<keywords<nkeywords> > {
      static constexpr bool value = true;
  };

  template <class T>
  struct is_reference_to_keywords
  {
      using key_t = cpp14::remove_cv_t<cpp14::remove_reference_t<T>>;
      static constexpr bool value = is_reference<T>::value && is_keywords<key_t>::value;
      
      using type = std::integral_constant<bool, value>;
  };
}

inline detail::keywords<1> args(char const* name)
{ 
    return detail::keywords<1>(name);
}

template<typename... Ts, int N = sizeof...(Ts)>
detail::keywords<N> args(Ts... names)
{
    detail::keywords<N> result;
    auto list = { detail::keyword(names)... };
    std::move(std::begin(list), std::end(list), 
              std::begin(result.elements));
    
    return result;
}

}} // namespace boost::python

# endif // KEYWORDS_DWA2002323_HPP

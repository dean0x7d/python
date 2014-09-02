// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef EXCEPTION_HANDLER_DWA2002810_HPP
# define EXCEPTION_HANDLER_DWA2002810_HPP

# include <boost/python/detail/config.hpp>
# include <functional>

namespace boost { namespace python { namespace detail {

struct BOOST_PYTHON_DECL_FORWARD exception_handler;

using handler_function = std::function<
    bool(exception_handler const&, std::function<void()> const&)
>;

struct BOOST_PYTHON_DECL exception_handler
{
 private: // types
    
 public:
    explicit exception_handler(handler_function const& impl);

    inline bool handle(std::function<void()> const& f) const;
    
    bool operator()(std::function<void()> const& f) const;
 
    static exception_handler* chain;
    
 private:
    static exception_handler* tail;
    
    handler_function m_impl;
    exception_handler* m_next;
};


inline bool exception_handler::handle(std::function<void()> const& f) const
{
    return this->m_impl(*this, f);
}

BOOST_PYTHON_DECL void register_exception_handler(handler_function const& f);

}}} // namespace boost::python::detail

#endif // EXCEPTION_HANDLER_DWA2002810_HPP

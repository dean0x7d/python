"""
>>> from std_containers_ext import *
>>> def print_func(a):
...     print a

### TUPLE

>>> print tuple_return_to_python.__doc__
tuple_return_to_python() -> tuple:

>>> print tuple_arg_from_python.__doc__
tuple_arg_from_python(a1: tuple) -> tuple:

>>> tuple_return_to_python()
(1, 2.0, 3.0)

>>> tuple_arg_to_python(print_func)
('char const*', 'std::string')

>>> tuple_return_from_python((2, 3))
(2, 3)

>>> tuple_arg_from_python((1, 2))
(1, 2)

>>> tuple_empty(tuple())
True

>>> try: tuple_return_from_python((2, 'wrong type'))
... except TypeError: pass
... else: print 'expected a TypeError exception'

>>> try: tuple_return_from_python('wrong type')
... except TypeError: pass
... else: print 'expected a TypeError exception'

>>> try: tuple_return_from_python((2, 3, 4))
... except TypeError: pass
... else: print 'expected a TypeError exception'

### PAIR

>>> print pair_return_to_python.__doc__
pair_return_to_python() -> tuple:

>>> print pair_arg_from_python.__doc__
pair_arg_from_python(a1: tuple) -> tuple:

>>> pair_return_to_python()
(1, 2.0)

>>> pair_arg_to_python(print_func)
('char const*', 'std::string')

>>> pair_return_from_python((2, 3))
(2, 3)

>>> pair_arg_from_python((1, 2))
(1, 2)

>>> try: pair_return_from_python((2, 'wrong type'))
... except TypeError: pass
... else: print 'expected a TypeError exception'

>>> try: pair_return_from_python('wrong type')
... except TypeError: pass
... else: print 'expected a TypeError exception'

>>> try: pair_return_from_python((2, 3, 4))
... except TypeError: pass
... else: print 'expected a TypeError exception'

### VECTOR

>>> print vector_return_to_python.__doc__
vector_return_to_python() -> list:

>>> print vector_arg_from_python.__doc__
vector_arg_from_python(a1: list) -> list:

>>> vector_return_to_python()
[1, 2, 3]

>>> vector_arg_to_python(print_func)
['a', 'b', 'c']

>>> vector_return_from_python([2, 3])
[2, 3]

>>> vector_arg_from_python([1, 2])
[1.0, 2.0]

>>> vector_arg_from_python((1, 2))
[1.0, 2.0]

>>> vector_empty([])
True

>>> try: vector_arg_from_python('wrong type')
... except TypeError: pass
... else: print 'expected a TypeError exception'

>>> try: vector_arg_from_python(['wrong type in list'])
... except TypeError: pass
... else: print 'expected a TypeError exception'

>>> try: vector_arg_from_python([1, 2.0, 'mixed types'])
... except TypeError: pass
... else: print 'expected a TypeError exception'

"""

def run(args = None):
    import sys
    import doctest

    if args is not None:
        sys.argv = args
    return doctest.testmod(sys.modules.get(__name__))
    
if __name__ == '__main__':
    print "running..."
    import sys
    status = run()[0]
    if (status == 0): print "Done."
    sys.exit(status)

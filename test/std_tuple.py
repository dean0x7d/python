"""
>>> from __future__ import print_function
>>> from std_tuple_ext import *
>>> def print_doc(obj, *args):
...   doc = obj.__doc__.splitlines()
...   print("\\n".join(["|" + doc[i] for i in args]))

>>> print_doc(tuple_return_to_python, 1)
|tuple_return_to_python() -> tuple:

>>> tuple_return_to_python()
(1, 2.0, 3.0)

>>> def print_tuple(t):
...     print(t)
>>> tuple_arg_to_python(print_tuple)
('char const*', 'std::string')

>>> tuple_return_from_python((2, 3))
(2, 3)

>>> try: tuple_return_from_python((2, 'wrong type'))
... except TypeError: pass
... else: print 'expected a TypeError exception'

>>> try: tuple_return_from_python('wrong type')
... except TypeError: pass
... else: print 'expected a TypeError exception'

>>> try: tuple_return_from_python((2, 3, 4))
... except TypeError: pass
... else: print 'expected a TypeError exception'

>>> print_doc(tuple_arg_from_python, 1)
|tuple_arg_from_python(a1: tuple) -> tuple:

>>> tuple_arg_from_python((1, 2))
(1, 2)

>>> test_empty(tuple())
1

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

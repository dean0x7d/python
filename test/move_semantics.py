# Copyright David Abrahams 2004. Distributed under the Boost
# Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
'''
>>> from move_semantics_ext import *

#
# Test move from rvalue converter
#

>>> movable_rvalue(None)
(False, True)

>>> movable_rvalue_const_ref(None)
(False, False)

>>> copyonly_rvalue(None)
(True, False)

>>> copyonly_rvalue_const_ref(None)
(False, False)

>>> moveonly_rvalue(None)
(False, True)

>>> moveonly_rvalue_const_ref(None)
(False, False)

#
# Test unique_ptr
#

>>> x = X(42)
>>> x.value()
42
>>> look(x), look(x)
(42, 42)

>>> maybe_steal(x, 0)
42
>>> look(x)
42

>>> maybe_steal(x, 1)
42
>>> look(x)
-1

>>> x = X(69)
>>> steal(x)
69
>>> look(x)
-1

>>> try: x.value()
... except TypeError: pass
... else: print 'expected a TypeError exception'

>>> x = make()
>>> look(x)
77

>>> z = callback(lambda z: z)
>>> z.value()
77

>>> extract(x).value()
77

#
# Test derived to base conversions
#

>>> y = Y(42)
>>> y.value()
42

>>> try: maybe_steal(y, 0)
... except TypeError: pass
... else: print 'expected a TypeError exception'

>>> y.value()
42

>>> steal(y)
42

>>> try: y.value()
... except TypeError: pass
... else: print 'expected a TypeError exception'

'''

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

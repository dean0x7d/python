# Copyright David Abrahams 2004. Distributed under the Boost
# Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
"""
>>> from dict_ext import *
>>> def printer(*args):
...     for x in args: print x,
...     print
...
>>> compare = lambda a, b: len(a) == len(b) and all(x in a for x in b)
>>> print new_dict()
{}
>>> data_dict() == {1: {'key2': 'value2'}, 'key1': 'value1'}
True
>>> tmp = data_dict()
>>> compare(dict_keys(tmp), [1, 'key1'])
True
>>> compare(dict_values(tmp), [{'key2': 'value2'}, 'value1'])
True
>>> compare(dict_items(tmp), [(1, {'key2': 'value2'}), ('key1', 'value1')])
True
>>> dict_from_sequence([(1,1),(2,2),(3,3)]) == {1: 1, 2: 2, 3: 3}
True
>>> test_templates(printer) #doctest: +NORMALIZE_WHITESPACE
a test string
13
None
[(1, 'a test string'), (1.5, 13)]
default
default
3
13
True False
a test string
default
(1.5, 13)
{}
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

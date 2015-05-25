"""
>>> from __future__ import print_function
>>> from def_lambda_ext import *

>>> captureless.__doc__
'captureless() -> str:'
>>> captureless()
'Hello World'

>>> capture_const.__doc__
'capture_const() -> int:'
>>> capture_const()
1

>>> capture_mutable.__doc__
'capture_mutable() -> int:'
>>> capture_mutable()
2
>>> capture_mutable()
3

>>> with_params.__doc__
'with_params(s: str, i: int) -> str:'
>>> with_params('abc', 123)
'abc123'

>>> and_defaults.__doc__
'and_defaults(o: object, s: str, i: int=7, b: bool=False) -> tuple:'
>>> and_defaults({}, 'string')
({}, 'string', 7, False)

>>> x = X()
>>> x.value
10

>>> X.static_method.__doc__
'static_method() -> str:'
>>> X.static_method()
'static'

>>> X.method.__doc__
'method(self: X, arg2: int) -> int:'
>>> x.method(5)
15

>>> X.add.__doc__
'add(self: X, value: int) -> None:'
>>> x.add(2)
>>> x.value
12

"""
    
if __name__ == '__main__':
    import sys
    import doctest
    sys.exit(doctest.testmod()[0])

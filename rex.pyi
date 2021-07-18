from typing import overload


class Regex:
    # TODO: add 'global' attributes, like ASCII, UNICODE
    #  GREEDY, NOT_GREEDY, MULTILINE, etc.

    def __init__(self, regex: str) -> None: ...

    def __contains__(self, item: str) -> bool: ...

    def __eq__(self, other: str) -> bool: ...

    @overload
    def __add__(self, other: Regex) -> Regex: ...

    @overload
    def __add__(self, other: str) -> Regex: ...

    def __radd__(self, other: str) -> Regex: ...

    @overload
    def __sub__(self, other: Regex) -> Regex: ...

    @overload
    def __sub__(self, other: str) -> Regex: ...

    def __rsub__(self, other: str) -> Regex: ...

    def __neg__(self) -> Regex: ...

    def __bool__(self) -> Regex:
        # for the purpose of 'not' operator?
        pass
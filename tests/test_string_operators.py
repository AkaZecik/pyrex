import unittest


class TestStringOperators(unittest.TestCase):
    def test_simple_ascii(self) -> None:
        pass

    def test_union(self) -> None:
        # |
        pass

    def test_intersection(self) -> None:
        # &
        pass

    def test_difference(self) -> None:
        # -
        pass

    def test_complement_in_ascii(self) -> None:
        # !
        pass

    def test_xor(self) -> None:
        # TODO: do we want to have it?
        # TODO: do we want it to be the same as ^?
        # ^
        # maybe use ":"?
        pass

    def test_star(self) -> None:
        # *
        pass

    def test_plus(self) -> None:
        # +
        pass

    def test_curly_exact_repeats(self) -> None:
        # {10}
        pass

    def test_curly_min_repeats(self) -> None:
        # {10,}
        pass

    def test_curly_max_repeats(self) -> None:
        # {,10}
        pass

    def test_curly_range_repeats(self) -> None:
        # {5,10}
        pass

    def test_optional(self) -> None:
        # ?
        pass

    def test_case_insensitive(self) -> None:
        # ~
        pass

    def test_repeats(self) -> None:
        # %
        pass

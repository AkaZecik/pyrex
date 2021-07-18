import unittest

from rex import Regex


class TestInstanceOperators(unittest.TestCase):
    def test_not(self) -> None:
        regex = Regex("TODO")

import unittest

from rex import Regex


class TestInstanceOperators(unittest.TestCase):
    def test_not(self) -> None:
        regex = Regex("TODO")

    def test_getitem(self) -> None:
        regex = Regex("(?hello)(world)(?Plorem:ipsum)")
        cg = regex[1]  # returns first capture group
        self.assertIsInstance(cg, Regex)
        cg = regex["lorem"]  # returns first capture group
        self.assertIsInstance(cg, Regex)

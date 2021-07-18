import unittest


class TestSpecialCharacters(unittest.TestCase):
    # TODO: test their corresponding escaping (remember about \\)

    def test_dot(self) -> None:
        pass

    def test_caret(self) -> None:
        pass

    def test_dollar(self) -> None:
        pass

    def test_at_string_beginning(self) -> None:
        # \A
        # TODO: what about the opposite?
        # TODO: how does it relate to ^?
        pass

    def test_at_string_end(self) -> None:
        # \Z
        pass

    def test_empty_string_at_word_beginning_or_end(self) -> None:
        # \b
        pass

    def test_empty_string_not_at_word_beginning_or_end(self) -> None:
        # \B
        pass

    def test_digit(self) -> None:
        # \d == [0-9] (ascii/unicode?)
        pass

    def test_not_digit(self) -> None:
        # \D
        pass

    def test_whitespace(self) -> None:
        # \s
        pass

    def test_not_whitespace(self) -> None:
        # \S
        pass

    def test_word_char(self) -> None:
        # \w
        pass

    def test_not_word_char(self) -> None:
        # \W
        pass

    def test_entire_language(self) -> None:
        # \@
        pass

    def test_empty_language(self) -> None:
        # \#
        pass

//
// Created by bercik on 10.08.2021.
//

#include <string>
#include <stdexcept>
#include <vector>

// https://stackoverflow.com/questions/16182781/how-to-create-exceptions

struct TokenizationException : std::runtime_error {
    explicit TokenizationException(std::string const &what) : std::runtime_error(what) {}
};

struct PrematureEndOfPatternError : TokenizationException {
    explicit PrematureEndOfPatternError(long long position) : TokenizationException("") {}
};

enum class TokenType {
    LPAREN, // (
    RPAREN, // )
    LBRACK, // (
    RBRACK, // )
    LCURLY, // {
    RCURLY, // }
    LARROW, // <
    RARROW, // >
    POWER, // ^
    DOLAR, // $
    QMARK, // ?
    EMARK, // !
    COLON, // :
    MINUS, // -
    STAR, // *
    PLUS, // +
    PERCENT, // %
    TILDE, // ~
    UNION, // |
    INTERSECTION, // &
    DOT, // .
    ESCAPE, // \char
    BYTE, // \xNN
    SHORT_UNICODE, // \uNNNN
    LONG_UNICODE, // \UNNNNNNNN
    LITERAL, // anything else
    END, // end of tokens
};

/* TODO: add "start" and "end" values */
union TokenValue {
    char c;
    unsigned int i;
};

struct Token {
    TokenType const type;
    TokenValue value;

    explicit Token(TokenType type) : type(type), value{} {}
    Token(TokenType type, unsigned int i) : type(type), value{.i = i} {}
    Token(TokenType type, char c) : type(type), value{.c = c} {}
};

struct Tokenizer {
    std::string regex;
    int curr_pos = 0;

    explicit Tokenizer(std::string regex) : regex(std::move(regex)) {}

    std::vector<Token> get_all_tokens() {
        std::vector<Token> tokens;

        while (true) {
            Token token = get_token();

            if (token.type == TokenType::END) {
                return tokens;
            } else {
                tokens.push_back(token);
            }
        }
    }

    Token get_token() {
        if (curr_pos >= regex.size()) {
            return Token(TokenType::END);
        }

        char c = regex[curr_pos];
        curr_pos += 1;

        switch (c) {
            case '(':
                return Token(TokenType::LPAREN);
            case ')':
                return Token(TokenType::RPAREN);
            case '[':
                return Token(TokenType::LBRACK);
            case ']':
                return Token(TokenType::RBRACK);
            case '{':
                return Token(TokenType::LCURLY);
            case '}':
                return Token(TokenType::RCURLY);
            case '<':
                return Token(TokenType::LARROW);
            case '>':
                return Token(TokenType::RARROW);
            case '^':
                return Token(TokenType::POWER);
            case '$':
                return Token(TokenType::DOLAR);
            case '?':
                return Token(TokenType::QMARK);
            case '!':
                return Token(TokenType::EMARK);
            case ':':
                return Token(TokenType::COLON);
            case '-':
                return Token(TokenType::MINUS);
            case '*':
                return Token(TokenType::STAR);
            case '+':
                return Token(TokenType::PLUS);
            case '%':
                return Token(TokenType::PERCENT);
            case '~':
                return Token(TokenType::TILDE);
            case '|':
                return Token(TokenType::UNION);
            case '&':
                return Token(TokenType::INTERSECTION);
            case '.':
                return Token(TokenType::DOT);
            case '\\':
                return tokenize_escape();
            default:
                return {TokenType::LITERAL, c};
        }
    }

    Token tokenize_escape() {
        if (curr_pos == regex.size()) {
            throw std::string(
                "Bad escape at position " +
                std::to_string(curr_pos - 1) +
                ": premature end of pattern");
        }

        char c = regex[curr_pos];
        curr_pos += 1;

        switch (c) {
            case 'x': {
                unsigned long value = parse_hex(2);
                curr_pos += 2;
                return {TokenType::BYTE, (unsigned int) value};
            }
            case 'u': {
                unsigned long value = parse_hex(4);
                curr_pos += 4;
                return {TokenType::SHORT_UNICODE, (unsigned int) value};
            }
            case 'U': {
                unsigned long value = parse_hex(8);
                curr_pos += 8;

                if (value > 0x10ffffUL) {
                    throw std::string(
                        "Bad unicode character escape at position " +
                        std::to_string(curr_pos - 2) +
                        ": maximum allowed value is \\U0010FFFF, got " +
                        regex.substr(curr_pos - 2, curr_pos + 8)
                    );
                }

                return {TokenType::LONG_UNICODE, (unsigned int) value};
            }
            default:
                return {TokenType::ESCAPE, c};
        }
    }

    static inline bool is_hex_digit(char c) {
        return '0' <= c && c <= '9' || 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z';
    }

    unsigned long parse_hex(int count) {
        if (curr_pos + count > regex.size()) {
            throw std::string(
                "Bad escape at position " +
                std::to_string(curr_pos - 2) +
                ": premature end of pattern"
            );
        }

        for (int i = curr_pos; i < curr_pos + count; ++i) {
            if (!is_hex_digit(regex[i])) {
                throw std::string(
                    "Bad byte character escape at position " +
                    std::to_string(curr_pos - 2) +
                    ": incorrect escape value " +
                    regex.substr(curr_pos - 2, curr_pos + count)
                );
            }
        }

        return std::stoul(regex.substr(curr_pos, curr_pos + count), nullptr, 16);
    }
};
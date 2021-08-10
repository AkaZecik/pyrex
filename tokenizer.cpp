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
    ESCAPE, // \char
    BYTE, // \xNN
    SHORT_UNICODE, // \uNNNN
    LONG_UNICODE, // \UNNNNNNNN
    LITERAL, // anything else
    END, // end of tokens
};

struct Token {
    TokenType const type;
    void *const value = nullptr;

    explicit Token(TokenType type) : type(type) {}

    Token(TokenType type, void *value) : type(type), value(value) {}

    ~Token() {
        if (value != nullptr) {
            switch (type) {
                case TokenType::ESCAPE:
                case TokenType::LITERAL:
                    delete (char *) value;
                    break;
                case TokenType::BYTE:
                case TokenType::SHORT_UNICODE:
                case TokenType::LONG_UNICODE:
                    delete (int *) value;
                    break;
                default:
                    break;
            }
        }
    }
};

struct Tokenizer {
    std::string regex;
    int curr_pos = 0;

    explicit Tokenizer(std::string regex) : regex(std::move(regex)) {}

    std::vector<Token *> get_all_tokens() {
        std::vector<Token *> tokens;

        try {
            while (true) {
                Token *token = get_token();

                if (token->type != TokenType::END) {
                    tokens.push_back(token);
                    break;
                }
            }
        } catch (TokenizationException const &e) {
            for (Token *token : tokens) {
                delete token;
            }

            throw e;
        }

        return tokens;
    }

    Token *get_token() {
        if (curr_pos >= regex.size()) {
            return new Token(TokenType::END);
        }

        char c = regex[curr_pos];
        curr_pos += 1;

        switch (c) {
            case '(':
                return new Token(TokenType::LPAREN);
            case ')':
                return new Token(TokenType::RPAREN);
            case '[':
                return new Token(TokenType::LBRACK);
            case ']':
                return new Token(TokenType::RBRACK);
            case '{':
                return new Token(TokenType::LCURLY);
            case '}':
                return new Token(TokenType::RCURLY);
            case '<':
                return new Token(TokenType::LARROW);
            case '>':
                return new Token(TokenType::RARROW);
            case '^':
                return new Token(TokenType::POWER);
            case '$':
                return new Token(TokenType::DOLAR);
            case '?':
                return new Token(TokenType::QMARK);
            case '!':
                return new Token(TokenType::EMARK);
            case ':':
                return new Token(TokenType::COLON);
            case '-':
                return new Token(TokenType::MINUS);
            case '*':
                return new Token(TokenType::STAR);
            case '+':
                return new Token(TokenType::PLUS);
            case '%':
                return new Token(TokenType::PERCENT);
            case '~':
                return new Token(TokenType::TILDE);
            case '|':
                return new Token(TokenType::UNION);
            case '&':
                return new Token(TokenType::INTERSECTION);
            case '\\':
                return tokenize_escape();
            default:
                return new Token(TokenType::LITERAL, new char(c));
        }
    }

    Token *tokenize_escape() {
        if (curr_pos == regex.size()) {
            throw std::string(
                "Bad escape at position " +
                std::to_string(curr_pos - 1) +
                ": premature end of pattern");
        }

        char c = regex[curr_pos];
        curr_pos += 1;

        switch (c) {
            case 'x':
                return get_byte_escape();
            case 'u':
                return get_short_unicode_escape();
            case 'U':
                return get_long_unicode_escape();
            default:
                return new Token(TokenType::LITERAL, new char(c));
        }
    }

    static inline bool is_hex_digit(char c) {
        return '0' <= c && c <= '9' || 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z';
    }

    Token *get_byte_escape() {
        if (curr_pos + 2 > regex.size()) {
            throw std::string(
                "Bad byte character escape at position " +
                std::to_string(curr_pos - 2) +
                ": premature end of pattern"
            );
        }

        if (!is_hex_digit(regex[curr_pos]) || !is_hex_digit(regex[curr_pos + 1])) {
            throw std::string(
                "Bad byte character escape at position " +
                std::to_string(curr_pos - 2) +
                ": incorrect escape value " +
                regex.substr(curr_pos - 2, curr_pos + 2)
            );
        }

        int value = std::stoi(regex.substr(curr_pos, curr_pos + 2), nullptr, 16);
        curr_pos += 2;
        return new Token(TokenType::BYTE, new int(value));
    }

    Token *get_short_unicode_escape() {
        if (curr_pos + 4 > regex.size()) {
            throw std::string(
                "Bad unicode escape at position " +
                std::to_string(curr_pos - 2) +
                ": premature end of pattern"
            );
        }

        if (!is_hex_digit(regex[curr_pos]) ||
            !is_hex_digit(regex[curr_pos + 1]) ||
            !is_hex_digit(regex[curr_pos + 2]) ||
            !is_hex_digit(regex[curr_pos + 3])
            ) {
            throw std::string(
                "Bad unicode character escape at position " +
                std::to_string(curr_pos - 2) +
                ": incorrect escape value " +
                regex.substr(curr_pos - 2, curr_pos + 4)
            );
        }

        int value = std::stoi(regex.substr(curr_pos, curr_pos + 4), nullptr, 16);
        curr_pos += 4;
        return new Token(TokenType::BYTE, new int(value));
    }

    Token *get_long_unicode_escape() {
        if (curr_pos + 8 > regex.size()) {
            throw std::string(
                "Bad unicode escape at position " +
                std::to_string(curr_pos - 2) +
                ": premature end of pattern"
            );
        }

        if (!is_hex_digit(regex[curr_pos]) ||
            !is_hex_digit(regex[curr_pos + 1]) ||
            !is_hex_digit(regex[curr_pos + 2]) ||
            !is_hex_digit(regex[curr_pos + 3]) ||
            !is_hex_digit(regex[curr_pos + 4]) ||
            !is_hex_digit(regex[curr_pos + 5]) ||
            !is_hex_digit(regex[curr_pos + 6]) ||
            !is_hex_digit(regex[curr_pos + 7])
            ) {
            throw std::string(
                "Bad unicode character escape at position " +
                std::to_string(curr_pos - 2) +
                ": incorrect escape value " +
                regex.substr(curr_pos - 2, curr_pos + 8)
            );
        }

        if (regex[curr_pos] != '0' || regex[curr_pos + 1] != '0') {
            throw std::string(
                "Bad unicode character escape at position " +
                std::to_string(curr_pos - 2) +
                ": maximum allowed value is \\U0010FFFF, got " +
                regex.substr(curr_pos - 2, curr_pos + 8)
            );
        }

        int value = std::stoi(regex.substr(curr_pos, curr_pos + 8), nullptr, 16);
        curr_pos += 8;
        return new Token(TokenType::BYTE, new int(value));
    }

    static inline Token *is_short_unicode_escape(std::string const &regex, long long pos) {
        if (pos + 6 <= regex.size() &&
            regex[pos + 1] == 'u' &&
            is_hex_digit(regex[pos + 2]) &&
            is_hex_digit(regex[pos + 3]) &&
            is_hex_digit(regex[pos + 4]) &&
            is_hex_digit(regex[pos + 5])) {
            int *value = new int(std::stoi(regex.substr(pos + 2, pos + 6), nullptr, 16));
            return new Token(TokenType::SHORT_UNICODE, value);
        }

        return;
    }

    static inline int is_long_unicode_escape(std::string const &regex, long long pos) {
        if (pos + 10 <= regex.size() &&
            regex[pos + 1] == 'U' &&
            is_hex_digit(regex)
            ) {

        }
    }
};
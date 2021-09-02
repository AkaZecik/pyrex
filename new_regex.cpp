#include "new_regex.h"

namespace pyrex {
    Regex::Regex(Regex const &other) : ast{other.ast}, regex{other.regex} {}
    Regex::Regex(std::string regex) : ast{AST::from_regex(regex)}, regex(regex) {}

    void Regex::compile() {
        if (!nfa) {
            nfa = NFA::from_ast(ast);
        }
    }
}
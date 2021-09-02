#include "regex.h"

#include <utility>

/* TODO:
 *  - make power, min, max unsigned numbers
 */

namespace pyrex {
    Regex::Regex(AST ast) : ast(std::move(ast)) {}
    Regex::Regex(Regex const &other) : ast{other.ast}, regex{other.regex} {}
    Regex::Regex(std::string regex) : ast{AST::from_regex(regex)}, regex(regex) {}

    void Regex::compile() {
        if (!nfa) {
            nfa = NFA::from_ast(ast);
        }
    }

    std::string Regex::to_string() {
        if (!regex) {
            regex = ast.to_string();
        }

        return *regex;
    }

    Regex Regex::operator[](std::size_t group_number) const {
        auto &numbered_cgroups = ast.get_numbered_cgroups();

        if (group_number > numbered_cgroups.size()) {
            throw std::runtime_error("Provided group number is too big");
        }

        return Regex{numbered_cgroups[group_number].node->to_string()};
    }

    Regex Regex::operator[](std::string const &group_name) const {
        auto &named_cgroups = ast.get_named_cgroups();
        auto group_it = named_cgroups.find(group_name);

        if (group_it == named_cgroups.cend()) {
            throw std::runtime_error("Provided group name doesn't exist");
        }

        return Regex{group_it->second.node->to_string()};
    }

    Regex Regex::for_nothing() {
        return Regex{AST::for_nothing()};
    }

    Regex Regex::for_empty() {
        return Regex{AST::for_empty()};
    }

    Regex Regex::for_char(char chr) {
        return Regex{AST::for_char(chr)};
    }

    Regex Regex::for_dot() {
        return Regex{AST::for_dot()};
    }

    Regex Regex::for_small_d() {
        return Regex{AST::for_small_d()};
    }

    Regex Regex::for_small_s() {
        return Regex{AST::for_small_s()};
    }

    Regex Regex::for_small_w() {
        return Regex{AST::for_small_w()};
    }

    Regex Regex::numbered_cgroup() const {
        return Regex{AST::numbered_cgroup(ast)};
    }

    Regex Regex::named_cgroup(std::string const& name) const {
        return Regex{AST::named_cgroup(ast, name)};
    }

    Regex Regex::non_cgroup() const {
        return Regex{AST::non_cgroup(ast)};
    }

    Regex Regex::qmark() const {
        return Regex{AST::qmark(ast)};
    }

    Regex Regex::star() const {
        return Regex{AST::star(ast)};
    }

    Regex Regex::plus() const {
        return Regex{AST::plus(ast)};
    }

    Regex Regex::power(int power) const {
        return Regex{AST::power(ast, power)};
    }

    Regex Regex::min(int min) const {
        return Regex{AST::min(ast, min)};
    }

    Regex Regex::max(int max) const {
        return Regex{AST::max(ast, max)};
    }

    Regex Regex::range(int min, int max) const {
        return Regex{AST::range(ast, min, max)};
    }

    Regex Regex::concat(const Regex &other) const {
        return Regex{AST::concat(ast, other.ast)};
    }

    Regex Regex::union_(const Regex &other) const {
        return Regex{AST::union_(ast, other.ast)};
    }

    Regex Regex::percent(const Regex &other) const {
        return Regex{AST::percent(ast, other.ast)};
    }
}
#ifndef PYREX_NEW_REGEX_H
#define PYREX_NEW_REGEX_H

#include <optional>
#include <set>
#include "ast.h"

/*
 * TODO:
 *  - determine which methods receive Regex, Regex const & and Regex &&.
 *  - similarly for NFA and DFA
 */

namespace pyrex {
    struct Regex {
        struct Group {
            std::shared_ptr<AST::Node> const group;
        };

        struct NFA {
            struct Node {

            };

            NFA() = default;
            NFA(NFA const &);
            NFA(NFA &&) = default;
            NFA &operator=(NFA const &) = default;
            NFA &operator=(NFA &&) = default;
            ~NFA();

            void connect_to_firstpos(NFA const &other);

            typedef std::pair<std::size_t, std::size_t> Match;
            typedef std::set<Match> Matches;
            typedef std::optional<Matches> MatchResult;
            MatchResult traverse();

            NFA from_ast(AST const &ast);
            NFA from_ast_node(AST::Node const &ast_node);
            // just traverse, traverse, fullmatch, lmatch, amatch,
        };

        struct DFA {
            struct Node {
            };
            // minimize, simulate, has nothing, has empty, fullmatch, lmatch, amatch
        };

        std::optional<std::string> regex;
        std::optional<NFA> nfa;
        std::optional<DFA> dfa;

        Regex(Regex const &);
        Regex(Regex &&);
        Regex(std::string regex);

        static Regex for_nothing();
        static Regex for_empty();
        static Regex for_char(char chr);
        static Regex for_dot();
        static Regex for_small_d();
        static Regex for_small_s();
        static Regex for_small_w();

        Regex &numbered_cgroup();
        Regex &named_cgroup(std::string name);
        Regex &non_cgroup();
        Regex &qmark();
        Regex &star();
        Regex &plus();
        Regex &power(int power);
        Regex &min(int min);
        Regex &max(int max);
        Regex &range(int min, int max);
        Regex &concat(Regex other);
        Regex &union_(Regex other);
        Regex &percent(Regex other);

        Regex numbered_cgroup() const;
        Regex named_cgroup(std::string name) const;
        Regex non_cgroup() const;
        Regex qmark() const;
        Regex star() const;
        Regex plus() const;
        Regex power(int power) const;
        Regex min(int min) const;
        Regex max(int max) const;
        Regex range(int min, int max) const;
        Regex concat(Regex const &other) const;
        Regex union_(Regex const &other) const;
        Regex percent(Regex const &other) const;
    };
}

#endif //PYREX_NEW_REGEX_H

#ifndef PYREX_NEW_REGEX_H
#define PYREX_NEW_REGEX_H

#include <optional>
#include <set>
#include <list>
#include <map>
#include <unordered_map>
#include "ast.h"

/* TODO:
 *  - determine which methods receive Regex, Regex const & and Regex &&.
 *  - similarly for NFA and DFA
 *  - make Regex::NFA::all_nodes std::list<std::shared_ptr<Node>>
 *  - consider adding information whether there is also a way to match
 *    without going through a group
 */

namespace pyrex {
    struct Regex {
    private:
        struct Group {
            std::shared_ptr<AST::Node> const group;
        };

        struct NFA {
            inline static std::size_t const MAX_NUM_OF_NODES = 100000;

            enum class GroupToken {
                ENTER, LEAVE,
            };

            typedef std::unordered_map<Group *, std::vector<GroupToken>> GroupToTokens;
            struct Node;
            typedef std::unordered_map<char, std::map<Node *, GroupToTokens>> Edges;
            typedef std::optional<GroupToTokens> EpsilonEdge;

            struct Node {
                Edges edges;
                EpsilonEdge epsilon_edge;

                void connect_to_firstpos(NFA const &nfa);
                void clear();
            };

            Node start_node;
            std::list<Node *> all_nodes;
            std::list<Node *> lastpos;

            NFA() = default;
            NFA(NFA const &);
            NFA(NFA &&) = default;
            NFA &operator=(NFA const &) = default;
            NFA &operator=(NFA &&) = default;
            ~NFA() noexcept;

            void connect_to_firstpos(NFA const &other);

            typedef std::pair<std::size_t, std::size_t> Match;
            typedef std::set<Match> Matches;
            typedef std::optional<Matches> MatchResult;
            MatchResult traverse(std::string const &text, Group *group);

            static NFA from_ast(AST const &ast);
            static NFA from_ast_node(std::shared_ptr<AST::Node> const &ast_node);

            static NFA for_nothing();
            static NFA for_empty();
            static NFA for_char(char chr);
            static NFA for_dot();
            static NFA for_small_d();
            static NFA for_small_s();
            static NFA for_small_w();

            NFA &for_non_cgroup();
            NFA &for_numbered_cgroup();
            NFA &for_named_cgroup();
            NFA &for_group(Group *group);
            NFA &qmark();
            NFA &star();
            NFA &plus();
            NFA &power(int power);
            NFA &min(int min);
            NFA &max(int max);
            NFA &range(int min, int max);
            NFA &concatenate(NFA other);
            NFA &union_(NFA other);
            NFA &percent(NFA other);
            // just traverse, traverse, fullmatch, lmatch, amatch,
        };

        struct DFA {
            inline static std::size_t const MAX_NUM_OF_NODES = 1000000;

            struct Node {
            };
            // minimize, simulate, has nothing, has empty, fullmatch, lmatch, amatch
        };

        AST ast;
        std::optional<std::string> regex;
        std::optional<NFA> nfa;
        std::optional<DFA> dfa;
        std::optional<std::list<Group>> numbered_groups;
        std::optional<std::map<std::string, Group>> named_groups;

    public:
        Regex(Regex const &);
        Regex(Regex &&);
        Regex(std::string regex);

        void compile();
        std::string to_string();

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

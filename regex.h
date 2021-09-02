#ifndef PYREX_REGEX_H
#define PYREX_REGEX_H

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
        enum class MatchType {
            LMATCH, RMATCH, FMATCH, AMATCH,
        };

    private:
        struct Group {
            std::shared_ptr<AST::Node> const group;
        };

        struct NFA {
            inline static std::size_t const MAX_NUM_OF_NODES = 100000;

            enum class GroupToken {
                ENTER, LEAVE,
            };

            typedef std::unordered_map<AST::Group const *, std::list<GroupToken>> GroupToTokens;
            struct Node;
            typedef std::unordered_map<char, std::list<Node *>> Edges;

            struct Node {
                std::unordered_map<char, std::set<Node *>> edges;
                std::unordered_map<Node *, GroupToTokens> node_to_groups;
                std::optional<GroupToTokens> epsilon_edge;

                void clear();
            };

        private:
            Node start_node;
            std::list<Node *> all_nodes;
            std::list<Node *> lastpos;
            std::size_t size = 0;

        public:
            NFA() = default;
            NFA(NFA const &);
            NFA(NFA &&) = default;
            NFA &operator=(NFA const &) = default;
            NFA &operator=(NFA &&) = default;
            ~NFA() noexcept;

            bool match(std::string const &text, MatchType match_type) const;

            typedef std::pair<std::size_t, std::size_t> Match;
            typedef std::set<Match> Matches;
            typedef std::optional<Matches> MatchResult;
            MatchResult submatches(
                std::string const &text, MatchType match_type, AST::Group *group
            ) const;

            static NFA from_ast(AST const &ast);

        private:
            static NFA from_ast_node(
                std::shared_ptr<AST::Node> const &ast_node,
                AST::NumberedCGroups::const_iterator &numbered_group_it,
                AST::NamedCGroups const &named_cgroups
            );

            static NFA for_nothing();
            static NFA for_empty();
            static NFA for_char(char chr);
            static NFA for_dot();
            static NFA for_small_d();
            static NFA for_small_s();
            static NFA for_small_w();

            NFA &for_group(AST::Group const *group);
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
        // we would like to build groups without building nfa first:

        explicit Regex(AST ast);

    public:
        Regex(Regex const &);
        Regex(Regex &&) = default;
        explicit Regex(std::string regex);

        void compile();
        std::string to_string();
        Regex operator[](std::size_t group_name) const;
        Regex operator[](std::string const &group_name) const;

        static Regex for_nothing();
        static Regex for_empty();
        static Regex for_char(char chr);
        static Regex for_dot();
        static Regex for_small_d();
        static Regex for_small_s();
        static Regex for_small_w();

        Regex numbered_cgroup() const;
        Regex named_cgroup(std::string const &name) const;
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

#endif //PYREX_REGEX_H

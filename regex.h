#ifndef PYREX_REGEX_H
#define PYREX_REGEX_H

#include <optional>
#include <set>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "ast.h"

/* TODO:
 *  - determine which methods receive Regex, Regex const & and Regex &&.
 *  - similarly for NFA and DFA
 *  - make Regex::NFA::all_nodes std::list<std::shared_ptr<Node>>
 *  - consider adding information whether there is also a way to match
 *    without going through a group
 */

namespace pyrex {
    static int counter = 0; // TODO: remove

    struct Regex {
        enum class MatchType {
            LMATCH, RMATCH, FMATCH, AMATCH,
        };

        typedef std::pair<std::size_t, std::size_t> Match;
        typedef std::set<Match> Matches;
        typedef std::optional<Matches> MatchResult;

    private:
        struct NFA {
            inline static std::size_t const MAX_NUM_OF_NODES = 100000;

            enum class GroupToken {
                ENTER, LEAVE,
            };

            struct GroupInfo {
                std::list<GroupToken> tokens;
                bool optional_path = false;
            };

            typedef std::unordered_map<AST::Group const *, GroupInfo> GroupToTokens;

            struct Node {
                std::unordered_map<char, std::set<Node const *>> edges_by_char;
                std::unordered_map<Node const *, GroupToTokens> edges;
                std::optional<GroupToTokens> epsilon_edge;

                std::string chr_; // TODO: remove
                int id; // TODO: remove

                void clear();
            };

        private:
            Node start_node{.id = 0};
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
            MatchResult submatches(
                std::string const &text, MatchType match_type, AST::Group const *group
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
        mutable std::optional<std::string> regex;
        mutable std::optional<NFA> nfa;
        mutable std::optional<DFA> dfa;
        // we would like to build groups without building nfa first:

        explicit Regex(AST ast);

        NFA const &get_nfa() const;

    public:
        Regex(Regex const &);
        Regex(Regex &&) = default;
        explicit Regex(std::string regex);

        void compile();
        std::string to_string() const;
        Regex operator[](std::size_t group_name) const;
        Regex operator[](std::string const &group_name) const;

        bool fmatch(std::string const &text) const;
        bool lmatch(std::string const &text) const;
        bool rmatch(std::string const &text) const;
        bool amatch(std::string const &text) const;

        MatchResult fsubmatches(std::string const &text, std::size_t group_number) const;
        MatchResult fsubmatches(std::string const &text, std::string const &group_name) const;
        MatchResult lsubmatches(std::string const &text, std::size_t group_number) const;
        MatchResult lsubmatches(std::string const &text, std::string const &group_name) const;
        MatchResult rsubmatches(std::string const &text, std::size_t group_number) const;
        MatchResult rsubmatches(std::string const &text, std::string const &group_name) const;
        MatchResult asubmatches(std::string const &text, std::size_t group_number) const;
        MatchResult asubmatches(std::string const &text, std::string const &group_name) const;

        AST::Group const *get_group(std::size_t group_number) const;
        AST::Group const *get_group(std::string const &group_name) const;

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

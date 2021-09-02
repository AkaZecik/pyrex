#include "ast.h"

#include <utility>

namespace pyrex {
    AST::NumberedCGroups AST::combine_numbered_cgroups(
        const NumberedCGroups &left, const NumberedCGroups &right
    ) {
        NumberedCGroups numbered_cgroups;
        numbered_cgroups.reserve(left.size() + right.size());
        auto end = std::copy(left.cbegin(), left.cend(), numbered_cgroups.begin());
        std::copy(right.cbegin(), right.cend(), end);
        return numbered_cgroups;
    }

    AST::NamedCGroups AST::combine_named_cgroups(
        const NamedCGroups &left, const NamedCGroups &right
    ) {
        for (auto &[name, _] : left) {
            if (right.find(name) != right.cend()) {
                throw std::runtime_error(std::string("Duplicate named group: ").append(name));
            }
        }

        NamedCGroups named_cgroups(left);
        named_cgroups.insert(right.cbegin(), right.cend());
        return named_cgroups;
    }

    AST::AST(std::shared_ptr<Node> root) : root{std::move(root)} {}

    AST::AST(std::shared_ptr<Node> root,
             NumberedCGroups numbered_cgroups,
             NamedCGroups named_cgroups)
        : root{std::move(root)},
          numbered_cgroups{numbered_cgroups},
          named_cgroups{named_cgroups} {}

    std::shared_ptr<AST::Node> AST::get_root() const {
        return root;
    }

    AST::NumberedCGroups const &AST::get_numbered_cgroups() const {
        if (!numbered_cgroups) {
            throw std::runtime_error("Numbered groups are not computed");
        }

        return *numbered_cgroups;
    }

    AST::NamedCGroups const &AST::get_named_cgroups() const {
        if (!named_cgroups) {
            throw std::runtime_error("Named groups are not computed");
        }

        return *named_cgroups;
    }

    std::string AST::to_string() const {
        return root->to_string();
    }

    AST AST::from_regex(std::string const &regex) {
        Parser parser(regex);
        return parser.parse();
    }

    AST AST::for_nothing() {
        return AST{std::make_shared<NothingNode>()};
    }

    AST AST::for_empty() {
        return AST{std::make_shared<EmptyNode>()};
    }

    AST AST::for_char(char c) {
        return AST{std::make_shared<CharNode>(c)};
    }

    AST AST::for_dot() {
        return AST{std::make_shared<DotNode>()};
    }

    AST AST::for_small_d() {
        return AST{std::make_shared<SmallDNode>()};
    }

    AST AST::for_small_s() {
        return AST{std::make_shared<SmallSNode>()};
    }

    AST AST::for_small_w() {
        return AST{std::make_shared<SmallWNode>()};
    }

    AST AST::numbered_cgroup(AST const &ast) {
        auto node = std::make_shared<NumberedCGroupNode>(ast.root);
        NumberedCGroups numbered_cgroups(*ast.numbered_cgroups);
        numbered_cgroups.push_back({node});
        return AST{node, std::move(numbered_cgroups), *ast.named_cgroups};
    }

    AST AST::named_cgroup(AST const &ast, const std::string &name) {
        if (ast.named_cgroups->find(name) != ast.named_cgroups->cend()) {
            throw std::runtime_error(
                std::string("Named group \"").append(name).append("\" already exists"));
        }

        auto node = std::make_shared<NamedCGroupNode>(ast.root, name);
        NamedCGroups named_cgroups(*ast.named_cgroups);
        named_cgroups[name] = {node};
        return AST{node, *ast.numbered_cgroups, std::move(named_cgroups)};
    }

    AST AST::non_cgroup(AST const &ast) {
        return AST{
            std::make_shared<NonCGroupNode>(ast.root),
            *ast.numbered_cgroups,
            *ast.named_cgroups
        };
    }

    AST AST::qmark(AST const &ast) {
        return AST{
            std::make_shared<QMarkNode>(ast.root),
            *ast.numbered_cgroups,
            *ast.named_cgroups
        };
    }

    AST AST::star(AST const &ast) {
        return AST{std::make_shared<StarNode>(ast.root)};
    }

    AST AST::plus(AST const &ast) {
        return AST{std::make_shared<PlusNode>(ast.root)};
    }

    AST AST::power(AST const &ast, int power) {
        return AST{std::make_shared<PowerNode>(ast.root, power)};
    }

    AST AST::min(AST const &ast, int min) {
        return AST{std::make_shared<MinNode>(ast.root, min)};
    }

    AST AST::max(AST const &ast, int max) {
        return AST{std::make_shared<MaxNode>(ast.root, max)};
    }

    AST AST::range(AST const &ast, int min, int max) {
        return AST{std::make_shared<RangeNode>(ast.root, min, max)};
    }

    AST AST::concat(AST const &left, AST const &right) {
        return AST{
            std::make_shared<ConcatNode>(left.root, right.root),
            combine_numbered_cgroups(*left.numbered_cgroups, *right.numbered_cgroups),
            combine_named_cgroups(*left.named_cgroups, *right.named_cgroups)
        };
    }

    AST AST::union_(AST const &left, AST const &right) {
        return AST{
            std::make_shared<UnionNode>(left.root, right.root),
            combine_numbered_cgroups(*left.numbered_cgroups, *right.numbered_cgroups),
            combine_named_cgroups(*left.named_cgroups, *right.named_cgroups)
        };
    }

    AST AST::percent(AST const &left, AST const &right) {
        return AST{
            std::make_shared<PercentNode>(left.root, right.root),
            combine_numbered_cgroups(*left.numbered_cgroups, *right.numbered_cgroups),
            combine_named_cgroups(*left.named_cgroups, *right.named_cgroups)
        };
    }
}
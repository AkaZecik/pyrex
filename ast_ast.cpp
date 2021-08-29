#include "ast.h"

#include <utility>

namespace pyrex {
    AST::AST(std::shared_ptr<Node> root) : root(std::move(root)) {}

    AST::Node *AST::get_root() {
        return root.get();
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

    AST AST::qmark(AST const &ast) {
        return AST{std::make_shared<QMarkNode>(ast.root)};
    }

    AST AST::concat(AST const &left, AST const &right) {
        return AST{std::make_shared<ConcatNode>(left.root, right.root)};
    }

    AST AST::union_(AST const &left, AST const &right) {
        return AST{std::make_shared<UnionNode>(left.root, right.root)};
    }

    AST AST::percent(AST const &left, AST const &right) {
        return AST{std::make_shared<PercentNode>(left.root, right.root)};
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
}
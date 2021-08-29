#include "ast.h"

#include <memory>
#include <utility>

namespace pyrex {
    /****************************
     *        Leaf nodes        *
     ****************************/

    AST::Node::Kind AST::NothingNode::kind() {
        return AST::Node::Kind::NOTHING;
    }

    std::string AST::NothingNode::to_string() {
        return "\\#";
    }

    AST::Node::Kind AST::EmptyNode::kind() {
        return AST::Node::Kind::EMPTY;
    }

    std::string AST::EmptyNode::to_string() {
        return "\\e";
    }

    AST::CharNode::CharNode(char chr) : value(chr) {}

    AST::Node::Kind AST::CharNode::kind() {
        return Node::Kind::CHAR;
    }

    std::string AST::CharNode::to_string() {
        // TODO
        return "";
    }

    AST::Node::Kind AST::DotNode::kind() {
        return AST::Node::Kind::DOT;
    }

    std::string AST::DotNode::to_string() {
        return ".";
    }

    AST::Node::Kind AST::SmallDNode::kind() {
        return Node::Kind::SMALL_D;
    }

    std::string AST::SmallDNode::to_string() {
        return "\\d";
    }

    AST::Node::Kind AST::SmallSNode::kind() {
        return Node::Kind::SMALL_S;
    }

    std::string AST::SmallSNode::to_string() {
        return "\\s";
    }

    AST::Node::Kind AST::SmallWNode::kind() {
        return Node::Kind::SMALL_W;
    }

    std::string AST::SmallWNode::to_string() {
        return "\\w";
    }

    /*****************************
     *        Group nodes        *
     *****************************/

    AST::GroupNode::GroupNode(std::shared_ptr<Node> node) : operand(std::move(node)) {}

    AST::InternalNode::Type AST::GroupNode::internal_node_type() {
        return InternalNode::Type::GROUP;
    }

    int AST::GroupNode::arity() {
        return 1;
    }

    AST::Node::Kind AST::NumberedCGroupNode::kind() {
        return Node::Kind::NUMBERED_CGROUP;
    }

    std::string AST::NumberedCGroupNode::to_string() {
        return std::string("(").append(operand->to_string()).append(")");
    }

    AST::Node::Kind AST::NamedCGroupNode::kind() {
        return Node::Kind::NAMED_CGROUP;
    }

    std::string AST::NamedCGroupNode::to_string() {
        return std::string("(?P<").append(name).append(")");
    }

    AST::Node::Kind AST::NonCGroupNode::kind() {
        return Node::Kind::NON_CGROUP;
    }

    std::string AST::NonCGroupNode::to_string() {
        return std::string("(?:").append(operand->to_string()).append(")");
    }

    /***************************
     *        Operators        *
     ***************************/

    AST::InternalNode::Type AST::Operator::internal_node_type() {
        return InternalNode::Type::OPERATOR;
    }

    AST::UnaryOperator::UnaryOperator(std::shared_ptr<Node> node) : operand(std::move(node)) {}

    int AST::UnaryOperator::arity() {
        return 1;
    }

    std::string AST::UnaryOperator::to_string() {

    }

    /*********************************
     *        Unary operators        *
     *********************************/

    AST::Node::Kind AST::QMarkNode::kind() {
        return Node::Kind::QMARK;
    }

    int AST::QMarkNode::precedence() {
        return 1;
    }


}
#include "ast.h"

#include <memory>
#include <utility>
#include <sstream>

/*
 * TODO:
 *  - check for correctness of std::string AST::CharNode::to_string(), specifically of \xNN escapes
 */

namespace pyrex {
    /****************************
     *        Leaf nodes        *
     ****************************/

    int AST::LeafNode::precedence() {
        return 0;
    }

    AST::Node::Kind AST::NothingNode::kind() const {
        return AST::Node::Kind::NOTHING;
    }

    std::string AST::NothingNode::to_string() {
        return "\\#";
    }

    AST::Node::Kind AST::EmptyNode::kind() const {
        return AST::Node::Kind::EMPTY;
    }

    std::string AST::EmptyNode::to_string() {
        return "\\e";
    }

    AST::CharNode::CharNode(char chr) : chr{chr} {}

    AST::Node::Kind AST::CharNode::kind() const {
        return Node::Kind::CHAR;
    }

    std::string AST::CharNode::to_string() {
        switch (chr) {
            case '\n':
                return "\\n";
            case '\r':
                return "\\r";
            case '\f':
                return "\\f";
            case '\t':
                return "\\t";
            case '\\':
                return "\\\\";
            case '.':
                return "\\.";
            case '(':
                return "\\(";
            case ')':
                return "\\)";
            case '{':
                return "\\{";
            case '}':
                return "\\}";
            case '?':
                return "\\?";
            case '*':
                return "\\*";
            case '+':
                return "\\+";
            case '%':
                return "\\%";
            case '|':
                return "\\|";
            default: {
                if (' ' <= chr && chr <= '~') {
                    return {chr};
                } else {
                    std::ostringstream oss;
                    oss << "\\x" << std::hex << chr;
                    return oss.str();
                }
            }
        }
    }

    AST::Node::Kind AST::DotNode::kind() const {
        return AST::Node::Kind::DOT;
    }

    std::string AST::DotNode::to_string() {
        return ".";
    }

    AST::Node::Kind AST::SmallDNode::kind() const {
        return Node::Kind::SMALL_D;
    }

    std::string AST::SmallDNode::to_string() {
        return "\\d";
    }

    AST::Node::Kind AST::SmallSNode::kind() const {
        return Node::Kind::SMALL_S;
    }

    std::string AST::SmallSNode::to_string() {
        return "\\s";
    }

    AST::Node::Kind AST::SmallWNode::kind() const {
        return Node::Kind::SMALL_W;
    }

    std::string AST::SmallWNode::to_string() {
        return "\\w";
    }

    /*****************************
     *        Group nodes        *
     *****************************/

    AST::GroupNode::GroupNode(std::shared_ptr<Node> node) : operand{std::move(node)} {}

    int AST::GroupNode::precedence() {
        return 0;
    }

    AST::InternalNode::Type AST::GroupNode::internal_node_type() {
        return InternalNode::Type::GROUP;
    }

    int AST::GroupNode::arity() {
        return 1;
    }

    AST::Node::Kind AST::NumberedCGroupNode::kind() const {
        return Node::Kind::NUMBERED_CGROUP;
    }

    std::string AST::NumberedCGroupNode::to_string() {
        return std::string("(").append(operand->to_string()).append(")");
    }

    AST::NamedCGroupNode::NamedCGroupNode(std::shared_ptr<Node> node, std::string name)
        : GroupNode{std::move(node)}, name{std::move(name)} {}

    AST::Node::Kind AST::NamedCGroupNode::kind() const {
        return Node::Kind::NAMED_CGROUP;
    }

    std::string AST::NamedCGroupNode::to_string() {
        return std::string("(?P<").append(name).append(">").append(operand->to_string())
            .append(")");
    }

    AST::Node::Kind AST::NonCGroupNode::kind() const {
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

    AST::UnaryOperator::UnaryOperator(std::shared_ptr<Node> node) : operand{std::move(node)} {}

    int AST::UnaryOperator::arity() {
        return 1;
    }

    std::string AST::UnaryOperator::to_string() {
        if (operand->precedence() > precedence()) {
            return std::string("(?:").append(operand->to_string()).append(")")
                .append(operator_repr());
        } else {
            return operand->to_string().append(operator_repr());
        }
    }

    AST::BinaryOperator::BinaryOperator(
        std::shared_ptr<Node> left_node, std::shared_ptr<Node> right_node
    ) : left_operand{std::move(left_node)}, right_operand{std::move(right_node)} {}

    int AST::BinaryOperator::arity() {
        return 2;
    }

    std::string AST::BinaryOperator::to_string() {
        std::string left, right;

        if (left_operand->precedence() > precedence()) {
            left = std::string("(?:").append(left_operand->to_string()).append(")");
        } else {
            left = left_operand->to_string();
        }

        if (right_operand->precedence() > precedence()) {
            right = std::string("(?:").append(right_operand->to_string()).append(")");
        } else {
            right = right_operand->to_string();
        }

        return left.append(operator_repr()).append(right);
    }

    /*********************************
     *        Unary operators        *
     *********************************/

    AST::Node::Kind AST::QMarkNode::kind() const {
        return Node::Kind::QMARK;
    }

    int AST::QMarkNode::precedence() {
        return 1;
    }

    std::string AST::QMarkNode::operator_repr() {
        return "?";
    }

    AST::Node::Kind AST::StarNode::kind() const {
        return Node::Kind::STAR;
    }

    int AST::StarNode::precedence() {
        return 1;
    }

    std::string AST::StarNode::operator_repr() {
        return "*";
    }

    AST::Node::Kind AST::PlusNode::kind() const {
        return Node::Kind::PLUS;
    }

    int AST::PlusNode::precedence() {
        return 1;
    }

    std::string AST::PlusNode::operator_repr() {
        return "+";
    }

    AST::PowerNode::PowerNode(std::shared_ptr<Node> node, int power)
        : UnaryOperator{std::move(node)}, power{power} {}

    AST::Node::Kind AST::PowerNode::kind() const {
        return Node::Kind::POWER;
    }

    int AST::PowerNode::precedence() {
        return 1;
    }

    std::string AST::PowerNode::operator_repr() {
        return std::string("{").append(std::to_string(power)).append("}");
    }

    AST::MinNode::MinNode(std::shared_ptr<Node> node, int min)
        : UnaryOperator{std::move(node)}, min{min} {}

    AST::Node::Kind AST::MinNode::kind() const {
        return Node::Kind::MIN;
    }

    int AST::MinNode::precedence() {
        return 1;
    }

    std::string AST::MinNode::operator_repr() {
        return std::string("{").append(std::to_string(min)).append(",}");
    }

    AST::MaxNode::MaxNode(std::shared_ptr<Node> node, int max)
        : UnaryOperator{std::move(node)}, max{max} {}

    AST::Node::Kind AST::MaxNode::kind() const {
        return Node::Kind::MAX;
    }

    int AST::MaxNode::precedence() {
        return 1;
    }

    std::string AST::MaxNode::operator_repr() {
        return std::string("{,").append(std::to_string(max)).append("}");
    }

    AST::RangeNode::RangeNode(std::shared_ptr<Node> node, int min, int max)
        : UnaryOperator{std::move(node)}, min{min}, max{max} {}

    AST::Node::Kind AST::RangeNode::kind() const {
        return Node::Kind::RANGE;
    }

    int AST::RangeNode::precedence() {
        return 1;
    }

    std::string AST::RangeNode::operator_repr() {
        return std::string("{").append(std::to_string(min)).append(",")
            .append(std::to_string(max)).append("}");
    }

    /**********************************
     *        Binary operators        *
     **********************************/

    AST::Node::Kind AST::ConcatNode::kind() const {
        return Node::Kind::CONCAT;
    }

    int AST::ConcatNode::precedence() {
        return 2;
    }

    std::string AST::ConcatNode::operator_repr() {
        return "";
    }

    AST::Node::Kind AST::PercentNode::kind() const {
        return Node::Kind::PERCENT;
    }

    int AST::PercentNode::precedence() {
        return 3;
    }

    std::string AST::PercentNode::operator_repr() {
        return "%";
    }

    AST::Node::Kind AST::UnionNode::kind() const {
        return Node::Kind::UNION;
    }

    int AST::UnionNode::precedence() {
        return 4;
    }

    std::string AST::UnionNode::operator_repr() {
        return "|";
    }
}

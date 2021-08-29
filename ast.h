#ifndef PYREX_AST_H
#define PYREX_AST_H

#include <utility>
#include <memory>

namespace pyrex {
    struct AST {
        struct Node {
            enum class Kind {
                /* leaves */
                NOTHING,
                EMPTY,
                CHAR,
                DOT,
                SMALL_D,
                SMALL_S,
                SMALL_W,

                /* groups */
                NUMBERED_CGROUP,
                NAMED_CGROUP,
                NON_CGROUP,

                /* unary operators */
                QMARK,
                STAR,
                PLUS,
                POWER,
                MIN,
                MAX,
                RANGE,

                /* binary operators */
                CONCAT,
                UNION,
                PERCENT,
            };

            virtual Kind kind() = 0;
            virtual std::string to_string() = 0;
        };

        /****************************
         *        Leaf nodes        *
         ****************************/

        struct LeafNode : Node {
        };

        struct NothingNode : LeafNode {
            Kind kind() override;
            std::string to_string() override;
        };

        struct EmptyNode : LeafNode {
            Kind kind() override;
            std::string to_string() override;
        };

        struct CharNode : LeafNode {
            char const value;

            explicit CharNode(char);

            Kind kind() override;
            std::string to_string() override;
        };

        struct DotNode : LeafNode {
            Kind kind() override;
            std::string to_string() override;
        };

        struct SmallDNode : LeafNode {
            Kind kind() override;
            std::string to_string() override;
        };

        struct SmallSNode : LeafNode {
            Kind kind() override;
            std::string to_string() override;
        };

        struct SmallWNode : LeafNode {
            Kind kind() override;
            std::string to_string() override;
        };

        /********************************
         *        Internal nodes        *
         ********************************/

        struct InternalNode : Node {
            enum class Type {
                GROUP, OPERATOR,
            };

            virtual int arity() = 0;

            virtual Type internal_node_type() = 0;
        };

        /*****************************
         *        Group nodes        *
         *****************************/

        struct GroupNode : InternalNode {
            std::shared_ptr<Node> operand;

            explicit GroupNode(std::shared_ptr<Node>);

            InternalNode::Type internal_node_type() override;
            int arity() override;
        };

        struct NumberedCGroupNode : GroupNode {
            using GroupNode::GroupNode;

            Kind kind() override;
            std::string to_string() override;
        };

        struct NamedCGroupNode : GroupNode {
            using GroupNode::GroupNode;

            std::string name;

            Kind kind() override;
            std::string to_string() override;
        };

        struct NonCGroupNode : GroupNode {
            using GroupNode::GroupNode;

            Kind kind() override;
            std::string to_string() override;
        };

        /***************************
         *        Operators        *
         ***************************/

        struct Operator : InternalNode {
            virtual int precedence() = 0;
            InternalNode::Type internal_node_type() override;
        };

        struct UnaryOperator : Operator {
            std::shared_ptr<Node> operand;

            explicit UnaryOperator(std::shared_ptr<Node>);

            int arity() override;
            std::string to_string() override;
        };

        struct BinaryOperator : Operator {
            std::shared_ptr<Node> left_operand, right_operand;

            BinaryOperator(std::shared_ptr<Node>, std::shared_ptr<Node>);

            int arity() override;
            std::string to_string() override;
        };

        /*********************************
         *        Unary operators        *
         *********************************/

        struct QMarkNode : UnaryOperator {
            using UnaryOperator::UnaryOperator;

            Kind kind() override;
            int precedence() override;
            std::string to_string() override;
        };

        struct StarNode : UnaryOperator {
            using UnaryOperator::UnaryOperator;

            Kind kind() override;
            int precedence() override;
            std::string to_string() override;
        };

        struct PlusNode : UnaryOperator {
            using UnaryOperator::UnaryOperator;

            Kind kind() override;
            int precedence() override;
            std::string to_string() override;
        };

        struct PowerNode : UnaryOperator {
            int power;

            PowerNode(std::shared_ptr<Node>, int);

            Kind kind() override;
            int precedence() override;
            std::string to_string() override;
        };

        struct MinNode : UnaryOperator {
            int min;

            MinNode(std::shared_ptr<Node>, int);

            Kind kind() override;
            int precedence() override;
            std::string to_string() override;
        };

        struct MaxNode : UnaryOperator {
            int max;

            MaxNode(std::shared_ptr<Node>, int);

            Kind kind() override;
            int precedence() override;
            std::string to_string() override;
        };

        struct RangeNode : UnaryOperator {
            int min;
            int max;

            RangeNode(std::shared_ptr<Node>, int, int);

            Kind kind() override;
            int precedence() override;
            std::string to_string() override;
        };

        /**********************************
         *        Binary operators        *
         **********************************/

        struct ConcatNode : BinaryOperator {
            using BinaryOperator::BinaryOperator;

            Kind kind() override;
            int precedence() override;
            std::string to_string() override;
        };

        struct UnionNode : BinaryOperator {
            using BinaryOperator::BinaryOperator;

            Kind kind() override;
            int precedence() override;
            std::string to_string() override;
        };

        struct PercentNode : BinaryOperator {
            using BinaryOperator::BinaryOperator;

            Kind kind() override;
            int precedence() override;
            std::string to_string() override;
        };

    private:
        std::shared_ptr<Node> const root;

        explicit AST(std::shared_ptr<Node>);

    public:
        AST(AST const &) = default;
        AST(AST &&) = default;

        Node *get_root();

        static AST for_nothing();
        static AST for_empty();
        static AST for_char(char c);
        static AST for_dot();
        static AST for_small_d();
        static AST for_small_s();
        static AST for_small_w();
        static AST concat(AST const &left, AST const &right);
        static AST union_(AST const &left, AST const &right);
        static AST percent(AST const &left, AST const &right);
        static AST star(AST const &ast);
        static AST plus(AST const &ast);
        static AST power(AST const &ast, int power);
        static AST min(AST const &ast, int min);
        static AST max(AST const &ast, int max);
        static AST range(AST const &ast, int min, int max);
        static AST qmark(AST const &ast);
    };


    struct Parser; // TODO
}

#endif //PYREX_AST_H

#ifndef PYREX_AST_H
#define PYREX_AST_H

#include <utility>
#include <memory>
#include <vector>

/* TODO:
 *  - handle duplicate group names
 *  - make AST::Node operands const
 */

namespace pyrex {
    struct AST {
    public:
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
                PERCENT,
                UNION,
            };

            virtual Kind kind() = 0;
            virtual int precedence() = 0;
            virtual std::string to_string() = 0;
        };

        /****************************
         *        Leaf nodes        *
         ****************************/

        struct LeafNode : Node {
            int precedence() override;
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
            char const chr;

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

            explicit GroupNode(std::shared_ptr<Node> node);

            int precedence() override;
            InternalNode::Type internal_node_type() override;
            int arity() override;
        };

        struct NumberedCGroupNode : GroupNode {
            using GroupNode::GroupNode;

            Kind kind() override;
            std::string to_string() override;
        };

        struct NamedCGroupNode : GroupNode {
            std::string const name;

            NamedCGroupNode(std::shared_ptr<Node> node, std::string name);

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
            InternalNode::Type internal_node_type() override;
            virtual std::string operator_repr() = 0;
        };

        struct UnaryOperator : Operator {
            std::shared_ptr<Node> operand;

            explicit UnaryOperator(std::shared_ptr<Node> node);

            int arity() override;
            std::string to_string() override;
        };

        struct BinaryOperator : Operator {
            std::shared_ptr<Node> left_operand, right_operand;

            BinaryOperator(std::shared_ptr<Node> left_node, std::shared_ptr<Node> right_node);

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
            std::string operator_repr() override;
        };

        struct StarNode : UnaryOperator {
            using UnaryOperator::UnaryOperator;

            Kind kind() override;
            int precedence() override;
            std::string operator_repr() override;
        };

        struct PlusNode : UnaryOperator {
            using UnaryOperator::UnaryOperator;

            Kind kind() override;
            int precedence() override;
            std::string operator_repr() override;
        };

        struct PowerNode : UnaryOperator {
            int const power;

            PowerNode(std::shared_ptr<Node>, int power);

            Kind kind() override;
            int precedence() override;
            std::string operator_repr() override;
        };

        struct MinNode : UnaryOperator {
            int const min;

            MinNode(std::shared_ptr<Node> node, int min);

            Kind kind() override;
            int precedence() override;
            std::string operator_repr() override;
        };

        struct MaxNode : UnaryOperator {
            int const max;

            MaxNode(std::shared_ptr<Node> node, int max);

            Kind kind() override;
            int precedence() override;
            std::string operator_repr() override;
        };

        struct RangeNode : UnaryOperator {
            int const min;
            int const max;

            RangeNode(std::shared_ptr<Node> node, int min, int max);

            Kind kind() override;
            int precedence() override;
            std::string operator_repr() override;
        };

        /**********************************
         *        Binary operators        *
         **********************************/

        struct ConcatNode : BinaryOperator {
            using BinaryOperator::BinaryOperator;

            Kind kind() override;
            int precedence() override;
            std::string operator_repr() override;
        };

        struct PercentNode : BinaryOperator {
            using BinaryOperator::BinaryOperator;

            Kind kind() override;
            int precedence() override;
            std::string operator_repr() override;
        };

        struct UnionNode : BinaryOperator {
            using BinaryOperator::BinaryOperator;

            Kind kind() override;
            int precedence() override;
            std::string operator_repr() override;
        };

    private:
        std::shared_ptr<Node> const root;

        explicit AST(std::shared_ptr<Node> root);

    public:
        AST(AST const &) = default;
        AST(AST &&) = default;

        Node *get_root();

        static AST from_regex(std::string const &regex);
        [[nodiscard]] std::string to_string() const;

        static AST for_nothing();
        static AST for_empty();
        static AST for_char(char chr);
        static AST for_dot();
        static AST for_small_d();
        static AST for_small_s();
        static AST for_small_w();
        static AST numbered_cgroup(AST const &ast);
        static AST named_cgroup(AST const &ast, const std::string& name);
        static AST non_cgroup(AST const &ast);
        static AST qmark(AST const &ast);
        static AST star(AST const &ast);
        static AST plus(AST const &ast);
        static AST power(AST const &ast, int power);
        static AST min(AST const &ast, int min);
        static AST max(AST const &ast, int max);
        static AST range(AST const &ast, int min, int max);
        static AST concat(AST const &left, AST const &right);
        static AST union_(AST const &left, AST const &right);
        static AST percent(AST const &left, AST const &right);

    private:
        struct Parser {
            enum class TokenType {
                LPAREN,
                RPAREN,
                LCURLY,
                RCURLY,
                STAR,
                PLUS,
                UNION,
                PERCENT,
                QMARK,
                CHAR,
                DIGIT,
                DOT,
                SMALL_D,
                SMALL_S,
                SMALL_W,
                EMPTY,
                NOTHING,
                END,
            };

            struct Token {
                TokenType const type;
                char const value{};

                explicit Token(TokenType type);
                Token(TokenType type, char value);
            };

            struct Tokenizer {
                std::string const &regex;
                std::size_t curr_pos;

                explicit Tokenizer(std::string const &regex);
                Tokenizer() = delete;
                Tokenizer(Tokenizer const &) = delete;
                Tokenizer(Tokenizer &&) = delete;

                std::vector<Token> get_all_tokens();
                Token get_token();
                Token parse_escape();
                char parse_hex();
            };

            std::string const &regex;
            std::vector<Token> all_tokens;
            std::vector<AST> results;
            std::vector<std::shared_ptr<AST::InternalNode>> stack;
            std::size_t curr_pos;
            bool concat_insertable;

            explicit Parser(std::string const &regex);
            Parser() = delete;
            Parser(Parser const &) = delete;
            Parser(Parser &&) = delete;

            AST parse();
            inline bool can_insert_concat();
            static inline bool after_concat(Token token);
            void parse_range();
            void parse_group();
            std::string parse_identifier();
            static bool identifier_start_char(char chr);
            static bool identifier_char(char chr);
            void push_node(std::shared_ptr<InternalNode> &&internal_node);
            void interpret_operator(std::shared_ptr<Operator> &&op);
            void drop_operators_precedence(int precedence);
            void drop_operators_until_group();
        };
    };
}

#endif //PYREX_AST_H

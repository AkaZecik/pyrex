//#ifndef AST_CPP
//#define AST_CPP
//
//#include <utility>
//#include <memory>
//#include "ast.h"
//
//
//namespace pyrex {
//    AST::CharNode::CharNode(char chr) : chr(chr) {}
//
//    struct AST {
//        struct Node {
//            enum class Kind {
//                NOTHING,
//                EMPTY,
//                CHAR,
//                DOT,
//                SMALL_D,
//                BIG_D,
//                SMALL_S,
//                BIG_S,
//                SMALL_W,
//                BIG_W,
//                NUMBERED_CGROUP,
//                NAMED_CGROUP,
//                NON_CGROUP,
//                STAR,
//                PLUS,
//                POWER,
//                MIN,
//                MAX,
//                RANGE,
//                QMARK,
//                CONCAT,
//                UNION,
//                PERCENT,
//            };
//
//            virtual Kind node_kind() = 0;
//        };
//
//        struct LeafNode : Node {
//        };
//
//        struct CharNode : LeafNode {
//            char chr;
//
//            explicit CharNode(char chr) : chr(chr) {}
//
//            Kind node_kind() override {
//                return Kind::CHAR;
//            }
//        };
//
//        struct DotNode : LeafNode {
//            Kind node_kind() override {
//                return Kind::DOT;
//            }
//        };
//
//        struct SmallDNode : LeafNode {
//            Kind node_kind() override {
//                return Kind::SMALL_D;
//            }
//        };
//
//        struct BigDNode : LeafNode {
//            Kind node_kind() override {
//                return Kind::BIG_D;
//            }
//        };
//
//        struct SmallSNode : LeafNode {
//            Kind node_kind() override {
//                return Kind::SMALL_S;
//            }
//        };
//
//        struct BigSNode : LeafNode {
//            Kind node_kind() override {
//                return Kind::BIG_S;
//            }
//        };
//
//        struct SmallWNode : LeafNode {
//            Kind node_kind() override {
//                return Kind::SMALL_W;
//            }
//        };
//
//        struct BigWNode : LeafNode {
//            Kind node_kind() override {
//                return Kind::BIG_W;
//            }
//        };
//
//        struct EmptyNode : LeafNode {
//            Kind node_kind() override {
//                return Kind::EMPTY;
//            }
//        };
//
//        struct NothingNode : LeafNode {
//            Kind node_kind() override {
//                return Kind::NOTHING;
//            }
//        };
//
//        struct InternalNode : Node {
//            enum class Type {
//                GROUP, OPERATOR,
//            };
//
//            virtual int arity() = 0;
//
//            virtual Type internal_node_type() = 0;
//        };
//
//        struct GroupNode : InternalNode {
//            std::shared_ptr<Node> operand;
//
//            explicit GroupNode(std::shared_ptr<Node> node) : operand(std::move(node)) {}
//        };
//
//        struct NumberedCGroupNode : InternalNode {
//            Kind node_kind() override {
//                return Kind::NUMBERED_CGROUP;
//            }
//
//            InternalNode::Type internal_node_type() override {
//                return InternalNode::Type::GROUP;
//            }
//
//            int arity() override {
//                return 1;
//            }
//        };
//
//        struct Operator : InternalNode {
//            virtual int precedence() = 0;
//
//            InternalNode::Type internal_node_type() override {
//                return InternalNode::Type::OPERATOR;
//            }
//        };
//
//        struct UnaryOperator : Operator {
//            std::shared_ptr<Node> operand;
//
//            explicit UnaryOperator(std::shared_ptr<Node> node) : operand(std::move(node)) {}
//
//            int arity() override {
//                return 1;
//            }
//        };
//
//        struct BinaryOperator : Operator {
//            std::shared_ptr<Node> left_operand, right_operand;
//
//            BinaryOperator(std::shared_ptr<Node> left_node, std::shared_ptr<Node> right_node)
//                : left_operand(std::move(left_node)), right_operand(std::move(right_node)) {}
//
//            int arity() override {
//                return 2;
//            }
//        };
//
//        struct StarNode : UnaryOperator {
//            using UnaryOperator::UnaryOperator;
//
//            Kind node_kind() override {
//                return Kind::STAR;
//            }
//
//            int precedence() override {
//                return 1;
//            }
//        };
//
//        struct PlusNode : UnaryOperator {
//            using UnaryOperator::UnaryOperator;
//
//            Kind node_kind() override {
//                return Kind::PLUS;
//            }
//
//            int precedence() override {
//                return 1;
//            }
//        };
//
//        struct PowerNode : UnaryOperator {
//            int power;
//
//            PowerNode(std::shared_ptr<Node> node, int power)
//                : UnaryOperator{std::move(node)}, power(power) {}
//
//            Kind node_kind() override {
//                return Kind::POWER;
//            }
//
//            int precedence() override {
//                return 1;
//            }
//        };
//
//        struct MinNode : UnaryOperator {
//            int min;
//
//            MinNode(std::shared_ptr<Node> node, int min)
//                : UnaryOperator{std::move(node)}, min(min) {}
//
//            Kind node_kind() override {
//                return Kind::MIN;
//            }
//
//            int precedence() override {
//                return 1;
//            }
//        };
//
//        struct MaxNode : UnaryOperator {
//            int max;
//
//            MaxNode(std::shared_ptr<Node> node, int max)
//                : UnaryOperator{std::move(node)}, max(max) {}
//
//            Kind node_kind() override {
//                return Kind::MAX;
//            }
//
//            int precedence() override {
//                return 1;
//            }
//        };
//
//        struct RangeNode : UnaryOperator {
//            int min;
//            int max;
//
//            RangeNode(std::shared_ptr<Node> node, int min, int max)
//                : UnaryOperator{std::move(node)}, min(min), max(max) {}
//
//            Kind node_kind() override {
//                return Kind::RANGE;
//            }
//
//            int precedence() override {
//                return 1;
//            }
//        };
//
//        struct QMarkNode : UnaryOperator {
//            using UnaryOperator::UnaryOperator;
//
//            Kind node_kind() override {
//                return Kind::QMARK;
//            }
//
//            int precedence() override {
//                return 1;
//            }
//        };
//
//        struct ConcatNode : BinaryOperator {
//            using BinaryOperator::BinaryOperator;
//
//            Kind node_kind() override {
//                return Kind::CONCAT;
//            }
//
//            int precedence() override {
//                return 2;
//            }
//        };
//
//        struct UnionNode : BinaryOperator {
//            using BinaryOperator::BinaryOperator;
//
//            Kind node_kind() override {
//                return Kind::UNION;
//            }
//
//            int precedence() override {
//                return 3;
//            }
//        };
//
//        struct PercentNode : BinaryOperator {
//            using BinaryOperator::BinaryOperator;
//
//            Kind node_kind() override {
//                return Kind::PERCENT;
//            }
//
//            int precedence() override {
//                return 100; // TODO: decide what precedence it should have
//            }
//        };
//
//    private:
//        std::shared_ptr<Node> const root;
//
//        explicit AST(std::shared_ptr<Node> root) : root(std::move(root)) {}
//
//    public:
//        // TODO: remember to handle combining ASTs with duplicate group names
//        AST(AST const &) = default;
//
//        AST(AST &&) = default;
//
//        Node *get_root() {
//            return root.get();
//        }
//
//        static AST for_nothing() {
//            return AST{std::make_shared<NothingNode>()};
//        }
//
//        static AST for_empty() {
//            return AST{std::make_shared<EmptyNode>()};
//        }
//
//        static AST for_char(char c) {
//            return AST{std::make_shared<CharNode>(c)};
//        }
//
//        static AST concat(AST const &left, AST const &right) {
//            return AST{std::make_shared<ConcatNode>(left.root, right.root)};
//        }
//
//        static AST union_(AST const &left, AST const &right) {
//            return AST{std::make_shared<UnionNode>(left.root, right.root)};
//        }
//
//        static AST percent(AST const &left, AST const &right) {
//            return AST{std::make_shared<PercentNode>(left.root, right.root)};
//        }
//
//        static AST star(AST const &ast) {
//            return AST{std::make_shared<StarNode>(ast.root)};
//        }
//
//        static AST plus(AST const &ast) {
//            return AST{std::make_shared<PlusNode>(ast.root)};
//        }
//
//        static AST power(AST const &ast, int power) {
//            return AST{std::make_shared<PowerNode>(ast.root, power)};
//        }
//
//        static AST min(AST const &ast, int min) {
//            return AST{std::make_shared<MinNode>(ast.root, min)};
//        }
//
//        static AST max(AST const &ast, int max) {
//            return AST{std::make_shared<MaxNode>(ast.root, max)};
//        }
//
//        static AST range(AST const &ast, int min, int max) {
//            return AST{std::make_shared<RangeNode>(ast.root, min, max)};
//        }
//
//        static AST qmark(AST const &ast) {
//            return AST{std::make_shared<QMarkNode>(ast.root)};
//        }
//    };
//}
//
//#endif // AST_CPP

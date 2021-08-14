//
// Created by bercik on 14.08.2021.
//

#include <set>
#include "ast.cpp"

bool nullable(Node *node) {
    switch (node->node_kind()) {
        case NodeKind::CHAR: return false;
        case NodeKind::GROUP: {
            auto group = reinterpret_cast<Group *>(node);
            return nullable(group->operand);
        }
    }
}

std::set<int> first_pos(Node *node) {
    return {};
}

std::set<int> last_pos(Node *node) {
    return {};
}

void build_nfa(Node *node) {

}
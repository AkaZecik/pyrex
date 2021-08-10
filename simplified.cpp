//
// Created by bercik on 10.08.2021.
//

#include <iostream>
#include <utility>
#include <vector>
#include <set>

/*
 * 0 - *
 * 1 - concat
 * 2 - union
 */

enum class NodeType {
    STAR, UNION, CONCAT, EMPTY, CHAR
};

struct Node {
    NodeType type;
    union {
        Node *operand;
        struct {
            Node *left_operand;
            Node *right_operand;
        } children;
        struct {
            char c;
            int id;
        } leaf;
    } value;
};

Node *build_tree(std::vector<char> const &onp) {
    std::vector<Node *> results;
    int id = 0;

    for (char c : onp) {
        if (c == '*') {
            if (results.empty()) {
                throw std::runtime_error("star applied to nothing!");
            }

            auto back = results.back();
            results.pop_back();
            results.push_back(new Node{NodeType::STAR, {.operand = back}});
        } else if (c == '.' || c == '|') {
            if (results.size() < 2) {
                throw std::runtime_error("not enough operands for concat!");
            }

            auto right = results.back();
            results.pop_back();
            auto left = results.back();
            results.pop_back();
            results.push_back(
                new Node{(c == '.' ? NodeType::CONCAT : NodeType::UNION),
                         {.children = {
                             .left_operand = left,
                             .right_operand = right}}});
        } else if (c == 'e') {
            results.push_back(new Node{NodeType::EMPTY});
        } else {
            results.push_back(new Node{NodeType::CHAR,
                                       {.leaf = {.c = c, .id = id}}});
        }
    }

    if (results.size() > 1) {
        throw std::runtime_error("too little operators!");
    } else if (results.empty()) {
        throw std::runtime_error("regex is empty???");
    }

    return results[0];
}


bool before_concat(char c) {
    return c == ')' || c == '*' || 'a' <= c && c <= 'z';
}

bool after_concat(char c) {
    return c == '(' || 'a' <= c && c <= 'z';
}

bool can_insert_concat(char before, char after) {
    return before_concat(before) && after_concat(after);
}

void verify_regex(std::string const &regex) {
    for (char c : regex) {
        if (c != '(' && c != ')' && c != '*' && c != '|' && !('a' <= c && c <= 'z')) {
            throw std::runtime_error("Your regex contains invalid characters!");
        }
    }
}


std::vector<char> get_onp(std::string const &regex) {
    std::vector<char> results;
    std::vector<char> operators;

    for (int i = 0; i < regex.size(); ++i) {
        if (i > 0 && can_insert_concat(regex[i - 1], regex[i])) {
            while (!operators.empty() &&
                   (operators.back() == '*' || operators.back() == '.')) {
                results.push_back(operators.back());
                operators.pop_back();
            }

            operators.push_back('.');
        }

        char c = regex[i];

        if (c == '(') {
            operators.push_back(c);
        } else if (c == ')') {
            while (!operators.empty() && operators.back() != '(') {
                results.push_back(operators.back());
                operators.pop_back();
            }

            if (operators.empty()) {
                throw std::runtime_error("too many closing brackets!");
            }

            operators.pop_back();
        } else if (c == '|') {
            while (!operators.empty() &&
                   (operators.back() == '.' || operators.back() == '|')) {
                results.push_back(operators.back());
                operators.pop_back();
            }

            operators.push_back(c);
        } else {
            results.push_back(c);
        }
    }

    while (!operators.empty()) {
        auto back = operators.back();
        operators.pop_back();

        if (back == '(') {
            throw std::runtime_error("not enough closing brackets!");
        }

        results.push_back(back);
    }

    return results;
}

Node *parse(std::string const &regex) {
    verify_regex(regex);
    auto onp = get_onp(regex);
    auto result = build_tree(onp);
    return result;
}

bool nullable(Node *node) {
    if (node->type == NodeType::STAR || node->type == NodeType::EMPTY) {
        return true;
    } else if (node->type == NodeType::CONCAT) {
        return nullable(node->value.children.left_operand) && nullable(node->value.children.right_operand);
    } else if (node->type == NodeType::UNION) {
        return nullable(node->value.children.left_operand) || nullable(node->value.children.right_operand);
    } else {
        return false;
    }
}

std::set<int> first_pos(Node *node) {
    if (node->type == NodeType::STAR) {
        return first_pos(node->value.operand);
    } else if (node->type == NodeType::CONCAT) {
        std::set<int> result;

        if (nullable(node->value.children.left_operand)) {
            auto first = first_pos(node->value.children.left_operand);
            auto second = first_pos(node->value.children.right_operand);
            result.insert(first.begin(), first.end());
            result.insert(second.begin(), second.end());
            return result;
        } else {
            return first_pos(node->value.children.left_operand);
        }
    } else if (node->type == NodeType::UNION) {
        std::set<int> result;
        auto first = first_pos(node->value.children.left_operand);
        auto second = first_pos(node->value.children.right_operand);
        result.insert(first.begin(), first.end());
        result.insert(second.begin(), second.end());
        return result;
    } else if (node->type == NodeType::EMPTY) {
        return {};
    } else {
        return std::set<int>({node->value.leaf.id});
    }
}

std::set<int> last_pos(Node *node) {
    if (node->type == NodeType::STAR) {
        return last_pos(node->value.operand);
    } else if (node->type == NodeType::CONCAT) {
        std::set<int> result;

        if (nullable(node->value.children.right_operand)) {
            auto first = last_pos(node->value.children.left_operand);
            auto second = last_pos(node->value.children.right_operand);
            result.insert(first.begin(), first.end());
            result.insert(second.begin(), second.end());
            return result;
        } else {
            return last_pos(node->value.children.right_operand);
        }
    } else if (node->type == NodeType::UNION) {
        std::set<int> result;
        auto first = last_pos(node->value.children.left_operand);
        auto second = last_pos(node->value.children.right_operand);
        result.insert(first.begin(), first.end());
        result.insert(second.begin(), second.end());
        return result;
    } else if (node->type == NodeType::EMPTY) {
        return {};
    } else {
        return std::set<int>({node->value.leaf.id});
    }
}

int main() {
    std::string regex;
    std::cin >> regex;
    auto onp = get_onp(regex);

    for (char c : onp) {
        std::cout << c << " ";
    }

    std::cout << std::endl;
}
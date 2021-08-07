//
// Created by bercik on 07.08.2021.
//

#include <string>
#include <iostream>
#include <vector>

inline int operator_priority(char c) {
    switch (c) {
        case '+':
        case '-':
            return 0;
        case '*':
        case '/':
            return 1;
        case '^':
            return 2;
        default:
            throw;
    }
}

enum class Associativity {
    LEFT, RIGHT,
};

inline Associativity operator_associativity(char c) {
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
            return Associativity::LEFT;
        case '^':
            return Associativity::RIGHT;
        default:
            throw;
    }
}

long long exp(long long base, long long exp) {
    long long result = 1;

    while (exp) {
        if (exp & 1) {
            result *= base;
        }
        base *= base;
        exp /= 2;
    }

    return result;
}

long long evaluate(char op, long long l, long long r) {
    switch (op) {
        case '+':
            return l + r;
        case '-':
            return l - r;
        case '*':
            return l * r;
        case '/':
            return l / r;
        case '^':
            return exp(l, r);
        default:
            throw;
    }
}

int main() {
    std::string input;
    std::cin >> input;

    std::vector<long long> results;
    std::vector<char> operators;

    auto internal_evaluate = [&results, &operators]() {
        long long r = results.back();
        results.pop_back();
        long long l = results.back();
        results.pop_back();
        char op = operators.back();
        operators.pop_back();
        results.push_back(evaluate(op, l, r));
    };

    for (int i = 0; i < input.size();) {
        char c = input[i];

        if ('0' <= c && c <= '9') {
            int j = i + 1;

            while (j < input.size() && '0' <= input[j] && input[j] <= '9') {
                ++j;
            }

            results.push_back(std::stoll(input.substr(i, j)));
            i = j;
            continue;
        } else if (c == '(') {
            operators.push_back(c);
        } else if (c == ')') {

            while (!operators.empty()) {
                if (operators.back() == '(') {
                    operators.pop_back();
                    break;
                }

                internal_evaluate();
            }
        } else {
            while (!operators.empty() && operators.back() != '(') {
                Associativity curr_op = operator_associativity(c);
                int curr_pr = operator_priority(c);
                int other_pr = operator_priority(operators.back());

                if (curr_op == Associativity::LEFT ? curr_pr <= other_pr : curr_pr < other_pr) {
                    internal_evaluate();
                } else {
                    break;
                }
            }

            operators.push_back(c);
        }

        ++i;
    }

    while (!operators.empty()) {
        internal_evaluate();
    }

    std::cout << results.back() << std::endl;
    return 0;
}
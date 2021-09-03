#include "regex.h"
#include <iostream>

int main() {
    std::cout << std::boolalpha;
    std::vector<std::pair<std::string, std::vector<std::string>>> tests;

    // tests.push_back({"a", {"", "a", "aa", "aaaaaaaa"}});
    // tests.push_back({"A.+n", {
    //     "Artur zda egzamin",
    //     "Artur nie zda egzaminu",
    //     "artur egzamin",
    //     "A.+n",
    // }});
    // tests.push_back({"((?:a)*)*b*",{
    //     "aaaaaaaaaaaaaaaaaaa",
    //     "",
    //     "aaaaaaabbbbbbbb",
    //     "bbbbbbbbb",
    //     "ab",
    //     "a",
    //     "b",
    //     "ba",
    //     "bbbbbaaaaa",
    // }});
    tests.push_back({"((ab?)|ba?bb)*abb", {
        "",
        "abb",
        "ababb",
        "baabb",
        "aaaaaaaaaabb",
        "babb",
        "bbabb",
        "bbbabb",
        "bbbbabb",
        "abbbaaaaabaabbbabaabb",
    }});

    for (auto &[regex, texts] : tests) {
        pyrex::counter = 0;
        std::cout << "regex: " << regex << std::endl;

        pyrex::Regex Regex(regex);

        std::cout << "fmatch:" << std::endl;

        for (auto &text : texts) {
            std::cout << "  " << text << " -> " << Regex.fmatch(text) << std::endl;
        }

        std::cout << "lmatch:" << std::endl;

        for (auto &text : texts) {
            std::cout << "  " << text << " -> " << Regex.lmatch(text) << std::endl;
        }

        std::cout << "amatch:" << std::endl;

        for (auto &text : texts) {
            std::cout << "  " << text << " -> " << Regex.amatch(text) << std::endl;
        }

        std::cout << "rmatch:" << std::endl;

        for (auto &text : texts) {
            std::cout << "  " << text << " -> " << Regex.rmatch(text) << std::endl;
        }

        std::cout << std::endl << "-------------------------------------------------\n\n";
    }
}
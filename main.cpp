#include "regex.h"
#include <iostream>

void test_match() {
    std::vector<std::pair<std::string, std::vector<std::string>>> tests;

    tests.push_back({"a", {"", "a", "aa", "aaaaaaaa"}});
    tests.push_back({"A.+n", {
        "Artur zda egzamin",
        "Artur nie zda egzaminu",
        "artur egzamin",
        "A.+n",
        }});
    tests.push_back({"((?:a)*)*b*",{
        "aaaaaaaaaaaaaaaaaaa",
        "",
        "aaaaaaabbbbbbbb",
        "bbbbbbbbb",
        "ab",
        "a",
        "b",
        "ba",
        "bbbbbaaaaa",
        }});
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
        Regex.compile();

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

void test_submatch() {
    std::vector<std::pair<std::string, std::vector<std::string>>> tests;

    pyrex::Regex regex("(a*)*");
    regex.compile();
    std::vector<std::string> texts({
        // "",
        // "a",
        // "aa",
        "aaaaaaaaaa",
    });

    /* ((((ab)?){2})*)*
     * (((((ab)?)*){2})*)*
     *
     * (((?:ab)?)*)*
     * ((ab)*)*
     * ((?:ab|(\e))*)*
     * ((?:(ab)*|(\e))*)*
     */

    /*
     * po kazdej operacji, kazda krawedz do stanu firstpos, jezeli ma grupe,
     * to ostatni token na tej grupie to ENTER
     * jesli wierzcholek ma krawedz epsilonowa, to ostatni token na tej krawedzi to LEAVE
     * jesli na krawedzi jest OPTIONAL_LEAVE, to jest zaraz po niej ENTER
     * na kazdej krawedzi wewnetrznej (nie start->firstpos, nie lastpos->end) po kazdej
     * krawedzi LEAVE jest krawedz ENTER,
     * na kazdej krawedzi wewnetrzej jest maksymalnie jeden leave?
     * jak popatrzymy sie na numery przy ENTER_X, ENTER_Y, LEAVE_Y, LEAVE_X,
     * to te numery sa ciagiem liczb, gdzie roznica miedzy kolejnymi liczbami to 1, 0 lub -1
     * jesli krawedz z lastpos do firstpos nie istnieje, to moze sie pojawic
     * tylko po zrobieniu operacji star
     *
     * moje algo rozwiazuje szerszy problem: dane jest wyrazenie regularne, gdzie nawiasy
     * sa oznaczane tagami/labelami. Tagi/labele moga sie powtarzac. Moj algorytm zwraca
     * wszystkie podslowa, ktore moga byc rozpoznane przez grupy z tym tagiem, wszystko na raz.
     */

    std::cout << regex.to_string() << std::endl;

    for (auto &text : texts) {
        std::cout << text << std::endl;
        std::cout << "fsubmatches:" << std::endl;
        auto result = regex.fsubmatches(text, 0);

        if (result) {
            std::cout << "num of matches: " << result->size() << std::endl;

            for (auto [start, end] : *result) {
                std::cout << start << " " << end << std::endl;
            }
        } else {
            std::cout << "No matches!" << std::endl;
        }
    }
}

int main() {
    std::cout << std::boolalpha;

    // test_match();
    test_submatch();
}
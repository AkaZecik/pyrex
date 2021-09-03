#include "regex.h"

namespace pyrex {
    bool Regex::fmatch(std::string const &text) const {
        return get_nfa().match(text, MatchType::FMATCH);
    }

    bool Regex::lmatch(std::string const &text) const {
        return get_nfa().match(text, MatchType::LMATCH);
    }

    bool Regex::rmatch(std::string const &text) const {
        return get_nfa().match(text, MatchType::RMATCH);
    }

    bool Regex::amatch(std::string const &text) const {
        return get_nfa().match(text, MatchType::AMATCH);
    }

    Regex::MatchResult
    Regex::fsubmatches(std::string const &text, std::size_t group_number) const {
        return get_nfa().submatches(text, MatchType::FMATCH, get_group(group_number));
    }

    Regex::MatchResult
    Regex::fsubmatches(std::string const &text, std::string const &group_name) const {
        return get_nfa().submatches(text, MatchType::FMATCH, get_group(group_name));
    }

    Regex::MatchResult
    Regex::lsubmatches(std::string const &text, std::size_t group_number) const {
        return get_nfa().submatches(text, MatchType::LMATCH, get_group(group_number));
    }

    Regex::MatchResult
    Regex::lsubmatches(std::string const &text, std::string const &group_name) const {
        return get_nfa().submatches(text, MatchType::LMATCH, get_group(group_name));
    }

    Regex::MatchResult
    Regex::rsubmatches(std::string const &text, std::size_t group_number) const {
        return get_nfa().submatches(text, MatchType::RMATCH, get_group(group_number));
    }

    Regex::MatchResult
    Regex::rsubmatches(std::string const &text, std::string const &group_name) const {
        return get_nfa().submatches(text, MatchType::RMATCH, get_group(group_name));
    }

    Regex::MatchResult
    Regex::asubmatches(std::string const &text, std::size_t group_number) const {
        return get_nfa().submatches(text, MatchType::AMATCH, get_group(group_number));
    }

    Regex::MatchResult
    Regex::asubmatches(std::string const &text, std::string const &group_name) const {
        return get_nfa().submatches(text, MatchType::AMATCH, get_group(group_name));
    }

    AST::Group const *Regex::get_group(std::size_t group_number) const {
        auto &numbered_cgroups = ast.get_numbered_cgroups();

        if (group_number > numbered_cgroups.size()) {
            throw std::runtime_error("Group number too big");
        }

        return &numbered_cgroups[group_number];
    }

    AST::Group const *Regex::get_group(std::string const &group_name) const {
        auto &named_cgroups = ast.get_named_cgroups();
        auto group_it = named_cgroups.find(group_name);

        if (group_it == named_cgroups.cend()) {
            throw std::runtime_error("Named group doesn't exist");
        }

        return &group_it->second;
    }
}
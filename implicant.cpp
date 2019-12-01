#include "implicant.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

Implicant::Implicant() : num_ones(0), done(false) {}

Implicant::Implicant(std::string bin_rep)
    : done(false), bin_rep(bin_rep) {
    num_ones = std::count(bin_rep.begin(), bin_rep.end(), '1');
}

void Implicant::add_minterm(unsigned int minterm) {
    minterms.insert(minterm);
}

void Implicant::add_minterms(const std::set<unsigned int> &other_minterms) {
    for (auto minterm : other_minterms) {
        minterms.insert(minterm);
    }
}

bool Implicant::is_true() const {
    return bin_rep == std::string(bin_rep.length(), '-');
}

std::string Implicant::to_string() const {
    std::stringstream ss;
    ss << "[";
    for (auto it = minterms.begin(); it != --minterms.end(); ++it) {
        ss << *it;
        ss << ", ";
    }
    ss << *--minterms.end() << "] " << bin_rep << (done ? "*" : "");
    return ss.str();
}

bool operator<(const Implicant &a, const Implicant &b) {
    return a.bin_rep < b.bin_rep;
}

bool operator==(const Implicant &a, const Implicant &b) {
    return a.bin_rep == b.bin_rep;
}

bool operator!=(const Implicant &a, const Implicant &b) {
    return !(a == b);
}

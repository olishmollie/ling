#include "implicant.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

Implicant::Implicant() : num_ones(0), done(false) {}

Implicant::Implicant(std::string bin_rep)
    : bin_rep(bin_rep), done(false) {
    num_ones = std::count(bin_rep.begin(), bin_rep.end(), '1');
}

bool Implicant::is_true() const {
    return bin_rep == std::string(bin_rep.length(), '-');
}

std::string Implicant::to_string() const {
    return bin_rep + (done ? "*" : "");
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

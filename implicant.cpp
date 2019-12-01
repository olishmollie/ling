#include "implicant.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

Implicant::Implicant() : num_ones(0), done(false) {}

int count_ones(std::string str) {
    return std::count(str.begin(), str.end(), '1');
}

Implicant::Implicant(std::string bin_rep)
    : bin_rep(bin_rep), num_ones(count_ones(bin_rep)), done(false) {}

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

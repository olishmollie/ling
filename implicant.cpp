#include "implicant.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

using std::count;
using std::set;
using std::string;
using std::stringstream;

Implicant new_implicant() {
    Implicant impl;
    impl.done = false;
    return impl;
}

Implicant new_implicant(const string bit_str) {
    Implicant impl = new_implicant();
    impl.bit_str = bit_str;
    return impl;
}

unsigned int num_ones(const Implicant &self) {
    return count(self.bit_str.begin(), self.bit_str.end(), '1');
}

void add_minterm(Implicant &self, const unsigned int minterm) {
    self.minterms.insert(minterm);
}

void add_minterms(Implicant &self, const set<unsigned int> &other_minterms) {
    for (auto minterm : other_minterms) {
        self.minterms.insert(minterm);
    }
}

bool is_true(const Implicant &self) {
    return self.bit_str == string(self.bit_str.length(), '-');
}

string to_string(const Implicant &self) {
    stringstream ss;
    ss << "[";
    for (auto it = self.minterms.begin(); it != --self.minterms.end(); ++it) {
        ss << *it;
        ss << ", ";
    }
    ss << *--self.minterms.end() << "] " << self.bit_str
       << (self.done ? "*" : "");
    return ss.str();
}

bool operator<(const Implicant &a, const Implicant &b) {
    return a.bit_str < b.bit_str;
}

bool operator==(const Implicant &a, const Implicant &b) {
    return a.bit_str == b.bit_str;
}

bool operator!=(const Implicant &a, const Implicant &b) {
    return !(a == b);
}

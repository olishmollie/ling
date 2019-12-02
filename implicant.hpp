#ifndef _IMPLICANT_H
#define _IMPLICANT_H

#include <string>
#include <set>

struct Implicant {
    bool done;

    std::set<unsigned int> minterms;
    std::string bin_rep;

    Implicant();
    Implicant(std::string bin_rep);

    unsigned int num_ones() const;
    void add_minterm(unsigned int minterm);
    void add_minterms(const std::set<unsigned int> &other_minterms);
    bool is_true() const;
    std::string to_string() const;
};

bool operator<(const Implicant &a, const Implicant &b);
bool operator==(const Implicant &a, const Implicant &b);
bool operator!=(const Implicant &a, const Implicant &b);

#endif

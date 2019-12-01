#ifndef _IMPLICANT_H
#define _IMPLICANT_H

#include <string>

struct Implicant {
    std::string bin_rep;
    int num_ones;
    bool done;
    Implicant();
    Implicant(std::string bin_rep);
    bool is_true() const;
    std::string to_string() const;
};

bool operator<(const Implicant &a, const Implicant &b);
bool operator==(const Implicant &a, const Implicant &b);
bool operator!=(const Implicant &a, const Implicant &b);

#endif

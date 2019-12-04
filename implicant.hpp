#ifndef _IMPLICANT_H
#define _IMPLICANT_H

#include <set>
#include <string>

struct Implicant {
    bool done;

    std::set<unsigned int> minterms;
    std::string bit_str;
};

Implicant new_implicant(std::string bit_str);

unsigned int num_ones(const Implicant &self);
void add_minterm(Implicant &self, const unsigned int minterm);
void add_minterms(Implicant &self,
                  const std::set<unsigned int> &other_minterms);

bool is_true(const Implicant &self);
std::string to_string(const Implicant &self);

bool operator<(const Implicant &a, const Implicant &b);
bool operator==(const Implicant &a, const Implicant &b);
bool operator!=(const Implicant &a, const Implicant &b);

#endif

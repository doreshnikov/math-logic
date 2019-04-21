#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include "../grammar/grammar.hpp"

class proof_splitter {
public:
    proof_splitter();

    s_ptr parse_proof();
    head const &get_head();

    static unsigned int is_axiom(e_ptr const &);

private:
    proof _proof;

    std::string _line;

    void parse_head();
    void get_line();
};

class proof_builder {
public:
    proof_builder(head const &, s_ptr const &);

    void print();

private:
    head _context;
    s_ptr _root;
};

#endif // UTILS_HPP

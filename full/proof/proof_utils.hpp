#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include "../grammar/grammar.hpp"

class proof_scanner {
public:
    proof_scanner();

    s_ptr parse_proof();
    proof const &get_proof() const;
    head const &get_head() const;

    static unsigned int is_axiom(e_ptr const &);

private:
    proof _proof;

    std::string _line;

    void parse_head();
    void get_line();
};

class proof_printer {
public:
    proof_printer(head const &, s_ptr const &);

    typedef statement_collector::print_policy print_policy;
    void print(print_policy const &) const;

private:
    head _context;
    s_ptr _root;
};

#endif // UTILS_HPP

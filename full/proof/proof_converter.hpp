#ifndef PROOF_CONVERTER_HPP
#define PROOF_CONVERTER_HPP

#include <vector>

#include "../grammar/grammar.hpp"
#include "proof_utils.hpp"

class intuitionistic_converter {

public:

    static s_ptr produce_axiom(unsigned int ax, e_ptr const &e1, e_ptr const &e2) {
        target.add_statement(axiom())
    }

    static s_ptr produce_axiom(unsigned int ax, e_ptr const &e1, e_ptr const &e2, e_ptr const &e3) {

    }

    static proof to_intuitionistic(proof const &seq) {
        proof pint;

        for (unsigned int i = 0; i < seq.length(); i++) {
            s_ptr s = seq[i];
            if (proof_splitter::is_axiom(s->get_expression())) {
                pint.add_statement(s);
            }
        }
    }


};

#endif // PROOF_CONVERTER_HPP

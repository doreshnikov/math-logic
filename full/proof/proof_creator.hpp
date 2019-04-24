#ifndef PROOF_CREATOR_HPP
#define PROOF_CREATOR_HPP

#include "../grammar/grammar.hpp"
#include "proof_translator.hpp"

namespace translators {

    static inline proof prove_ex_falso(e_ptr const &e1, e_ptr const &e2) {
        proof tmp;
        head context;
        e_ptr ne1(new negation(e1));
        e_ptr ne2(new negation(e2));
        e_ptr nne2(new negation(ne2));

        context.add_hypothesis(e1);
        context.add_hypothesis(ne1);
        context.set_result(e2);
        tmp.set_head(context);

        unsigned int id = 0;
        tmp.add_hypothesis(e1, id++);
        tmp.add_hypothesis(ne1, id++);

        tmp.add_axiom(produce_axiom(9, ne2, e1), id++, 9);
        tmp.add_axiom(produce_axiom(1, e1, ne2), id++, 1);
        tmp.add_axiom(produce_axiom(1, ne1, ne2), id++, 1);

        tmp.add_modus_ponens(e_ptr(new implication(ne2, e1)), id++);
        e_ptr imp(new implication(ne2, ne1));
        tmp.add_modus_ponens(imp, id++);
        tmp.add_modus_ponens(e_ptr(new implication(imp, nne2)), id++);
        tmp.add_modus_ponens(nne2, id++);

        tmp.add_axiom(produce_axiom(10, e2), id++, 10);
        tmp.add_modus_ponens(e2, id++);
        return tmp;
    }

    static inline void add_deducted_ex_falso(proof &target, e_ptr const &e1, e_ptr const &e2, unsigned int &id) {
        proof d = to_deduction(prove_ex_falso(e1, e2), e1);
        for (unsigned int i = 0; i < d.length(); i++) {
            e_ptr expr = d[i]->get_expression();
            if (d[i]->get_type() == 'a') {
                target.add_axiom(expr, id++, a_cast(d[i])->get_number());
            } else if (d[i]->get_type() == 'h') {
                target.add_hypothesis(expr, id++);
            } else {
                target.add_modus_ponens(expr, id++);
            }
        }
    }

    static inline proof prove_disjunction(e_ptr const &e1, e_ptr const &e2, bool n1, bool n2) {
        proof target;
        head context;
        e_ptr ne1(new negation(e1));
        e_ptr ne2(new negation(e2));
        context.add_hypothesis(n1 ? ne1 : e1);
        context.add_hypothesis(n2 ? ne2 : e2);

        e_ptr result(new disjunction(e1, e2));
        e_ptr neg(new negation(result));
        if (!n1 || !n2) {
            context.set_result(result);
        } else {
            context.set_result(e_ptr(new negation(result)));
        }
        target.set_head(context);
        unsigned int id = 0;

        if (!n1) {
            target.add_axiom(produce_axiom(6, e1, e2), id++, 6);
            target.add_hypothesis(e1, id++);
            target.add_modus_ponens(result, id++);
        } else if (!n2) {
            target.add_axiom(produce_axiom(7, e1, e2), id++, 7);
            target.add_hypothesis(e2, id++);
            target.add_modus_ponens(result, id++);
        } else {
            target.add_hypothesis(ne1, id++);
            target.add_hypothesis(ne2, id++);

            add_identity(target, e1, id);
            add_deducted_ex_falso(target, e2, e1, id);
            e_ptr ax1 = produce_axiom(8, e1, e2, e1);
            target.add_axiom(ax1, id++, 8);

            e_ptr r1 = i_cast(ax1)->get_right();
            target.add_modus_ponens(r1, id++);
            target.add_modus_ponens(i_cast(r1)->get_right(), id++);

            e_ptr ax2 = produce_axiom(9, result, e1);
            target.add_axiom(ax2, id++, 9);
            e_ptr ax3 = produce_axiom(1, ne1, result);
            target.add_axiom(ax3, id++, 1);
            target.add_modus_ponens(i_cast(ax3)->get_right(), id++);
            target.add_modus_ponens(i_cast(ax2)->get_right(), id++);
            target.add_modus_ponens(neg, id++);
        }

        return target;
    }

    static inline proof prove_conjunction(e_ptr const &e1, e_ptr const &e2, bool n1, bool n2) {
        proof target;
        head context;
        e_ptr ne1(new negation(e1));
        e_ptr ne2(new negation(e2));
        context.add_hypothesis(n1 ? ne1 : e1);
        context.add_hypothesis(n2 ? ne2 : e2);

        e_ptr result(new conjunction(e1, e2));
        e_ptr neg(new negation(result));
        if (!n1 && !n2) {
            context.set_result(result);
        } else {
            context.set_result(e_ptr(new negation(result)));
        }
        target.set_head(context);
        unsigned int id = 0;

        if (!n1 && !n2) {
            target.add_axiom(produce_axiom(3, e1, e2), id++, 6);
            target.add_hypothesis(e1, id++);
            target.add_modus_ponens(e_ptr(new implication(e2, result)), id++);
            target.add_hypothesis(e2, id++);
            target.add_modus_ponens(result, id++);
        } else if (n1) {
            target.add_axiom(produce_axiom(9, result, e1), id++, 9);
            target.add_axiom(produce_axiom(4, e1, e2), id++, 4);

            target.add_axiom(produce_axiom(1, ne1, result), id++, 1);
            target.add_hypothesis(ne1, id++);
            e_ptr imp(new implication(result, ne1));
            target.add_modus_ponens(imp, id++);

            target.add_modus_ponens(e_ptr(new implication(imp, neg)), id++);
            target.add_modus_ponens(neg, id++);
        } else {
            target.add_axiom(produce_axiom(9, result, e2), id++, 9);
            target.add_axiom(produce_axiom(5, e1, e2), id++, 4);

            target.add_axiom(produce_axiom(1, ne2, result), id++, 1);
            target.add_hypothesis(ne2, id++);
            e_ptr imp(new implication(result, ne2));
            target.add_modus_ponens(imp, id++);

            target.add_modus_ponens(e_ptr(new implication(imp, neg)), id++);
            target.add_modus_ponens(neg, id++);
        }

        return target;
    }

    static inline proof prove_implication(e_ptr const &e1, e_ptr const &e2, bool n1, bool n2) {
        proof target;
        head context;
        e_ptr ne1(new negation(e1));
        e_ptr ne2(new negation(e2));
        context.add_hypothesis(n1 ? ne1 : e1);
        context.add_hypothesis(n2 ? ne2 : e2);

        e_ptr result(new implication(e1, e2));
        e_ptr neg(new negation(result));
        if (n1 || !n2) {
            context.set_result(result);
        } else {
            context.set_result(e_ptr(new negation(result)));
        }
        target.set_head(context);
        unsigned int id = 0;

        if (!n2) {
            target.add_hypothesis(e2, id++);
            target.add_axiom(produce_axiom(1, e2, e1), id++, 1);
            target.add_modus_ponens(result, id++);
        } else if (n1) {
            add_deducted_ex_falso(target, e1, e2, id);
        } else {
            e_ptr imp(new implication(e1, e2));
            e_ptr ax1 = produce_axiom(2, imp, e1, e2);
            target.add_axiom(ax1, id++, 2);

            target.add_axiom(produce_axiom(1, e1, imp), id++, 1);
            target.add_hypothesis(e1, id++);
            target.add_modus_ponens(e_ptr(new implication(imp, e1)), id++);
            add_identity(target, imp, id);
            target.add_modus_ponens(i_cast(ax1)->get_right(), id++);
            target.add_modus_ponens(e_ptr(new implication(imp, e2)), id++);

            target.add_axiom(produce_axiom(1, ne2, imp), id++, 1);
            target.add_hypothesis(ne2, id++);
            target.add_modus_ponens(e_ptr(new implication(imp, ne2)), id++);
            e_ptr ax2 = produce_axiom(9, imp, e2);
            target.add_axiom(ax2, id++, 9);
            target.add_modus_ponens(i_cast(ax2)->get_right(), id++);
            target.add_modus_ponens(neg, id++);
        }

        return target;
    }

};

#endif // PROOF_CREATOR_HPP

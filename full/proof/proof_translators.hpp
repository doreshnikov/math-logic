#ifndef PROOF_CONVERTER_HPP
#define PROOF_CONVERTER_HPP

#include <vector>

#include "../grammar/grammar.hpp"
#include "proof_utils.hpp"

namespace translators {

    static inline e_ptr produce_axiom(unsigned int ax, e_ptr const &e1, e_ptr const &e2) {
        if (ax == 1) {
            return e_ptr(new implication(e1, e_ptr(new implication(e2, e1))));
        } else if (ax == 3) {
            return e_ptr(new implication(e1, e_ptr(new implication(e2, e_ptr(new conjunction(e1, e2))))));
        } else if (ax == 4) {
            return e_ptr(new implication(e_ptr(new conjunction(e1, e2)), e1));
        } else if (ax == 5) {
            return e_ptr(new implication(e_ptr(new conjunction(e1, e2)), e2));
        } else if (ax == 6) {
            return e_ptr(new implication(e1, e_ptr(new disjunction(e1, e2))));
        } else if (ax == 7) {
            return e_ptr(new implication(e2, e_ptr(new disjunction(e1, e2))));
        } else if (ax == 9) {
            return e_ptr(new implication(
                e_ptr(new implication(e1, e2)),
                e_ptr(new implication(
                    e_ptr(new implication(e1, e_ptr(new negation(e2)))),
                    e_ptr(new negation(e1)))
                )
            ));
        } else if (ax == 11) {
            return e_ptr(new implication(e1, e_ptr(new implication(e_ptr(new negation(e1)), e2))));
        } else {
            return nullptr;
        }
    }

    static inline e_ptr produce_axiom(unsigned int ax, e_ptr const &e1, e_ptr const &e2, e_ptr const &e3) {
        if (ax == 2) {
            return e_ptr(new implication(
                e_ptr(new implication(e1, e2)),
                e_ptr(new implication(
                    e_ptr(new implication(
                        e1,
                        e_ptr(new implication(e2, e3))
                    )),
                    e_ptr(new implication(e1, e3))
                ))
            ));
        } else if (ax == 8) {
            return e_ptr(new implication(
                e_ptr(new implication(e1, e3)),
                e_ptr(new implication(
                    e_ptr(new implication(e2, e3)),
                    e_ptr(new implication(
                        e_ptr(new disjunction(e1, e2)),
                        e3
                    ))
                ))
            ));
        } else {
            return nullptr;
        }
    }

    static inline void add_identity(proof &target, e_ptr const &expr, unsigned int &id) {
        e_ptr identity(new implication(expr, expr));
        e_ptr ax1 = produce_axiom(1, expr, expr), ax2 = produce_axiom(1, expr, identity);

        target.add_statement(s_ptr(new axiom(ax1, id++, 1)));
        target.add_statement(s_ptr(new axiom(ax2, id++, 1)));
        target.add_statement(s_ptr(new axiom(produce_axiom(2, expr, identity, expr), id++, 2)));

        e_ptr mp1(new implication(ax2, identity));
        target.add_statement(s_ptr(new modus_ponens(mp1, id++,
                                                    target[target.length() - 3], target[target.length() - 1])));
        target.add_statement(s_ptr(new modus_ponens(identity, id++,
                                                    target[target.length() - 3], target[target.length() - 1])));
    }

    static inline proof to_deduction(proof const &source, e_ptr const &hyp, unsigned int &id) {
        proof target;
        head context;
        for (unsigned int i = 0; i < source.get_head().length(); i++) {
            e_ptr expr = source.get_head()[i];
            if (!expr->equals(hyp)) {
                context.add_hypothesis(expr);
            }
        }
        context.set_result(e_ptr(new implication(hyp, source.get_head().get_result())));
        target.set_head(context);

        for (unsigned int i = 0; i < source.length(); i++) {
            s_ptr stat = source[i];
            e_ptr expr = stat->get_expression();
            e_ptr corr = e_ptr(new implication(hyp, expr));

            unsigned int index = target.length();
            if (stat->get_type() == 'a' || stat->get_type() == 'h') {
                if (stat->get_type() == 'h' && expr->equals(hyp)) {
                    add_identity(target, hyp, id);
                } else {
                    if (stat->get_type() == 'h') {
                        target.add_statement(s_ptr(new hypothesis(expr, id++, target.find_hypothesis(expr))));
                    } else {
                        target.add_statement(s_ptr(new axiom(expr, id++, a_cast(stat)->get_number())));
                    }
                    target.add_statement(s_ptr(new axiom(produce_axiom(1, expr, hyp), id++, 1)));
                    target.add_statement(s_ptr(new modus_ponens(corr, id++, target[index], target[index + 1])));
                }
            } else {
                auto origin = source.find_modus_ponens(expr);
                implication *full = i_cast(source[origin.second - 1]->get_expression());

                e_ptr mp1 = produce_axiom(2, hyp, full->get_left(), expr);
                target.add_statement(s_ptr(new axiom(mp1, id++, 2)));

                e_ptr right = i_cast(mp1)->get_right();
                auto mparents = target.find_modus_ponens(right);
                target.add_statement(s_ptr(new modus_ponens(right, id++,
                                                            target[mparents.first - 1], target[mparents.second - 1])));
                auto rparents = target.find_modus_ponens(corr);
                target.add_statement(s_ptr(new modus_ponens(corr, id++,
                                                            target[rparents.first - 1], target[rparents.second - 1])));
            }
        }

        return target;
    }

    static inline proof to_deduction(proof const &source, e_ptr const &hyp) {
        unsigned int id = 0;
        return to_deduction(source, hyp, id);
    }

    static inline void add_contraposition(proof &target, e_ptr const &e1, e_ptr const &e2, unsigned int &id) {
        e_ptr ne1(new negation(e1));
        e_ptr ne2(new negation(e2));

        proof obvious;
        head context;

        context.add_hypothesis(e_ptr(new implication(e1, e2)));
        context.add_hypothesis(ne2);
        context.set_result(ne1);

        obvious.set_head(context);

        obvious.add_statement(s_ptr(new hypothesis(context[0], 0, 1)));
        obvious.add_statement(s_ptr(new hypothesis(context[1], 1, 2)));
        obvious.add_statement(s_ptr(new axiom(produce_axiom(1, ne2, e1), 2, 1)));

        e_ptr imp(new implication(e1, ne2));
        obvious.add_statement(s_ptr(new modus_ponens(imp, 3, obvious[1], obvious[2])));
        obvious.add_statement(s_ptr(new axiom(produce_axiom(9, e1, e2), 4, 9)));

        obvious.add_statement(s_ptr(new modus_ponens(e_ptr(new implication(imp, ne1)), 5, obvious[0], obvious[4])));
        obvious.add_statement(s_ptr(new modus_ponens(ne1, 6, obvious[3], obvious[5])));

        proof tmp1 = to_deduction(obvious, context[1]);
        proof tmp2 = to_deduction(tmp1, context[0], id);
        for (unsigned int i = 0; i < tmp2.length(); i++) {
            target.add_statement(tmp2[i]);
        }
    }

    static proof to_intuitionistic(proof const &source) {
        proof target;

        unsigned int id = 0;
        for (unsigned int i = 0; i < source.length(); i++) {
            s_ptr stat = source[i];
            e_ptr expr = stat->get_expression();

            if (stat->get_type() == 'a' || stat->get_type() == 'h') {
                if (stat->get_type() == 'a' && a_cast(stat)->get_number() == 10) {

                } else {
                    e_ptr neg(new negation(expr));
                    e_ptr impl1(new implication(neg, expr));
                    e_ptr impl2(new implication(neg, neg));

                    unsigned int index = target.length();
                    target.add_statement(s_ptr(new axiom(produce_axiom(8, neg, expr), id++, 8)));
                    target.add_statement(s_ptr(new axiom(produce_axiom(1, expr, neg), id++, 1)));
                    target.add_statement(s_ptr(stat));
                    target.add_statement(s_ptr(new modus_ponens(impl1, id++, target[index + 1], target[index])));
                    target.add_statement(s_ptr(new modus_ponens(e_ptr(new implication(impl2, e_ptr(new negation(neg)))),
                                                                id++,
                                                                target[index + 3], target[index])));

                    add_identity(target, neg, id);
                    target.add_statement(s_ptr(new modus_ponens(e_ptr(new negation(neg)), id++,
                                                                target[index + 4], target[index + 9])));
                }
            } else {

            }
        }
    }

};

#endif // PROOF_CONVERTER_HPP

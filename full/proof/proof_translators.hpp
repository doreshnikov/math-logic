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
        target.add_modus_ponens(mp1, id++);
        target.add_modus_ponens(identity, id++);
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
                    target.add_modus_ponens(corr, id++);
                }
            } else {
                auto origin = source.find_modus_ponens(expr);
                implication *full = i_cast(source[origin.second - 1]->get_expression());

                e_ptr imp = produce_axiom(2, hyp, full->get_left(), expr);
                target.add_statement(s_ptr(new axiom(imp, id++, 2)));

                e_ptr right = i_cast(imp)->get_right();
                target.add_modus_ponens(right, id++);
                target.add_modus_ponens(corr, id++);
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
        obvious.add_modus_ponens(imp, 3);
        obvious.add_statement(s_ptr(new axiom(produce_axiom(9, e1, e2), 4, 9)));

        obvious.add_modus_ponens(e_ptr(new implication(imp, ne1)), 5);
        obvious.add_modus_ponens(ne1, 6);

        unsigned int tid = id;
        proof tmp = to_deduction(obvious, context[1], tid);
        for (unsigned int i = 1; i < tmp.length(); i++) {
            e_ptr expr = tmp[i]->get_expression();
            if (tmp[i]->get_type() == 'a') {
                target.add_statement(s_ptr(new axiom(expr, id++, a_cast(tmp[i])->get_number())));
            } else if (tmp[i]->get_type() == 'h') {
                target.add_statement(s_ptr(new hypothesis(expr, id++, target.find_hypothesis(expr))));
            } else {
                target.add_modus_ponens(expr, id++);
            }
        }
        id = tid;
    }

    static proof to_intuitionistic(proof const &source) {
        proof target;
        target.set_head(source.get_head());
        target.get_head().set_result(e_ptr(new negation(e_ptr(new negation(source.get_head().get_result())))));

        unsigned int id = 0;
        for (unsigned int i = 0; i < source.length(); i++) {
            s_ptr stat = source[i];
            e_ptr expr = stat->get_expression();

            e_ptr neg = e_ptr(new negation(expr));
            e_ptr nneg = e_ptr(new negation(neg));

            if (stat->get_type() == 'a' || stat->get_type() == 'h') {
                if (stat->get_type() == 'a' && a_cast(stat)->get_number() == 10) {
                    e_ptr a = i_cast(expr)->get_right();
                    e_ptr na(new negation(a));
                    e_ptr nna = i_cast(expr)->get_left();

                    target.add_statement(s_ptr(new axiom(produce_axiom(1, a, nna), id++, 1)));
                    add_contraposition(target, a, expr, id);
                    target.add_statement(s_ptr(new axiom(produce_axiom(11, na, a), id++, 1)));
                    add_contraposition(target, na, expr, id);

                    e_ptr ax = produce_axiom(9, neg, na);
                    target.add_statement(s_ptr(new axiom(ax, id++, 9)));

                    e_ptr right = i_cast(ax)->get_right();
                    target.add_modus_ponens(right, id++);
                    target.add_modus_ponens(nneg, id++);
                } else {
                    e_ptr imp1(new implication(neg, expr));
                    e_ptr imp2(new implication(neg, neg));

                    target.add_statement(s_ptr(new axiom(produce_axiom(9, neg, expr), id++, 9)));
                    target.add_statement(s_ptr(new axiom(produce_axiom(1, expr, neg), id++, 1)));

                    if (stat->get_type() == 'h') {
                        target.add_statement(s_ptr(new hypothesis(expr, id++, target.find_hypothesis(expr))));
                    } else {
                        target.add_statement(s_ptr(new axiom(expr, id++, a_cast(stat)->get_number())));
                    }

                    target.add_modus_ponens(imp1, id++);
                    target.add_modus_ponens(e_ptr(new implication(imp2, nneg)), id++);

                    add_identity(target, neg, id);
                    target.add_modus_ponens(nneg, id++);
                }
            } else {
                auto parents = source.find_modus_ponens(expr);
                e_ptr a = source[parents.first - 1]->get_expression();
                e_ptr na(new negation(a));
                e_ptr nna(new negation(na));

                e_ptr imp = source[parents.second - 1]->get_expression();
                e_ptr nimp(new negation(imp));
                e_ptr nnimp(new negation(nimp));

                proof tmp;
                head context;
                context.add_hypothesis(nna);
                context.add_hypothesis(neg);
                context.add_hypothesis(imp);
                context.set_result(nneg);
                tmp.set_head(context);

                unsigned int tid = 0;
                tmp.add_statement(s_ptr(new hypothesis(nna, tid++, 1)));
                tmp.add_statement(s_ptr(new hypothesis(imp, tid++, 3)));
                add_contraposition(tmp, a, expr, tid);
                add_contraposition(tmp, neg, na, tid);
                tmp.add_modus_ponens(nneg, tid++);

                proof d1 = to_deduction(tmp, imp);
                tid = d1.length();
                d1.get_head().set_result(nimp);
                d1.add_statement(s_ptr(new hypothesis(neg, tid++, 2)));

                e_ptr ax0 = produce_axiom(1, neg, imp);
                d1.add_statement(s_ptr(new axiom(ax0, tid++, 1)));

                e_ptr r0 = i_cast(ax0)->get_right();
                d1.add_modus_ponens(r0, tid++);

                e_ptr ax = produce_axiom(9, imp, neg);
                d1.add_statement(s_ptr(new axiom(ax, tid++, 9)));
                e_ptr right = i_cast(ax)->get_right();
                d1.add_modus_ponens(right, tid++);
                d1.add_modus_ponens(nimp, id++);

                proof d2 = to_deduction(d1, neg);
                for (unsigned int i = 0; i < d2.length(); i++) {
                    e_ptr ep = d2[i]->get_expression();
                    if (d2[i]->get_type() == 'a') {
                        target.add_statement(s_ptr(new axiom(ep, id++, a_cast(d2[i])->get_number())));
                    } else if (d2[i]->get_type() == 'h') {
                        target.add_statement(s_ptr(new hypothesis(ep, id++, target.find_hypothesis(ep))));
                    } else {
                        target.add_modus_ponens(ep, id++);
                    }
                }

                e_ptr ax1 = produce_axiom(1, nnimp, neg);
                target.add_statement(s_ptr(new axiom(ax1, id++, 1)));
                target.add_modus_ponens(i_cast(ax1)->get_right(), id++);

                e_ptr ax2 = produce_axiom(9, neg, nimp);
                target.add_statement(s_ptr(new axiom(ax2, id++, 9)));
                target.add_modus_ponens(i_cast(ax2)->get_right(), id++);

                target.add_modus_ponens(nneg, id++);
            }
        }

        return target;
    }

};

#endif // PROOF_CONVERTER_HPP

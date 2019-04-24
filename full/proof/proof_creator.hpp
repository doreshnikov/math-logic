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

    static inline void add_deduced_ex_falso(proof &target, e_ptr const &e1, e_ptr const &e2, unsigned int &id) {
        proof ef = prove_ex_falso(e1, e2);
        proof d = to_deduction(ef, e1);

        for (unsigned int i = 0; i < d.length(); i++) {
            e_ptr expr = d[i]->get_expression();
            if (d[i]->get_type() == 'a') {
                target.add_axiom(expr, id++, a_cast(d[i])->get_number());
            } else if (d[i]->get_type() == 'd') {
                target.add_modus_ponens(expr, id++);
            } else {
                target.add_hypothesis(expr, id++);
            }
        }
    }

    static inline void prove_tertium_non_datur(proof &target, e_ptr const &expr, unsigned int &id) {
        e_ptr neg(new negation(expr));
        e_ptr res(new conjunction(expr, neg));

        e_ptr ax = produce_axiom(9, res, expr);
        target.add_axiom(ax, id++, 9);
        target.add_axiom(produce_axiom(4, expr, neg), id++, 4);
        target.add_axiom(produce_axiom(5, expr, neg), id++, 5);

        target.add_modus_ponens(i_cast(ax)->get_right(), id++);
        target.add_modus_ponens(e_ptr(new negation(res)), id++);
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
            add_deduced_ex_falso(target, e2, e1, id);
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
            add_deduced_ex_falso(target, e1, e2, id);
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

    static inline proof prove_negation(e_ptr const &expr, bool n) {
        proof target;
        head context;
        e_ptr neg(new negation(expr));
        e_ptr nneg(new negation(neg));

        context.add_hypothesis(n ? neg : expr);
        context.set_result(n ? expr : nneg);
        target.set_head(context);

        unsigned int id = 0;
        if (n) {
            target.add_hypothesis(neg, id++);
        } else {
            e_ptr ax = produce_axiom(9, neg, expr);
            target.add_axiom(ax, id++, 9);

            target.add_axiom(produce_axiom(1, expr, neg), id++, 1);
            target.add_hypothesis(expr, id++);
            target.add_modus_ponens(e_ptr(new implication(neg, expr)), id++);
            add_identity(target, neg, id);

            target.add_modus_ponens(i_cast(ax)->get_right(), id++);
            target.add_modus_ponens(nneg, id++);
        }

        return target;
    }

    static inline bool prove_equals_self(proof &target, e_ptr const &expr, unsigned int &id) {
        if (expr->get_type() == 'v') {
            if (target.find_hypothesis(expr) != 0) {
                target.add_hypothesis(expr, id++);
                return true;
            } else {
                target.add_hypothesis(e_ptr(new negation(expr)), id++);
                return false;
            }
        } else if (expr->get_type() == 'n') {
            negation *neg = n_cast(expr);
            bool under = prove_equals_self(target, neg->get_under(), id);

            proof d = prove_negation(neg->get_under(), !under);
            for (unsigned int i = 0; i < d.length(); i++) {
                e_ptr e = d[i]->get_expression();
                if (d[i]->get_type() == 'a') {
                    target.add_axiom(e, id++, a_cast(d[i])->get_number());
                } else if (d[i]->get_type() == 'd') {
                    target.add_modus_ponens(e, id++);
                } else {
                    target.add_hypothesis(e, id++);
                }
            }
            return !under;
        } else if (expr->get_type() == 'd') {
            disjunction *disj = d_cast(expr);
            bool left = prove_equals_self(target, disj->get_left(), id);
            bool right = prove_equals_self(target, disj->get_right(), id);

            proof d = prove_disjunction(disj->get_left(), disj->get_right(), !left, !right);
            for (unsigned int i = 0; i < d.length(); i++) {
                e_ptr e = d[i]->get_expression();
                if (d[i]->get_type() == 'a') {
                    target.add_axiom(e, id++, a_cast(d[i])->get_number());
                } else if (d[i]->get_type() == 'd') {
                    target.add_modus_ponens(e, id++);
                } else {
                    target.add_hypothesis(e, id++);
                }
            }
            return left || right;
        } else if (expr->get_type() == 'c') {
            conjunction *conj = c_cast(expr);
            bool left = prove_equals_self(target, conj->get_left(), id);
            bool right = prove_equals_self(target, conj->get_right(), id);

            proof d = prove_conjunction(conj->get_left(), conj->get_right(), !left, !right);
            for (unsigned int i = 0; i < d.length(); i++) {
                e_ptr e = d[i]->get_expression();
                if (d[i]->get_type() == 'a') {
                    target.add_axiom(e, id++, a_cast(d[i])->get_number());
                } else if (d[i]->get_type() == 'd') {
                    target.add_modus_ponens(e, id++);
                } else {
                    target.add_hypothesis(e, id++);
                }
            }
            return left && right;
        } else if (expr->get_type() == 'i') {
            implication *impl = i_cast(expr);
            bool left = prove_equals_self(target, impl->get_left(), id);
            bool right = prove_equals_self(target, impl->get_right(), id);

            proof d = prove_implication(impl->get_left(), impl->get_right(), !left, !right);
            for (unsigned int i = 0; i < d.length(); i++) {
                e_ptr e = d[i]->get_expression();
                if (d[i]->get_type() == 'a') {
                    target.add_axiom(e, id++, a_cast(d[i])->get_number());
                } else if (d[i]->get_type() == 'd') {
                    target.add_modus_ponens(e, id++);
                } else {
                    target.add_hypothesis(e, id++);
                }
            }
            return !left || right;
        } else {
            // wtf
            return false;
        }
    }

    static inline proof merge_proofs(proof const &p1, proof const &p2, e_ptr const &var) {
        e_ptr result = p1.get_head().get_result();

        proof d1 = to_deduction(p1, var);
        unsigned int id = d1.length();
        add_contraposition(d1, var, result, id);

        e_ptr nvar(new negation(var));
        proof d2 = to_deduction(p2, nvar);
        for (unsigned int i = 0; i < d2.length(); i++) {
            e_ptr expr = d2[i]->get_expression();
            if (d2[i]->get_type() == 'a') {
                d1.add_axiom(expr, id++, a_cast(d2[i])->get_number());
            } else if (d2[i]->get_type() == 'd') {
                d1.add_modus_ponens(expr, id++);
            } else {
                d1.add_hypothesis(expr, id++);
            }
        }
        add_contraposition(d1, nvar, result, id);

        e_ptr ax = produce_axiom(9, e_ptr(new negation(result)), nvar);
        d1.add_axiom(ax, id++, 9);
        e_ptr r = i_cast(ax)->get_right();
        d1.add_modus_ponens(r, id++);
        d1.add_modus_ponens(i_cast(r)->get_right(), id++);

        d1.add_axiom(produce_axiom(10, result), id++, 10);
        d1.add_modus_ponens(result, id++);
        d1.get_head().set_result(p1.get_head().get_result());
        return d1;
    }

    static inline proof try_prove(e_ptr const &expr) {
        std::unordered_set<std::string> names_set;
        expr->collect_variable_names(names_set);
        std::vector<std::string> names;
        std::copy(names_set.begin(), names_set.end(), std::back_inserter(names));

        std::vector<std::vector<proof>> proofs(1u << names.size(), std::vector<proof>(1u << names.size()));
        unsigned int all = (1u << names.size()) - 1;
        for (unsigned int mask = 0; mask < (1u << names.size()); mask++) {
            proof target;
            head context;
            for (unsigned int i = 0; i < names.size(); i++) {
                e_ptr v(new variable(names[i]));
                context.add_hypothesis((mask & (1u << i)) == 0 ? e_ptr(new negation(v)) : v);
            }
            bool value = expr->compute(context.to_map());
            context.set_result(value ? expr : e_ptr(new negation(expr)));
            target.set_head(context);

            unsigned int id = 0;
            prove_equals_self(target, expr, id);
            proofs[all][mask] = target;
        }

        unsigned int pos_mask = all + 1, pos_bits = names.size() + 1;
        unsigned int neg_mask = all + 1, neg_bits = names.size() + 1;
        for (int pmask = all; pmask >= 0; pmask--) {
            std::vector<unsigned int> bits;
            for (unsigned int i = 0; i < names.size(); i++) {
                if ((pmask & (1u << i)) != 0) {
                    bits.push_back(i);
                }
            }
            if (bits.size() < pos_bits) {
                if (proofs[pmask][pmask].length() != 0 &&
                    proofs[pmask][pmask].get_root()->get_expression()->equals(expr)) {
                    pos_bits = bits.size();
                    pos_mask = pmask;
                }
            }
            if (bits.size() < neg_bits) {
                if (proofs[pmask][0].length() != 0 &&
                    !proofs[pmask][0].get_root()->get_expression()->equals(expr)) {
                    neg_bits = bits.size();
                    neg_mask = pmask;
                }
            }
            for (unsigned int mask = 0; mask <= pmask; mask++) {
                if ((pmask & mask) != mask) {
                    continue;
                }
                for (unsigned int i : bits) {
                    unsigned int id = 1u << i;
                    if (proofs[pmask][mask].length() != 0 && proofs[pmask][mask ^ id].length() != 0
                        && proofs[pmask][mask].get_root()->get_expression()->equals(
                            proofs[pmask][mask ^ id].get_root()->get_expression())) {
                        e_ptr var(new variable(names[i]));
                        if (proofs[pmask][mask].find_hypothesis(var) != 0) {
                            proofs[pmask ^ id][mask & (all ^ id)] =
                                merge_proofs(proofs[pmask][mask],
                                             proofs[pmask][mask ^ id],
                                             e_ptr(new variable(names[i])));
                        } else {
                            proofs[pmask ^ id][mask & (all ^ id)] =
                                merge_proofs(proofs[pmask][mask ^ id],
                                             proofs[pmask][mask],
                                             e_ptr(new variable(names[i])));
                        }
                    }
                }
            }
        }

        if (pos_mask != all + 1) {
            return proofs[pos_mask][pos_mask];
        } else if (neg_mask != all + 1) {
            return proofs[neg_mask][0];
        } else {
            return proof();
        }
    }

};

#endif // PROOF_CREATOR_HPP

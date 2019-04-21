#include "proof_utils.hpp"
#include "../parser/parser.hpp"

proof_scanner::proof_scanner() {}

s_ptr proof_scanner::parse_proof() {
    parse_head();
    unsigned int line_number = 0;
    while (std::cin) {
        get_line();
        line_number++;
        e_ptr expr = parser(_line).parse_expression();
        if (expr.get() == nullptr) {
            break;
        }
        unsigned int number;
        if ((number = _proof.find_hypothesis(expr))) {
            _proof.add_statement(expr, s_ptr(new hypothesis(expr, line_number, number)));
        } else if ((number = is_axiom(expr))) {
            _proof.add_statement(expr, s_ptr(new axiom(expr, line_number, number)));
        } else {
            std::pair<unsigned int, unsigned int> mp = _proof.find_modus_ponens(expr);
            if (mp.first == 0) {
                throw std::logic_error("proof is incorrect");
            } else {
                _proof.add_statement(expr, s_ptr(new modus_ponens(expr, line_number,
                                                         _proof[mp.first - 1], _proof[mp.second - 1])));
            }
        }
    }
    if (!_proof.get_root()->get_expression()->equals(_proof.get_head().get_result())) {
        throw std::logic_error("proof is incorrect");
    }
    return _proof.get_root();
}

unsigned int proof_scanner::is_axiom(e_ptr const &expr) {
    if (expr->get_type() != 'i') {
        return 0;
    } else {
        implication *imp = i_cast(expr);
        auto left = imp->get_left();
        e_ptr right = imp->get_right();
        if (left->get_type() == 'c') {
            if (c_cast(left)->get_left()->equals(right)) {
                return 4;
            } else if (c_cast(left)->get_right()->equals(right)) {
                return 5;
            }
        } else if (left->get_type() == 'n') {
            e_ptr left_under = n_cast(left)->get_under();
            if (left_under->get_type() == 'n' && n_cast(left_under)->get_under()->equals(right)) {
                return 10;
            }
        }
        if (right->get_type() == 'd') {
            if (d_cast(right)->get_left()->equals(left)) {
                return 6;
            } else if (d_cast(right)->get_right()->equals(left)) {
                return 7;
            }
        }
        if (right->get_type() != 'i') {
            return 0;
        }
        implication *iright = i_cast(right);
        if (iright->get_right()->equals(left)) {
            return 1;
        } else {
            if (iright->get_right()->equals(e_ptr(new conjunction(left, iright->get_left())))) {
                return 3;
            }
            if (left->get_type() != 'i' || iright->get_left()->get_type() != 'i') {
                return 0;
            }
            implication *ileft = i_cast(left);
            implication *imid = i_cast(iright->get_left());
            right = iright->get_right();

            if (ileft->get_left()->equals(imid->get_left()) &&
                imid->get_right()->equals(e_ptr(new negation(ileft->get_right()))) &&
                right->equals(e_ptr(new negation(ileft->get_left())))) {
                return 9;
            } else {
                if (right->get_type() != 'i') {
                    return 0;
                }
                iright = i_cast(right);

                if (ileft->get_left()->equals(imid->get_left()) && ileft->get_left()->equals(iright->get_left()) &&
                    imid->get_right()->equals(e_ptr(new implication(ileft->get_right(), iright->get_right())))) {
                    return 2;
                }
                if (ileft->get_right()->equals(imid->get_right()) && ileft->get_right()->equals(iright->get_right())
                    &&
                        iright->get_left()->equals(e_ptr(new disjunction(ileft->get_left(), imid->get_left())))) {
                    return 8;
                }
            }
        }
    }
    return 0;
}

void proof_scanner::parse_head() {
    get_line();
    _proof.set_head(parser(_line).parse_context());
}

void proof_scanner::get_line() {
    char c;
    _line = "";
    while (std::cin.get(c)) {
        if (c == '\n') {
            break;
        }
        _line.push_back(c);
    }
}

head const &proof_scanner::get_head() const {
    return _proof.get_head();
}

proof const &proof_scanner::get_proof() const {
    return _proof;
}

proof_printer::proof_printer(head const &context, s_ptr const &root) : _context(context), _root(root) {}

void proof_printer::print(print_policy const &policy) const {
    _context.print_all();
    statement_collector collector;
    _root->walk(collector);
    collector.print_all(policy);
}
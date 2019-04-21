#include "grammar.hpp"
#include <algorithm>

void statement_collector::add_statement(statement const *stat) {
    if (_ids.count(stat->get_id()) == 0) {
        _actual_proof.push_back(stat);
        _ids.insert(stat->get_id());
    }
}

void statement_collector::print_all(print_policy const &policy) {
    std::sort(_actual_proof.begin(), _actual_proof.end(), [](statement const *l, statement const *r) {
        return l->get_id() < r->get_id();
    });
    for (size_t i = 0; i < _actual_proof.size(); i++) {
        _actual_proof[i]->set_id(i + 1);
        if (policy == print_policy::MARKED) {
            _actual_proof[i]->print();
        } else {
            _actual_proof[i]->get_expression()->print("\n");
        }
    }
}

sign::sign(sign_t value) : _value(value) {}

bool str_equal(std::string const &s1, std::string const &line, unsigned int position) {
    int i = 0;
    while (i < s1.length() && position < line.length() && s1[i] == line[position]) {
        position++;
        i++;
    }
    return i == s1.length();
}

bool sign::is_space(std::string const &line, unsigned int position) {
    return line[position] == ' ' || line[position] == '\r';
}

sign_t sign::fits_in_expression(std::string const &line, unsigned int position) {
    if (is_space(line, position)) {
        return sign_t::Space;
    }
    for (sign_t value : _expression_signs) {
        std::string repr = to_string(value);
        if (str_equal(repr, line, position)) {
            return value;
        }
    }
    return sign_t::None;
}

sign_t sign::fits_in_context(std::string const &line, unsigned int position) {
    if (is_space(line, position)) {
        return sign_t::Space;
    }
    for (sign_t value : _context_signs) {
        std::string repr = to_string(value);
        if (str_equal(repr, line, position)) {
            return value;
        }
    }
    return sign_t::None;
}

std::string sign::to_string(sign_t value) {
    switch (value) {
        case sign_t::Negation:return "!";
        case sign_t::Conjunction:return "&";
        case sign_t::Disjunction:return "|";
        case sign_t::Implication:return "->";
        case sign_t::LBracket:return "(";
        case sign_t::RBracket:return ")";
        case sign_t::Turnstile:return "|-";
        case sign_t::Separator:return ",";
        case sign_t::Space:return " ";
        default: return "";
    }
}

std::string sign::to_string() const {
    return to_string(_value);
}

std::string variable_name::fits(std::string const &line, unsigned int position) {
    std::string result;
    if (fits_first(line, position)) {
        while (fits_any(line, position)) {
            result.push_back(line[position++]);
        }
    }
    return result;
}

bool variable_name::fits_first(std::string const &line, unsigned int position) {
    char c = line[position];
    return c >= 'A' && c <= 'Z';
}

bool variable_name::fits_any(std::string const &line, unsigned int position) {
    char c = line[position];
    return c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '\'';
}

expression::expression() : _hash("") {}

expression::expression(std::string const &s) : _hash(s) {}

expression::expression(std::string const &s, expression_hash const &h1) : _hash(s, h1) {}

expression::expression(std::string const &s, expression_hash const &h1, expression_hash const &h2) : _hash(s, h1, h2) {}

bool expression::equals(e_ptr const &other) const {
    return get_type() == other->get_type() && _hash == other->_hash;
}

bool expression::operator==(expression const &other) const {
    return _hash == other._hash;
}

void expression::print(std::string const &endl = "\n") const {
    std::cout << to_infix() << endl;
}

bi_expression::bi_expression(sign_t const &s, e_ptr const &left, e_ptr const &right) :
    expression(sign::to_string(s), left->_hash, right->_hash), _s(s), _left(left), _right(right) {}

std::string bi_expression::to_prefix() const {
    return '(' + _s.to_string() + ',' + _left->to_prefix() + ',' + _right->to_prefix() + ')';
}

std::string bi_expression::to_infix() const {
    return '(' + _left->to_infix() + " " + _s.to_string() + " " + _right->to_infix() + ')';
}

implication::implication(e_ptr const &left, e_ptr const &right) :
    bi_expression(sign_t::Implication, left, right) {}

char implication::get_type() const {
    return 'i';
}

e_ptr const &implication::get_left() const {
    return _left;
}

e_ptr const &implication::get_right() const {
    return _right;
}

conjunction::conjunction(e_ptr const &left, e_ptr const &right) :
    bi_expression(sign_t::Conjunction, left, right) {}

char conjunction::get_type() const {
    return 'c';
}

e_ptr const &conjunction::get_left() const {
    return _left;
}

e_ptr const &conjunction::get_right() const {
    return _right;
}

disjunction::disjunction(e_ptr const &left, e_ptr const &right) :
    bi_expression(sign_t::Disjunction, left, right) {}

char disjunction::get_type() const {
    return 'd';
}

e_ptr const &disjunction::get_left() const {
    return _left;
}

e_ptr const &disjunction::get_right() const {
    return _right;
}

negation::negation(e_ptr const &under) : expression(sign::to_string(sign_t::Negation), under->_hash), _under(under) {}

char negation::get_type() const {
    return 'n';
}

e_ptr const &negation::get_under() const {
    return _under;
}

std::string negation::to_prefix() const {
    return "(!" + _under->to_prefix() + ")";
}

std::string negation::to_infix() const {
    return "!" + _under->to_infix();
}

variable::variable(std::string const &name) : expression(name), _name(name) {}

std::string variable::to_prefix() const {
    return _name;
}

std::string variable::to_infix() const {
    return _name;
}

char variable::get_type() const {
    return 'v';
}

head::head() : _result(nullptr) {}

void head::add_hypothesis(e_ptr const &expr) {
    _context.emplace_back(expr);
}

void head::set_result(e_ptr const &expr) {
    _result = expr;
}

void head::print_all() const {
    for (unsigned int i = 0; i < _context.size(); i++) {
        _context[i]->print(i == _context.size() - 1 ? "" : ", ");
    }
    std::cout << " |- ";
    _result->print();
}

unsigned int head::size() const {
    return _context.size();
}

e_ptr head::operator[](unsigned int i) const {
    return _context[i];
}

e_ptr const &head::get_result() const {
    return _result;
}

unsigned int head::length() const {
    return _context.size();
}

statement::statement(e_ptr const &expr, unsigned int id)
    : _id(id), _expression(expr) {}

void statement::print() const {
    _expression->print();
}

unsigned int statement::get_id() const {
    return _id;
}

e_ptr const &statement::get_expression() const {
    return _expression;
}

void statement::set_id(unsigned int id) const {
    _id = id;
}

hypothesis::hypothesis(e_ptr const &expr, unsigned int id, unsigned int number) :
    statement(expr, id), _number(number) {}

void hypothesis::print() const {
    std::cout << "[" << _id << ". Hypothesis " << _number << "] ";
    statement::print();
}

void hypothesis::walk(statement_collector &collector) {
    collector.add_statement(this);
}

char hypothesis::get_type() const {
    return 'h';
}

unsigned int hypothesis::get_number() const {
    return _number;
}

axiom::axiom(e_ptr const &expr, unsigned int id, unsigned int number) :
    statement(expr, id), _number(number) {}

void axiom::print() const {
    std::cout << "[" << _id << ". Ax. sch. " << _number << "] ";
    statement::print();
}

void axiom::walk(statement_collector &collector) {
    collector.add_statement(this);
}

char axiom::get_type() const {
    return 'a';
}

unsigned int axiom::get_number() const {
    return _number;
}

modus_ponens::modus_ponens(e_ptr const &expr, unsigned int id,
    std::shared_ptr<statement> const &left, std::shared_ptr<statement> const &right) :
    statement(expr, id), _left(left), _right(right) {}

void modus_ponens::print() const {
    std::cout << "[" << _id << ". M.P. " << _right->get_id() << ", " << _left->get_id() << "] ";
    statement::print();
}

void modus_ponens::walk(statement_collector &collector) {
    _left->walk(collector);
    _right->walk(collector);
    collector.add_statement(this);
}

char modus_ponens::get_type() const {
    return 'd';
}

s_ptr modus_ponens::get_left() const {
    return _left;
}

s_ptr modus_ponens::get_right() const {
    return _right;
}

proof::proof() : _context() {}

void proof::set_head(head const &context) {
    _context = context;
}

void proof::add_statement(e_ptr const &expr, s_ptr const &stat) {
    _statements.emplace_back(stat);
    auto const &outer_key = hashable_expression(stat->get_expression());
    if (_exists.count(outer_key) == 0) {
        _exists.insert({outer_key, _statements.size() - 1});
    }
    if (expr->get_type() == 'i') {
        implication *imp = i_cast(expr);
        auto const &key = hashable_expression(imp->get_right());
        _mp_imp[key].push_back(_statements.size() - 1);
    }
}

void proof::add_statement(s_ptr const &stat) {
    add_statement(stat->get_expression(), stat);
}

s_ptr proof::get_root() const {
    return _statements.back();
}

s_ptr proof::operator[](unsigned int i) const {
    return _statements.at(i);
}

unsigned int proof::length() const {
    return _statements.size();
}

unsigned int proof::find_hypothesis(e_ptr const &expr) const {
    for (size_t i = 0; i < _context.size(); i++) {
        if (_context[i]->equals(expr)) {
            return i + 1;
        }
    }
    return 0;
}

unsigned int proof::find_id(e_ptr const &expr) const {
    auto const &key = hashable_expression(expr);
    if (_exists.count(key)) {
        return _exists.at(key) + 1;
    }
    return 0;
}

std::pair<unsigned int, unsigned int> proof::find_modus_ponens(e_ptr const &expr) const {
#ifndef SLOW
    auto const &outer_key = hashable_expression(expr);
    if (_mp_imp.count(outer_key) == 0) {
        return {0, 0};
    }
    for (unsigned int idx : _mp_imp.at(outer_key)) {
        implication *imp = i_cast(_statements[idx]->get_expression());
        auto const &key = hashable_expression(imp->get_left());
        if (_exists.count(key)) {
            return {_exists.at(key) + 1, idx + 1};
        }
    }
#else
    for (size_t i = 0; i < _statements.size(); i++) {
        for (size_t j = 0; j < _statements.size(); j++) {
            e_ptr l = _statements[i]->get_expression();
            e_ptr r = _statements[j]->get_expression();
            if (l->get_type() == 'i') {
                implication *il = i_cast(l);
                if (il->get_left()->equals(r) && il->get_right()->equals(expr)) {
                    return {j + 1, i + 1};
                }
            }
        }
    }
#endif
    return {0, 0};
}

head const &proof::get_head() const {
    return _context;
}

head &proof::get_head() {
    return _context;
}

unsigned int proof::add_modus_ponens(e_ptr const &expr, unsigned int id) {
    auto parents = find_modus_ponens(expr);
    if (parents.first != 0) {
        add_statement(s_ptr(new modus_ponens(expr, id, (*this)[parents.first - 1], (*this)[parents.second - 1])));
        return id;
    } else {
        return 0;
    }
}

hashable_expression::hashable_expression(e_ptr const &expr) : _expr(expr) {}

hashable_expression::hashable_expression(hashable_expression const &other) : _expr(other._expr) {}

bool hashable_expression::operator==(hashable_expression const &other) const {
    return _expr->equals(other._expr);
}
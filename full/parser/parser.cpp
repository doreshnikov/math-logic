#include <cassert>
#include "parser.hpp"

parser::parser(std::string const &target) : _target(target), _position(0) {}

e_ptr parser::parse_expression() {
    if (_position < _target.length()) {
        next_token();
        return parse_implication();
    } else {
        return e_ptr();
    }
}

head parser::parse_context() {
    head c(_target);
    next_token();
    if (_sign_token != sign_t::Turnstile) {
        revert();
        while (_sign_token == sign_t::Separator || _position == 0) {
            c.add_hypothesis(parse_expression());
        }
    }
    assert(_sign_token == sign_t::Turnstile);
    c.set_result(parse_expression());
    return c;
}

void parser::next_token() {
    while ((_sign_token = sign::fits_in_expression(_target, _position)) == sign_t::Space) {
        _position += sign::to_string(_sign_token).length();
    }
    if ((_sign_token = sign::fits_in_context(_target, _position)) != sign_t::None) {
        _token = sign::to_string(_sign_token);
    } else if ((_sign_token = sign::fits_in_expression(_target, _position)) != sign_t::None) {
        _token = sign::to_string(_sign_token);
    } else {
        _token = variable_name::fits(_target, _position);
    }
    _position += _token.size();
}
void parser::revert() {
    _position = 0;
    _sign_token = sign_t::None;
    _token = "";
}

e_ptr parser::parse_implication() {
    e_ptr node = parse_disjunction();
    if (_sign_token == sign_t::Implication) {
        next_token();
        node = e_ptr(new implication(node, parse_implication()));
    }
    return node;
}

e_ptr parser::parse_disjunction() {
    e_ptr node = parse_conjunction();
    while (_sign_token == sign_t::Disjunction) {
        next_token();
        node = e_ptr(new disjunction(node, parse_conjunction()));
    }
    return node;
}

e_ptr parser::parse_conjunction() {
    e_ptr node = parse_negation();
    while (_sign_token == sign_t::Conjunction) {
        next_token();
        node = e_ptr(new conjunction(node, parse_negation()));
    }
    return node;
}

e_ptr parser::parse_negation() {
    e_ptr node;
    if (_sign_token == sign_t::Negation) {
        next_token();
        node = e_ptr(new negation(parse_negation()));
    } else {
        if (_sign_token == sign_t::LBracket) {
            next_token();
            node = parse_implication();
        } else {
            node = e_ptr(new variable(_token));
        }
        next_token();
    }
    return node;
}

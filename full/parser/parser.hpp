#ifndef PARSER_HPP
#define PARSER_HPP

#include "../grammar/grammar.hpp"

class parser {
public:
    parser(std::string const &);

    e_ptr parse_expression();
    head parse_context();

private:
    std::string _target;
    unsigned int _position;
    std::string _token;
    sign_t _sign_token;

    void next_token();
    void revert();

    e_ptr parse_implication();
    e_ptr parse_disjunction();
    e_ptr parse_conjunction();
    e_ptr parse_negation();
};

#endif // PARSER_HPP

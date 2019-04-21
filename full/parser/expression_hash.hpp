#ifndef EXPRESSION_HASH_HPP
#define EXPRESSION_HASH_HPP

#include <string>

class expression_hash {
public:
    expression_hash(std::string const &);
    expression_hash(std::string const &, expression_hash const &);
    expression_hash(std::string const &, expression_hash const &, expression_hash const &);

    bool operator==(expression_hash const &) const;

    size_t hash() const;

private:
    static const unsigned long long P = 239;
    static const unsigned long long BASE = 1000000009;

    unsigned long long _value;
    unsigned int _length;

    void forward_hash(std::string const &);

    static unsigned int get_power(unsigned int);
};

#endif // EXPRESSION_HASH_HPP

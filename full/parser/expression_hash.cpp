#include "expression_hash.hpp"

expression_hash::expression_hash(std::string const &s) : _value(0), _length(s.length()) {
    forward_hash(s);
}

expression_hash::expression_hash(std::string const &s, expression_hash const &h1) :
    _value(0), _length(s.length() + h1._length) {
    forward_hash(s);
    _value = static_cast<unsigned int>(
        (static_cast<unsigned long long>(_value) + get_power(s.length()) * h1._value) % BASE);
}

expression_hash::expression_hash(std::string const &s, expression_hash const &h1, expression_hash const &h2) :
    _value(0), _length(s.length() + h1._length + h2._length) {
    forward_hash(s);
    _value = static_cast<unsigned int>(
        (static_cast<unsigned long long>(_value) +
            get_power(s.length()) * h1._value + get_power(s.length() + h1._length) * h2._value) % BASE);
}

bool expression_hash::operator==(expression_hash const &other) const {
    return _length == other._length && _value == other._value;
}

void expression_hash::forward_hash(std::string const &s) {
    for (char c : s) {
        _value = static_cast<unsigned int>((static_cast<unsigned long long>(_value) * P + c) % BASE);
    }
}

unsigned int expression_hash::get_power(unsigned int i) {
    if (i == 0) {
        return 1;
    } else {
        unsigned long long p = get_power(i / 2);
        if (i % 2 == 0) {
            return static_cast<unsigned int>(p * p % BASE);
        } else {
            return static_cast<unsigned int>(p * p % BASE * P % BASE);
        }
    }
}

size_t expression_hash::hash() const {
    return static_cast<size_t>(_value) ^ _length;
}

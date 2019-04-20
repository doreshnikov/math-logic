#include "expression_hash.hpp"

expression_hash::expression_hash(std::string const &s) : _forward(0), _reverse(0), _nosigns(0), _length(s.length()) {
    forward_hash(s);
    reverse_hash(s);
    _nosigns = static_cast<unsigned int>((static_cast<unsigned long long>(_forward) ^ 0xFEDC) % BASE);
}

expression_hash::expression_hash(std::string const &s, expression_hash const &h1) :
    _forward(0), _reverse(0), _nosigns(0), _length(s.length() + h1._length) {
    forward_hash(s);
    _forward =
        static_cast<unsigned int>(
            (static_cast<unsigned long long>(_forward) + get_power(s.length()) * h1._forward) % BASE);
    _reverse = h1._reverse;
    reverse_hash(s);
    _nosigns = static_cast<unsigned int>((static_cast<unsigned long long>(_forward) ^ _reverse) % BASE);
}

expression_hash::expression_hash(std::string const &s, expression_hash const &h1, expression_hash const &h2) :
    _forward(0), _reverse(0), _nosigns(0), _length(s.length() + h1._length + h2._length) {
    forward_hash(s);
    _forward =
        static_cast<unsigned int>(
            (static_cast<unsigned long long>(_forward) +
            get_power(s.length()) * h1._forward + get_power(s.length() + h1._length) * h2._forward) % BASE);
    _reverse =
        static_cast<unsigned int>(
            (static_cast<unsigned long long>(h2._reverse) + get_power(h2._length) * h1._reverse) % BASE);
    reverse_hash(s);
    _nosigns = static_cast<unsigned int>(
        ((static_cast<unsigned long long>(_forward) & static_cast<unsigned long long>(_reverse)) ^ 0xAAAA) % BASE);
}

bool expression_hash::operator==(expression_hash const &other) const {
    return _length == other._length &&  _forward == other._forward && _reverse == other._reverse && _nosigns == other._nosigns;
}

void expression_hash::forward_hash(std::string const &s) {
    for (char c : s) {
        _forward = static_cast<unsigned int>((static_cast<unsigned long long>(_forward) * P + c) % BASE);
    }
}

void expression_hash::reverse_hash(std::string const &s) {
    for (auto c = s.rbegin(); c != s.rend(); c++) {
        _reverse = static_cast<unsigned int>((static_cast<unsigned long long>(_reverse) * P + *c) % BASE);
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
    return static_cast<size_t>(_forward * 29 + (_reverse << 4) * 13 + ((_nosigns & _length) >> 3));
}

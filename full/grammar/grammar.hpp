#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <string>
#include <memory>
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "../parser/expression_hash.hpp"

enum class sign_t {
    Negation,
    Conjunction,
    Disjunction,
    Implication,
    LBracket,
    RBracket,

    Turnstile,
    Separator,

    Space,
    None,
};

constexpr sign_t _expression_signs[] = {
    sign_t::Negation,
    sign_t::Conjunction,
    sign_t::Disjunction,
    sign_t::Implication,
    sign_t::LBracket,
    sign_t::RBracket
};

constexpr sign_t _context_signs[] = {
    sign_t::Turnstile,
    sign_t::Separator
};

class sign {
public:
    sign(sign_t);

    static bool is_space(std::string const &, unsigned int);
    static sign_t fits_in_expression(std::string const &, unsigned int);
    static sign_t fits_in_context(std::string const &, unsigned int);
    static std::string to_string(sign_t);
    std::string to_string() const;

private:
    sign_t _value;
};

class variable_name {
public:
    static std::string fits(std::string const &, unsigned int);

private:
    static bool fits_first(std::string const &, unsigned int);
    static bool fits_any(std::string const &, unsigned int);
};

class expression;

typedef std::shared_ptr<expression> e_ptr;

class expression {
public:
    expression();
    virtual ~expression() {};
    bool operator==(expression const &) const;

    virtual bool equals(e_ptr const &) const;

    virtual std::string to_prefix() const = 0;
    virtual std::string to_infix() const = 0;
    virtual char get_type() const = 0;
    virtual bool compute(std::unordered_map<std::string, bool> const &) const = 0;

    virtual void collect_variable_names(std::unordered_set<std::string> &) const = 0;

    void print(std::string const &endl) const;

    expression_hash _hash;

protected:
    expression(std::string const &);
    expression(std::string const &, expression_hash const &);
    expression(std::string const &, expression_hash const &, expression_hash const &);
};

class bi_expression : public expression {
public:
    std::string to_prefix() const override;
    std::string to_infix() const override;

    void collect_variable_names(std::unordered_set<std::string> &) const override;

protected:
    bi_expression(sign_t const &s, e_ptr const &left, e_ptr const &right);

    sign _s;
    e_ptr _left;
    e_ptr _right;
};

class implication : public bi_expression {
public:
    implication(e_ptr const &left, e_ptr const &right);

    char get_type() const override;
    e_ptr const &get_left() const;
    e_ptr const &get_right() const;

    bool compute(std::unordered_map<std::string, bool> const &) const override;
};

class conjunction : public bi_expression {
public:
    conjunction(e_ptr const &left, e_ptr const &right);

    char get_type() const override;
    e_ptr const &get_left() const;
    e_ptr const &get_right() const;

    bool compute(std::unordered_map<std::string, bool> const &) const override;
};

class disjunction : public bi_expression {
public:
    disjunction(e_ptr const &left, e_ptr const &right);

    char get_type() const override;
    e_ptr const &get_left() const;
    e_ptr const &get_right() const;

    bool compute(std::unordered_map<std::string, bool> const &) const override;
};

class negation : public expression {
public:
    negation(e_ptr const &under);

    std::string to_prefix() const override;
    std::string to_infix() const override;

    char get_type() const override;
    e_ptr const &get_under() const;

    bool compute(std::unordered_map<std::string, bool> const &) const override;
    void collect_variable_names(std::unordered_set<std::string> &) const override;

private:
    e_ptr _under;
};

class variable : public expression {
public:
    variable(std::string const &);

    std::string to_prefix() const override;
    std::string to_infix() const override;

    char get_type() const override;

    bool compute(std::unordered_map<std::string, bool> const &) const override;
    void collect_variable_names(std::unordered_set<std::string> &) const override;

private:
    std::string _name;
};

inline implication *i_cast(e_ptr const &expr) {
    return reinterpret_cast<implication *>(expr.get());
}

inline disjunction *d_cast(e_ptr const &expr) {
    return reinterpret_cast<disjunction *>(expr.get());
}

inline conjunction *c_cast(e_ptr const &expr) {
    return reinterpret_cast<conjunction *>(expr.get());
}

inline negation *n_cast(e_ptr const &expr) {
    return reinterpret_cast<negation *>(expr.get());
}

inline variable *v_cast(e_ptr const &expr) {
    return reinterpret_cast<variable *>(expr.get());
}

class head {
public:
    head();
    head(head const &) = default;

    void add_hypothesis(e_ptr const &);
    void set_result(e_ptr const &);

    void print_all() const;
    unsigned int length() const;

    std::unordered_map<std::string, bool> to_map() const;

    unsigned int size() const;
    e_ptr operator[](unsigned int) const;
    e_ptr const &get_result() const;

private:
    std::vector<e_ptr> _context;
    e_ptr _result;
};

class statement;

typedef std::shared_ptr<statement> s_ptr;

class statement_collector {
public:
    enum class print_policy {
        MARKED,
        UNMARKED
    };

    statement_collector() = default;
    void add_statement(statement const *);

    void print_all(print_policy const &);

private:
    std::vector<statement const *> _actual_proof;
    std::unordered_set<unsigned int> _ids;
};

class statement {
public:
    statement(e_ptr const &, unsigned int);
    statement(statement const &) = default;
    virtual ~statement() {};

    virtual void walk(statement_collector &) = 0;
    virtual char get_type() const = 0;
    virtual void print() const;
    void set_id(unsigned int) const;

    unsigned int get_id() const;
    e_ptr const &get_expression() const;

protected:
    mutable unsigned int _id;
    e_ptr _expression;
};

class hypothesis : public statement {
public:
    hypothesis(e_ptr const &, unsigned int, unsigned int);
    ~hypothesis() = default;

    void print() const override;
    void walk(statement_collector &) override;
    char get_type() const override;

    unsigned int get_number() const;

private:
    unsigned int _number;
};

class axiom : public statement {
public:
    axiom(e_ptr const &, unsigned int, unsigned int);
    ~axiom() = default;

    void print() const override;
    void walk(statement_collector &) override;
    char get_type() const override;

    unsigned int get_number() const;

private:
    unsigned int _number;
};

class modus_ponens : public statement {
public:
    modus_ponens(e_ptr const &, unsigned int, s_ptr const &, s_ptr const &);
    ~modus_ponens() = default;

    void print() const override;
    void walk(statement_collector &) override;
    char get_type() const override;

    s_ptr get_left() const;
    s_ptr get_right() const;

private:
    s_ptr _left;
    s_ptr _right;
};

inline hypothesis *h_cast(s_ptr const &stat) {
    return reinterpret_cast<hypothesis *>(stat.get());
}

inline axiom *a_cast(s_ptr const &stat) {
    return reinterpret_cast<axiom *>(stat.get());
}

inline modus_ponens *mp_cast(s_ptr const &stat) {
    return reinterpret_cast<modus_ponens *>(stat.get());
}

struct hashable_expression {
    e_ptr _expr;

    hashable_expression(e_ptr const &);
    hashable_expression(hashable_expression const &);

    bool operator==(hashable_expression const &) const;
};

namespace std {

    template<>
    struct hash<hashable_expression> {
        size_t operator()(hashable_expression const &expr) const {
            return expr._expr->_hash.hash();
        }
    };

}

class proof {
public:
    proof();
    ~proof() = default;

    void set_head(head const &);
    // TODO deprecated
    void add_statement(e_ptr const &, s_ptr const &);
    // TODO also deprecated
    void add_statement(s_ptr const &);

    s_ptr get_root() const;
    s_ptr operator[](unsigned int) const;
    head const &get_head() const;
    head &get_head();
    unsigned int length() const;

    unsigned int find_hypothesis(e_ptr const &) const;
    unsigned int find_id(e_ptr const &) const;
    std::pair<unsigned int, unsigned int> find_modus_ponens(e_ptr const &expr) const;

    void add_axiom(e_ptr const &, unsigned int, unsigned int);
    unsigned int add_hypothesis(e_ptr const &, unsigned int);
    unsigned int add_modus_ponens(e_ptr const &, unsigned int);

private:
    head _context;
    std::vector<s_ptr> _statements;

    std::unordered_map<hashable_expression, unsigned int> _exists;
    std::unordered_map<hashable_expression, std::vector<unsigned int>> _mp_imp;
};

#endif // GRAMMAR_HPP

#include <iostream>
#include "grammar/grammar.hpp"
#include "proof/proof_utils.hpp"
#include "proof/proof_translator.hpp"
#include "proof/proof_creator.hpp"
#include "parser/parser.hpp"

int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

#ifdef LOCAL
//    freopen("_output.txt", "w", stdout);

    for (int n1 = 1; n1 < 2; n1++) {
        for (int n2 = 1; n2 < 2; n2++) {
            proof res;

            res = translators::prove_disjunction(e_ptr(new variable("A")), e_ptr(new variable("B")), n1, n2);
            proof_printer(res.get_head(), res.get_root()).print(proof_printer::print_policy::MARKED);
            std::cout << std::endl;

            res = translators::prove_conjunction(e_ptr(new variable("A")), e_ptr(new variable("B")), n1, n2);
            proof_printer(res.get_head(), res.get_root()).print(proof_printer::print_policy::MARKED);
            std::cout << std::endl;

            res = translators::prove_implication(e_ptr(new variable("A")), e_ptr(new variable("B")), n1, n2);
            proof_printer(res.get_head(), res.get_root()).print(proof_printer::print_policy::MARKED);
            std::cout << std::endl;
        }

        proof res = translators::prove_negation(e_ptr(new variable("A")), n1);
        proof_printer(res.get_head(), res.get_root()).print(proof_printer::print_policy::MARKED);
        std::cout << std::endl;
    }

    freopen("_input.txt", "r", stdin);
    freopen("_output.txt", "w", stdout);

    std::string line;
    std::cin >> line;
    translators::try_prove(parser(line).parse_expression());

#endif


}

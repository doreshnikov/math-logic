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
    freopen("_input.txt", "r", stdin);
    freopen("_output.txt", "w", stdout);
#endif

    std::string line;
    std::getline(std::cin, line);
    proof result = translators::prove_disjunction(e_ptr(new variable("A")), e_ptr(new variable("B")), 1, 1);
        proof_printer(result.get_head(), result.get_root()).print(proof_printer::print_policy::UNMARKED);

//    proof result = translators::try_prove(parser(line).parse_expression());
//    if (result.length() == 0) {
//        std::cout << ":(" << std::endl;
//    } else {
//        proof_printer(result.get_head(), result.get_root()).print(proof_printer::print_policy::UNMARKED);
//    }

}

#include <iostream>
#include "grammar/grammar.hpp"
#include "proof/proof_utils.hpp"
#include "proof/proof_translator.hpp"
#include "proof/proof_creator.hpp"

int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

#ifdef LOCAL
    freopen("_output.txt", "w", stdout);
#endif

//    for (int n1 = 0; n1 < 2; n1++) {
//        for (int n2 = 0; n2 < 2; n2++) {
            proof res = translators::prove_implication(e_ptr(new variable("A")), e_ptr(new variable("B")), 1, 1);
            proof_printer(res.get_head(), res.get_root()).print(proof_printer::print_policy::MARKED);
            std::cout << std::endl;
//            res.get_head().print_all();
//            for (unsigned int i = 0; i < res.length(); i++) {
//                res[i]->print();
//            }
//        }
//    }

}

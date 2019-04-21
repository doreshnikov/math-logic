#include <iostream>
#include "proof/proof_utils.hpp"
#include "proof/proof_translators.hpp"

using namespace std;

int main() {

    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

#ifdef LOCAL
    freopen("_input.txt", "r", stdin);
    freopen("_output.txt", "w", stdout);
#endif

    proof_scanner splitter;
    try {
        splitter.parse_proof();
//        proof target = translators::to_deduction(splitter.get_proof(), splitter.get_head()[0]);
//        proof_printer(target.get_head(), target.get_root()).print(proof_printer::print_policy::UNMARKED);
//
        proof cp = splitter.get_proof();
        unsigned id = cp.length();
        translators::add_contraposition(cp, e_ptr(new variable("A")), e_ptr(new variable("B")), id);
        cp.get_head().print_all();
        for (unsigned int i = 0; i < id; i++) {
            cp[i]->print();
        }
    } catch (std::logic_error const &e) {
        std::cout << "Proof is incorrect\n";
#ifdef LOCAL
        throw e;
#endif
    }

}

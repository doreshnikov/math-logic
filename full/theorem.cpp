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
    splitter.parse_proof();
    proof target = translators::to_intuitionistic(splitter.get_proof());
//    target.get_head().print_all();
//    for (unsigned int i = 0; i < target.length(); i++) {
//        target[i]->get_expression()->print("\n");
//    }
    proof_printer(target.get_head(), target.get_root()).print(proof_printer::print_policy::UNMARKED);

}

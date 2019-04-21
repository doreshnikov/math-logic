#include <iostream>
#include "proof/proof_utils.hpp"

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
        proof_printer builder(splitter.get_head(), splitter.get_proof().get_root());
        builder.print(proof_printer::print_policy::MARKED);
    } catch (std::logic_error const &e) {
        std::cout << "Proof is incorrect\n";
    }

}

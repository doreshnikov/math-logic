#include <iostream>
#include "proof/proof_utils.hpp"
#include "proof/proof_translator.hpp"

using namespace std;

int main() {

    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

#ifdef LOCAL
    freopen("_input.txt", "r", stdin);
    freopen("_output.txt", "w", stdout);
#endif

    proof_scanner scanner;
    scanner.parse_proof();
    proof target = translators::to_intuitionistic(scanner.get_proof());
    proof_printer(target.get_head(), target.get_root()).print(proof_printer::print_policy::UNMARKED);

}

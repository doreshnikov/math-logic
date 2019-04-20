#include <iostream>
#include "proof/proof_utils.hpp"

using namespace std;

int main() {
    
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

#ifdef LOCAL
    freopen("input.txt", "r", stdin);
#endif

    proof_splitter splitter;
    try {
        proof_builder builder(splitter.get_head(), splitter.parse_proof());
        builder.print();
    } catch (std::logic_error const &e) {
        std::cout << "Proof is incorrect\n";
    }

}

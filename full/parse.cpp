#include <iostream>
#include "parser/parser.hpp"

int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::string line;
    std::cin >> line;

    parser line_parser(line);
    std::cout << line_parser.parse_expression()->to_prefix() << '\n';

}

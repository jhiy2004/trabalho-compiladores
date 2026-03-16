#include "compilador.h"
#include <iostream>
#include <filesystem>
#include "util.h"

int main() {
    /* std::string teste = parse_file_to_string(std::filesystem::path(EXAMPLES_DIR) / "overflow_int.txt"); */
    std::string teste = "(1 + 1) / 3 * 9\n   \n\n\n\n   p";

    LexicalAnalysisCalc test(teste);

    test.analyze();

    const std::vector<TokenCalc> tokens = test.get_tokens();

    print_tokens_calc(tokens);

    return 0;
}

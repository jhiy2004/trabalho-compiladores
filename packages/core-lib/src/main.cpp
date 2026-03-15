#include "compilador_lalg.h"
#include <iostream>
#include <filesystem>
#include "util.h"

int main() {
    std::string teste = parse_file_to_string(std::filesystem::path(EXAMPLES_DIR) / "overflow_int.txt");

    LexicalAnalysisLALG test(teste);

    test.tokenize_all();

    const std::vector<Token> tokens = test.get_tokens();

    print_tokens(tokens);

    return 0;
}

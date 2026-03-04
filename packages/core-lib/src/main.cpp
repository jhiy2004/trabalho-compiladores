#include "compilador_lalg.h"
#include <iostream>

void print_tokens(const std::vector<Token>& tokens) {
    for (Token t : tokens) {
        std::cout << "Lexema: " << t.lexeme << ";" << t.line << ":" << t.col << "\n";
    }
}

int main() {
    LexicalAnalysisLALG test("program correto;\nint a, b, c;\nboolean d, e, f;\n");

    test.get_token();
    test.get_token();
    test.get_token();
    test.get_token();
    test.get_token();

    const std::vector<Token> tokens = test.get_tokens();

    print_tokens(tokens);

    return 0;
}

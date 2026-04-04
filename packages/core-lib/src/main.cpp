#include "compilador.h"
#include <iostream>
#include <filesystem>
#include "util.h"
#include "analisador_sintatico_procedimento.h"

int main() {
    std::string teste = parse_file_to_string(std::filesystem::path(EXAMPLES_DIR) / "variable_decl.txt");
    /*
    std::string teste = "(1 + 1) / 3 * 9\n   \n\n\n\n   p";

    LexicalAnalysisCalc test(teste);

    test.analyze();

    const std::vector<TokenCalc> tokens = test.get_tokens();

    print_tokens_calc(tokens);
    */

    SyntacticAnalyzerProcedures analysis(teste);

    analysis.run();

    auto snapshots = analysis.get_snapshots();

    int step = 1;
    for (const auto& s : snapshots) {
        std::cout << "================================================\n";
        std::cout << " PASSO " << step++ << ": " << s.action << "\n";
        std::cout << "================================================\n";

        // --- SESSÃO DE ERROS ---
        std::cout << "[ ERROS ]\n";
        auto q = s.curr_errors; // Copia por valor para poder consumir
        if (q.empty()) {
            std::cout << "  (Nenhum erro)\n";
        } else {
            while (!q.empty()) {
                // Se a sua struct tiver linha e coluna, você pode adicionar aqui:
                // std::cout << "  - [L:" << q.front().line << ", C:" << q.front().col << "] ";
                std::cout << "  - " << q.front().error << "\n";
                q.pop(); // <- CORREÇÃO DO BUG DO LOOP INFINITO
            }
        }
        std::cout << "\n";

        // --- SESSÃO DA PILHA DE SÍMBOLOS ---
        std::cout << "[ PILHA DE SÍMBOLOS (Topo -> Base) ]\n  ";
        auto stack = s.curr_symbols; // Copia por valor para poder consumir
        if (stack.empty()) {
            std::cout << "(Vazia)";
        } else {
            bool first = true;
            while (!stack.empty()) {
                if (!first) std::cout << " | ";
                std::cout << stack.top().name;
                stack.pop();
                first = false;
            }
        }
        std::cout << "\n\n";
    }

    return 0;
}

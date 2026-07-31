#include "compilador.h"
#include <iostream>
#include <iomanip>
#include <filesystem>
#include "util.h"
#include "analisador_sintatico_procedimento.h"

int main() {
    std::string teste = parse_file_to_string(std::filesystem::path(EXAMPLES_DIR) / "correto1.txt");

    SyntacticAnalyzerProcedures analysis(teste);
    analysis.run();

    std::cout << "\n================================================\n";
    std::cout << " TABELA DE SÍMBOLOS\n";
    std::cout << "================================================\n";
    std::cout << std::left
              << std::setw(15) << "Cadeia"
              << std::setw(15) << "Token"
              << std::setw(15) << "Categoria"
              << std::setw(12) << "Tipo"
              << std::setw(10) << "Valor"
              << std::setw(15) << "Escopo"
              << std::setw(12) << "Utilizada"
              << std::setw(8)  << "Linha"
              << "\n";
    std::cout << std::string(102, '-') << "\n";

    for (const auto& entry : analysis.get_tabela_simbolos().get_entradas()) {
        std::cout << std::left
                  << std::setw(15) << entry.cadeia
                  << std::setw(15) << entry.token
                  << std::setw(15) << entry.categoria_str()
                  << std::setw(12) << (entry.tipo.empty() ? "-" : entry.tipo)
                  << std::setw(10) << (entry.valor.empty() ? "-" : entry.valor)
                  << std::setw(15) << entry.escopo
                  << std::setw(12) << (entry.utilizada ? "Sim" : "Não")
                  << std::setw(8)  << entry.linha
                  << "\n";
    }

    std::cout << "\n================================================\n";
    std::cout << " ERROS SEMÂNTICOS\n";
    std::cout << "================================================\n";
    auto erros_sem = analysis.get_erros_semanticos();
    if (erros_sem.empty()) {
        std::cout << "Nenhum erro semântico encontrado!\n";
    } else {
        for (const auto& err : erros_sem) {
            std::cout << "- [Linha " << err.linha << "] " << err.mensagem << "\n";
        }
    }

    return 0;
}

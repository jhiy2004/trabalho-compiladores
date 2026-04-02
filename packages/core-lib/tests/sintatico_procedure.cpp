#include <gtest/gtest.h>
#include <filesystem>
#include "analisador_sintatico_procedimento.h"
#include "util.h"

TEST(SintaticoTest, VariableDecl) {
    std::string content = parse_file_to_string(std::filesystem::path(EXAMPLES_DIR) / "variable_decl.txt");

    SyntacticAnalyzerProcedures analise(content);
    analise.run();
}

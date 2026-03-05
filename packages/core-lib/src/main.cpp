#include "compilador_lalg.h"
#include <iostream>
#include "util.h"

std::string type_to_str(TokenType t) {
    switch (t) {
        case TokenType::ProgramWord: return "ProgramWord";
        case TokenType::ProcedureWord: return "ProcedureWord";
        case TokenType::VarWord: return "VarWord";
        case TokenType::IntWord: return "IntWord";
        case TokenType::BooleanWord: return "BooleanWord";
        case TokenType::ReadWord: return "ReadWord";
        case TokenType::WriteWord: return "WriteWord";
        case TokenType::TrueWord: return "TrueWord";
        case TokenType::FalseWord: return "FalseWord";
        case TokenType::BeginWord: return "BeginWord";
        case TokenType::EndWord: return "EndWord";
        case TokenType::IfWord: return "IfWord";
        case TokenType::ThenWord: return "ThenWord";
        case TokenType::ElseWord: return "ElseWord";
        case TokenType::WhileWord: return "WhileWord";
        case TokenType::DoWord: return "DoWord";
        case TokenType::OrWord: return "OrWord";
        case TokenType::DivWord: return "DivWord";
        case TokenType::AndWord: return "AndWord";
        case TokenType::NotWord: return "NotWord";

        case TokenType::SemiColonOp: return "SemiColonOp";
        case TokenType::ColonOp: return "ColonOp";
        case TokenType::CommaOp: return "CommaOp";
        case TokenType::DotOp: return "DotOp";
        case TokenType::EqualOp: return "EqualOp";
        case TokenType::AssignOp: return "AssignOp";
        case TokenType::DiffOp: return "DiffOp";
        case TokenType::LessOp: return "LessOp";
        case TokenType::LessEqualOp: return "LessEqualOp";
        case TokenType::GreaterEqualOp: return "GreaterEqualOp";
        case TokenType::GreaterOp: return "GreaterOp";
        case TokenType::AddOp: return "AddOp";
        case TokenType::SubOp: return "SubOp";
        case TokenType::MulOp: return "MulOp";
        case TokenType::OpenParOp: return "OpenParOp";
        case TokenType::CloseParOp: return "CloseParOp";
        case TokenType::SingleCommentOp: return "SingleCommentOp";

        case TokenType::Id: return "Id";
        case TokenType::Num: return "Num";
        case TokenType::UNK: return "UNK";

        default: return "UNKNOWN_TOKEN";
    }
}

void print_tokens(const std::vector<Token>& tokens) {
    for (Token t : tokens) {
        std::cout << "Tipo: " << type_to_str(t.type) << "\t" <<"Lexema: " << t.lexeme << "\t" << t.line << ":" << t.col << "\n";
    }
}

int main() {
    std::string teste = parse_file_to_string("/home/joseyamaoki/Documents/trabalho-compiladores/packages/core-lib/tests/examples/com.txt");

    LexicalAnalysisLALG test(teste);

    test.tokenize_all();

    const std::vector<Token> tokens = test.get_tokens();

    print_tokens(tokens);

    return 0;
}

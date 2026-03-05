#include "util.h"

#include <fstream>
#include <iomanip>

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
    if (tokens.empty()) {
        std::cout << "No Tokens\n";
        return;
    }

    std::cout << std::left
              << std::setw(20) << "Tipo"
              << std::setw(20) << "Lexema"
              << std::setw(10) << "Linha"
              << std::setw(10) << "Coluna"
              << "\n";

    std::cout << std::string(60, '-') << "\n";

    for (const Token& t : tokens) {
        std::cout << std::left
                  << std::setw(20) << type_to_str(t.type)
                  << std::setw(20) << t.lexeme
                  << std::setw(10) << t.line
                  << std::setw(10) << t.col
                  << "\n";
    }
}

std::string parse_file_to_string(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    std::ostringstream buffer;

    if (!file.is_open()) {
        std::cerr << "File not found\n";
        return std::string();
    }

    std::string line;
    while(std::getline(file, line)) {
        buffer << line << "\n";
    }

    return buffer.str();
}

std::ostream& operator<<(std::ostream& os, const Token& t) {
    os << "Token("
       << type_to_str(t.type) << ", "
       << "\"" << t.lexeme << "\", "
       << t.line << ", "
       << t.col << ")";
    return os;
}

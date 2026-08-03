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
        case TokenType::RealWord: return "RealWord";
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

std::string type_calc_to_str(TokenTypeCalc t) {
    switch (t) {
        case TokenTypeCalc::IntId: return "IntId";
        case TokenTypeCalc::RealId: return "RealId";
        case TokenTypeCalc::OpAdd: return "OpAdd";
        case TokenTypeCalc::OpSub: return "OpSub";
        case TokenTypeCalc::OpMul: return "OpMul";
        case TokenTypeCalc::OpDiv: return "OpDiv";
        case TokenTypeCalc::OpenPar: return "OpenPar";
        case TokenTypeCalc::ClosePar: return "ClosePar";
        case TokenTypeCalc::Unk: return "Unk";

        default: return "UNKNOWN_TOKEN";
    }
}

void print_tokens_calc(const std::vector<TokenCalc>& tokens) {
    std::cout << std::left
          << std::setw(20) << "Tipo"
          << std::setw(20) << "Lexema"
          << std::setw(10) << "Linha"
          << std::setw(10) << "Coluna"
          << ")\n";
    std::cout << std::string(60, '-') << "\n";
    for (const TokenCalc& token : tokens) {
        std::cout << std::left
                  << std::setw(20) << type_calc_to_str(token.type)
                  << std::setw(20) << token.lexeme
                  << std::setw(10) << token.line
                  << std::setw(10) << token.col
                  << "\n";
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

void save_program(const std::vector<Command>& cmds, const std::filesystem::path& filename) {
    std::fstream file(filename, std::ios::binary | std::ios::out);

    for (const auto& c : cmds) {
        writeBinary(file, c.type);
        if (c.arg) {
            writeBinary(file, *c.arg);
        }
    }

    file.close();
}

std::vector<Command> generate_example1_builder() {
    CodeBuilder builder;

    builder
        .inpp()
        .amem(1)
        .amem(1)
        .leit()
        .armz(0)
        .leit()
        .armz(1)
        .crvl(0)
        .crvl(1)
        .cmma()
        .dsvf("0")
        .crvl(0)
        .impe()
        .nada("0")
        .para()
    ;

    return builder.get_commands();
}

std::vector<Command> generate_example2_builder() {
    CodeBuilder builder;

    builder
        .inpp()

        .amem(1)
        .amem(1)
        .amem(1)

        .leit()
        .armz(1)

        .leit()
        .armz(2)

        .crvl(0)
        .crvl(2)
        .crct(10)
        .crvl(1)
        .divi()
        .crct(1)
        .soma()
        .mult()
        .soma()
        .armz(0)

        .crvl(0)
        .impe()

        .crvl(1)
        .impe()

        .crvl(2)
        .impe()

        .crvl(0)
        .crct(0)
        .cmma()

        .dsvf("0")

        .crvl(1)
        .crct(0)
        .cmma()

        .dsvf("1")

        .crvl(0)
        .crvl(1)
        .mult()
        .armz(1)

        .dsvs("2")

        .nada("1")

        .crvl(1)
        .crvl(0)
        .soma()
        .armz(1)

        .nada("2")

        .dsvs("3")

        .nada("0")

        .crvl(1)
        .crct(0)
        .cmma()

        .dsvf("4")

        .crvl(1)
        .crct(-1)
        .mult()
        .armz(1)

        .dsvs("5")

        .nada("4")

        .crvl(1)
        .crvl(2)
        .soma()
        .armz(1)

        .nada("5")
        .nada("3")

        .para();

    return builder.get_commands();
}


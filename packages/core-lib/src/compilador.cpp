#include "compilador.h"

bool LexicalAnalysis::analyze() {
    std::string temp{};
    for (char c : _text) {
        if (auto search = valid_chars.find(c); search == std::string::npos) 
            return false;

        if (c == ' ' || c == '\n' || c == '\t')
            continue;

        if (auto op = char_to_op_type(c); op) {
            if (!temp.empty()) {
                if (auto search = temp.find('.'); search != std::string::npos) {
                    _tokens.emplace_back(TokenType::RealId, temp);
                } else {
                    _tokens.emplace_back(TokenType::IntId, temp);
                }
            }
            _tokens.emplace_back(op.value(), std::string(1, c));
            temp.clear();
        } else {
            temp += c;
        }
    }

    if (!temp.empty()) {
        if (auto search = temp.find('.'); search != std::string::npos) {
            _tokens.emplace_back(TokenType::RealId, temp);
        } else {
            _tokens.emplace_back(TokenType::IntId, temp);
        }
    }

    return true;
}

void LexicalAnalysis::print() {
    if (_tokens.empty()) {
        std::cout << "No tokens found\n";
        return;
    }

    for(Token t : _tokens) {
        std::cout << token_type_to_str(t.type) << ":" << t.lexeme << std::endl;
    }
}

std::string LexicalAnalysis::token_type_to_str(TokenType type) {
    switch(type) {
        case TokenType::IntId: return "IntId";
        case TokenType::RealId: return "RealId";
        case TokenType::OpAdd: return "OpAdd";
        case TokenType::OpSub: return "OpSub";
        case TokenType::OpMul: return "OpMul";
        case TokenType::OpDiv: return "OpDiv";
        case TokenType::OpenPar: return "OpenPar";
        case TokenType::ClosePar: return "ClosePar";
        default: return "Unk";
    }
}

std::optional<TokenType> LexicalAnalysis::char_to_op_type(char c) {
    switch(c) {
        case '+': return TokenType::OpAdd;
        case '-': return TokenType::OpSub;
        case '*': return TokenType::OpMul;
        case '/': return TokenType::OpDiv;
        case '(': return TokenType::OpenPar;
        case ')': return TokenType::ClosePar;
        default: return std::nullopt;
    }
}

int add(int a, int b) {
    return a + b;
}
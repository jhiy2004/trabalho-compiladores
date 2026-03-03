#include "compilador.h"

bool LexicalAnalysis::analyze() {
    std::string temp{};
    unsigned int start_line{1};
    unsigned int start_col{1};
    unsigned int line{1};
    unsigned int col{1};

    for (char c : _text) {
        if (auto search = valid_chars.find(c); search == std::string::npos) 
            return false;

        if (temp.empty()) {
            start_line = line;
            start_col = col;
        }

        if (c == '\n'){
                if (auto search = temp.find('.'); search != std::string::npos) {
                    _tokens.emplace_back(TokenType::RealId, temp, start_line, start_col);
                } else {
                    _tokens.emplace_back(TokenType::IntId, temp, start_line, start_col);
                }
                temp.clear();
                
                line++;
                col = 1;
                continue;
            }

        if (c == ' ' || c == '\t'){
            
            col++;
            continue;
        }

        if (auto op = char_to_op_type(c); op) {
            if (!temp.empty()) {
                if (auto search = temp.find('.'); search != std::string::npos) {
                    _tokens.emplace_back(TokenType::RealId, temp, start_line, start_col);
                } else {
                    _tokens.emplace_back(TokenType::IntId, temp, start_line, start_col);
                }

                start_line = line;
                start_col = col;
            }
            _tokens.emplace_back(op.value(), std::string(1, c), start_line, start_col);
            temp.clear();
        } else {
            temp += c;
        }
        col++;
    }

    if (!temp.empty()) {
        if (auto search = temp.find('.'); search != std::string::npos) {
            _tokens.emplace_back(TokenType::RealId, temp, start_line, start_col);
        } else {
            _tokens.emplace_back(TokenType::IntId, temp, start_line, start_col);
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
        std::cout << token_type_to_str(t.type) << ":" << t.lexeme << ";" << t.line << ":" << t.col << std::endl;
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
#include "compilador.h"

bool LexicalAnalysisCalc::analyze() {
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
                    _tokens.emplace_back(TokenTypeCalc::RealId, temp, start_line, start_col);
                } else {
                    _tokens.emplace_back(TokenTypeCalc::IntId, temp, start_line, start_col);
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
                    _tokens.emplace_back(TokenTypeCalc::RealId, temp, start_line, start_col);
                } else {
                    _tokens.emplace_back(TokenTypeCalc::IntId, temp, start_line, start_col);
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
            _tokens.emplace_back(TokenTypeCalc::RealId, temp, start_line, start_col);
        } else {
            _tokens.emplace_back(TokenTypeCalc::IntId, temp, start_line, start_col);
        }
    }

    return true;
}

void LexicalAnalysisCalc::print() {
    if (_tokens.empty()) {
        std::cout << "No tokens found\n";
        return;
    }

    for(TokenCalc t : _tokens) {
        std::cout << token_type_to_str(t.type) << ":" << t.lexeme << ";" << t.line << ":" << t.col << std::endl;
    }
}

std::string LexicalAnalysisCalc::token_type_to_str(TokenTypeCalc type) {
    switch(type) {
        case TokenTypeCalc::IntId: return "IntId";
        case TokenTypeCalc::RealId: return "RealId";
        case TokenTypeCalc::OpAdd: return "OpAdd";
        case TokenTypeCalc::OpSub: return "OpSub";
        case TokenTypeCalc::OpMul: return "OpMul";
        case TokenTypeCalc::OpDiv: return "OpDiv";
        case TokenTypeCalc::OpenPar: return "OpenPar";
        case TokenTypeCalc::ClosePar: return "ClosePar";
        default: return "Unk";
    }
}

std::optional<TokenTypeCalc> LexicalAnalysisCalc::char_to_op_type(char c) {
    switch(c) {
        case '+': return TokenTypeCalc::OpAdd;
        case '-': return TokenTypeCalc::OpSub;
        case '*': return TokenTypeCalc::OpMul;
        case '/': return TokenTypeCalc::OpDiv;
        case '(': return TokenTypeCalc::OpenPar;
        case ')': return TokenTypeCalc::ClosePar;
        default: return std::nullopt;
    }
}

int add(int a, int b) {
    return a + b;
}

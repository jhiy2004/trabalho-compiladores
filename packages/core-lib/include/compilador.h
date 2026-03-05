#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <vector>
#include <optional>

int add(int a, int b);

enum class TokenTypeCalc {
    IntId,
    RealId,
    OpAdd,
    OpSub,
    OpMul,
    OpDiv,
    OpenPar,
    ClosePar,
};

struct TokenCalc {
    TokenTypeCalc type;
    std::string lexeme;
    unsigned int line;
    unsigned int col; 

    TokenCalc(TokenTypeCalc t, std::string_view le, unsigned int li, unsigned int c)
        : type(t), lexeme(le), line(li), col(c) {}

    bool operator==(const TokenCalc& rhs) const {
        return (type == rhs.type) && (lexeme == rhs.lexeme) && (line == rhs.line) && (col == rhs.col);
    }
};

class LexicalAnalysisCalc {
public:
    LexicalAnalysisCalc(std::string_view text) {
        _text = text;
    }

    bool analyze();
    void print();

    std::vector<TokenCalc> get_tokens() const {
        return _tokens;
    }
private:
    std::string token_type_to_str(TokenTypeCalc type);
    std::optional<TokenTypeCalc> char_to_op_type(char c);

    const std::string valid_chars = "0123456789+-/*().\n\t ";

    std::string _text;
    std::vector<TokenCalc> _tokens;
};

#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <vector>
#include <optional>

int add(int a, int b);

enum class TokenType {
    IntId,
    RealId,
    OpAdd,
    OpSub,
    OpMul,
    OpDiv,
    OpenPar,
    ClosePar,
};

struct Token {
    TokenType type;
    std::string lexeme;
    unsigned int line;
    unsigned int col; 

    Token(TokenType t, std::string_view le, unsigned int li, unsigned int c)
        : type(t), lexeme(le), line(li), col(c) {}

    bool operator==(const Token& rhs) const {
        return (type == rhs.type) && (lexeme == rhs.lexeme) && (line == rhs.line) && (col == rhs.col);
    }
};

class LexicalAnalysis {
public:
    LexicalAnalysis(std::string_view text) {
        _text = text;
    }

    bool analyze();
    void print();

    std::vector<Token> get_tokens() const {
        return _tokens;
    }
private:
    std::string token_type_to_str(TokenType type);
    std::optional<TokenType> char_to_op_type(char c);

    const std::string valid_chars = "0123456789+-/*().\n\t ";

    std::string _text;
    std::vector<Token> _tokens;
};

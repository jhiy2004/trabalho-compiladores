#include "compilador_lalg.h"

const std::unordered_map<std::string, TokenType>
LexicalAnalysisLALG::operators = {
    { ";", TokenType::SemiColonOp },
    { ".", TokenType::DotOp },
    { "=", TokenType::EqualOp },
    { ":=", TokenType::AssignOp },
    { "<>", TokenType::DiffOp },
    { "<", TokenType::LessOp },
    { "<=", TokenType::LessEqualOp },
    { ">=", TokenType::GreaterEqualOp },
    { ">", TokenType::GreaterOp },
    { "+", TokenType::AddOp },
    { "-", TokenType::SubOp },
    { ",", TokenType::CommaOp },
    { ":", TokenType::ColonOp },
    { "*", TokenType::MulOp },
    { "(", TokenType::OpenParOp },
    { ")", TokenType::CloseParOp },
    { "//", TokenType::SingleCommentOp },
};

const std::unordered_map<std::string, TokenType>
LexicalAnalysisLALG::reserved_words = {
    { "program", TokenType::ProgramWord },
    { "procedure", TokenType::ProcedureWord },
    { "var", TokenType::VarWord },
    { "int", TokenType::IntWord },
    { "boolean", TokenType::BooleanWord },
    { "read", TokenType::ReadWord },
    { "write", TokenType::WriteWord },
    { "true", TokenType::TrueWord },
    { "false", TokenType::FalseWord },
    { "begin", TokenType::BeginWord },
    { "end", TokenType::EndWord },
    { "if", TokenType::IfWord },
    { "then", TokenType::ThenWord },
    { "else", TokenType::ElseWord },
    { "while", TokenType::WhileWord },
    { "do", TokenType::DoWord },
    { "or", TokenType::OrWord },
    { "div", TokenType::DivWord },
    { "and", TokenType::AndWord },
    { "not", TokenType::NotWord },
};

std::optional<Token> LexicalAnalysisLALG::get_token() {
    std::size_t text_size = _text.size();
    
    if (curr_pos >= text_size) {
        return std::nullopt;
    }

    while(curr_pos < text_size) {
        char c = _text[curr_pos];

        if (!is_separator(c))
            break;

        if (is_newline(c)) {
            curr_line++;
            curr_col = 1;
        } else {
            curr_col++; 
        }
        curr_pos++;
    }

    if(curr_pos >= text_size) {
        return std::nullopt;
    }

    std::string buffer;
    const unsigned int start_col = curr_col;
    const unsigned int start_line = curr_line;

    const char first_c = _text[curr_pos++];
    buffer += first_c;

    curr_col++;
    if(is_letter(first_c)) {
        while(curr_pos < text_size) {
            char c = _text[curr_pos];

            if (!is_letter(c) && !is_digit(c))
                break;

            buffer += c;
            curr_col++;
            curr_pos++;
        }

        if(auto it = reserved_words.find(buffer); it != reserved_words.end()) {
            tokens.emplace_back(it->second, it->first, start_line, start_col);
            return tokens.back();
        }

        tokens.emplace_back(TokenType::Id, buffer, start_line, start_col);
        return tokens.back();
    } else if(is_op(first_c)) {
        while (curr_pos < text_size) {
            char c = _text[curr_pos];

            if (!is_op(c))
                break;

            if (auto it = operators.find(buffer + c); it == operators.end()) {
                break; 
            }
            buffer += c;
            curr_col++;
            curr_pos++;
        }

        if (auto it = operators.find(buffer); it != operators.end()) {
            if(it->second == TokenType::SingleCommentOp) {
                while(curr_pos < text_size) {
                    char c = _text[curr_pos];

                    if (is_newline(c))
                        break;

                    curr_pos++;
                }
                curr_pos++;
                curr_col = 1;
                curr_line++;
                return get_token();
            }
            tokens.emplace_back(it->second, it->first, start_line, start_col);
            return tokens.back();
        }
    } else if(is_digit(first_c)) {
        while (curr_pos < text_size) {
            char c = _text[curr_pos];

            if (!is_digit(c))
                break;

            buffer += c;
            curr_pos++;
            curr_col++;
        }
        tokens.emplace_back(TokenType::Num, buffer, start_line, start_col);
        return tokens.back();
    } else if(first_c == start_multiline_comment) {
        while(curr_pos < text_size) {
            char c = _text[curr_pos];

            if (c == end_multiline_comment)
                break;

            if (is_newline(c)) {
                curr_line++;
                curr_col = 1;
            }
            curr_pos++;
        }

        if (curr_pos >= text_size) {
            tokens.emplace_back(TokenType::UNK, "malformed_multiline_comment", start_line, start_col);
            return tokens.back();
        }

        if (_text[curr_pos] == end_multiline_comment) {
            curr_col++;
            curr_pos++;
        }

        return get_token();
    }

    tokens.emplace_back(TokenType::UNK, std::string(1, first_c), start_line, start_col);
    return tokens.back();
}

std::vector<Token> LexicalAnalysisLALG::get_tokens() const {
    return tokens;
}


std::vector<Token> LexicalAnalysisLALG::tokenize_all() {
    while(get_token().has_value() == true);
    return tokens;
}

bool LexicalAnalysisLALG::is_digit(char c) {
    return std::isdigit(c);
}

bool LexicalAnalysisLALG::is_letter(char c) {
    return c == '_' || std::isalpha(c);
}

bool LexicalAnalysisLALG::is_op(char c) {
    return c == '+' || c == '-' || c == '*' || c == '=' || c == '<' || c == '>' || c == '(' || c == ')' || c == ';' || c == '.' || c == '/' || c == ',' || c == ':';
}

bool LexicalAnalysisLALG::is_newline(char c) {
    return c == '\n';
}

bool LexicalAnalysisLALG::is_separator(char c) {
    return c == ' ' || c == '\n' || c == '\t';
}

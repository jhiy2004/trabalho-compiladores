#pragma once

#include "compilador_lalg.h"
#include <queue>
#include <stack>

struct StackElem {
    bool terminal; 
    std::string name;
};

struct SyntacticError {
    std::string error;
    unsigned int line;
    unsigned int col;
};

class SyntacticAnalyzerProcedures {
public:
    SyntacticAnalyzerProcedures(std::string_view text) : _lexical(text) {}

    void run() {
        // Get the first token
        advance();
        program();
    }
private:
    void program() {
        symbols.push(StackElem{
            .terminal = true,
            .name = "."
        });

        symbols.push(StackElem{
            .terminal = false,
            .name = "bloco"
        });

        symbols.push(StackElem{
            .terminal = true,
            .name = ";"
        });

        symbols.push(StackElem{
            .terminal = true,
            .name = "id"
        });

        symbols.push(StackElem{
            .terminal = true,
            .name = "program"
        });

        std::optional<Token> lookahead = _lexical.get_token();
        if (!match(TokenType::ProgramWord)) {
            enqueue_error(lookahead->line, lookahead->col, "program word not found");
        }

        if (!match(TokenType::Id)) {
            enqueue_error(lookahead->line, lookahead->col, "program without id");
        }

        if (match(TokenType::SemiColonOp)) {
            enqueue_error(lookahead->line, lookahead->col, "program without semicolon");
        }

        block();

        if (match(TokenType::DotOp)) {
            enqueue_error(lookahead->line, lookahead->col, "program without dot");
        }
    }

    void enqueue_error(unsigned int line, unsigned int col, std::string_view message) {
        errors.push(SyntacticError{
            .error = std::string(message),
            .line = line,
            .col = col
        });
    }

    bool match(TokenType type) {
        if (lookahead.has_value()) {
            return false;
        }

        if (lookahead.value().type != type) {
            return false;
        }

        advance();
        return true;
    }

    void block() {
        std::optional<Token> curr_token = _lexical.get_token();

    }

    void variable_declaration_part() {
        _lexical.get_token();
    }

    void variable_declaration() {
        if(!match(TokenType::IntWord) && !match(TokenType::BooleanWord)) {
            return;
        }

        identifier_list();
    }

    void identifier_list() {
        if(!match(TokenType::Id)) {
            enqueue_error(lookahead->line, lookahead->col, "identifier list without id");
        }

        while (true) {
            if (!match(TokenType::CommaOp)) {
                break;
            }

            if (!match(TokenType::Id)) {
                enqueue_error(lookahead->line, lookahead->col,  "identifier list without id");
                break;
            }
        }

    }

    void advance() {
        lookahead = _lexical.get_token();
    }

    LexicalAnalysisLALG _lexical;
    std::optional<Token> lookahead;
    std::stack<StackElem> symbols;
    std::queue<SyntacticError> errors;
};

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

struct Snapshot {
    std::optional<Token> curr_token;
    std::stack<StackElem> curr_symbols;
    std::queue<SyntacticError> curr_errors;
    std::string action;
};

enum class NonTerminal {
    Program,
    Block,
    VariableDeclarationPart,
    VariableDeclaration,
    IdentifierList,
    SubroutinesDeclarationPart,
    ProcedureDeclaration,
    FormalParameters,
    FormalParametersSection,
    Type,
    Identifier,
    CompoundCommand,
    Command,
    Assign,
    ProcedureCall,
    ConditionalCommand,
    Expression,
    RepetitiveCommand,
    Relation,
    SimpleExpression,
    Term,
    Factor,
    Variable,
    ExpressionList,
};

class SyntacticAnalyzerProcedures {
public:
    SyntacticAnalyzerProcedures(std::string_view text) : _lexical(text) {}

    void run();

    std::queue<SyntacticError> get_errors() const;
    std::stack<StackElem> get_symbols() const;
    std::vector<Snapshot> get_snapshots() const;
private:
    void enqueue_error(unsigned int line, unsigned int col, std::string_view message);
    void stack_non_terminal(NonTerminal nt);
    void stack_terminal(TokenType token);
    void stack_symbol(bool terminal, const std::string& name);

    void record_snapshot(const std::string action);

    void advance();
    bool peek(TokenType type);
    bool match(TokenType type);
    bool expect(TokenType expected_type, std::string_view error_message);

    bool is_type();

    void program();
    void block();
    void type();
    void variable_declaration_part();
    void variable_declaration();
    void identifier_list();

    LexicalAnalysisLALG _lexical;
    std::optional<Token> lookahead;
    std::stack<StackElem> symbols;
    std::queue<SyntacticError> errors;
    std::vector<Snapshot> snapshots;

    static const std::unordered_map<NonTerminal, std::string> non_terminals;
    static const std::unordered_map<TokenType, std::string> terminals;
};

#pragma once

#include "compilador_lalg.h"
#include "tabela_simbolos.h"
#include "analisador_semantico.h"
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

struct IdentInfo {
    std::string cadeia;
    unsigned int linha;
    unsigned int col;
};

class SyntacticAnalyzerProcedures {
public:
    SyntacticAnalyzerProcedures(std::string_view text) 
        : _lexical(text), analisador_semantico(tabela_simbolos) {}

    void run();

    std::queue<SyntacticError> get_errors() const;
    std::stack<StackElem> get_symbols() const;
    std::vector<Snapshot> get_snapshots() const;
    const TabelaSimbolos& get_tabela_simbolos() const;
    std::vector<SemanticError> get_erros_semanticos() const;

    void print_current_lexeme();

private:
    void enqueue_error(std::string_view message);
    void stack_non_terminal(NonTerminal nt);
    void stack_terminal(TokenType token);
    void stack_symbol(bool terminal, const std::string& name);
    void pop_symbol();

    void record_snapshot(const std::string action);

    void advance();
    bool peek(TokenType type);
    bool match(TokenType type);
    bool expect(TokenType expected_type, std::string_view error_message);

    bool is_type();
    bool is_relation();

    void program();
    void block();
    std::string type();
    void variable_declaration_part();
    void variable_declaration();
    std::vector<IdentInfo> identifier_list();

    void subroutines_declaration_part();
    void procedure_declaration();
    void formal_parameters(std::vector<ParametroSimbolo>& params);
    void formal_parameters_section(std::vector<ParametroSimbolo>& params);

    void compound_command();

    void command();
    void assign(const IdentInfo& id_info);
    ArgExpr variable();
    void procedure_call(const IdentInfo& proc_info);
    void conditional_command_1();
    void repetitive_command_1();

    std::vector<ArgExpr> expression_list();

    ArgExpr expression();
    ArgExpr simple_expression();
    ArgExpr term();
    void relation();
    ArgExpr factor();

    LexicalAnalysisLALG _lexical;
    std::optional<Token> lookahead;
    std::stack<StackElem> symbols;
    std::queue<SyntacticError> errors;
    std::vector<Snapshot> snapshots;

    TabelaSimbolos tabela_simbolos;
    AnalisadorSemantico analisador_semantico;
    std::string escopo_atual{"global"};

    static const std::unordered_map<NonTerminal, std::string> non_terminals;
    static const std::unordered_map<TokenType, std::string> terminals;
};

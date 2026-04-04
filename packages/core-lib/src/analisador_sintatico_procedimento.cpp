#include "analisador_sintatico_procedimento.h"

void SyntacticAnalyzerProcedures::run() {
    // Get the first token
    advance();
    program();
}

std::queue<SyntacticError> SyntacticAnalyzerProcedures::get_errors() const {
    return errors;
}

std::stack<StackElem> SyntacticAnalyzerProcedures::get_symbols() const {
    return symbols;
}

std::vector<Snapshot> SyntacticAnalyzerProcedures::get_snapshots() const {
    return snapshots;
}

bool SyntacticAnalyzerProcedures::expect(TokenType expected_type, std::string_view error_message) {
    if (match(expected_type)) {
        return true;
    }

    unsigned int line = lookahead ? lookahead->line : 0; // Fallback caso seja EOF
    unsigned int col  = lookahead ? lookahead->col : 0;

    enqueue_error(line, col, error_message);

    return false;
}

void SyntacticAnalyzerProcedures::stack_terminal(TokenType token) {
    auto it = terminals.find(token);
    if (it == terminals.end()) {
        return;
    }

    stack_symbol(true, it->second);
}

void SyntacticAnalyzerProcedures::stack_non_terminal(NonTerminal nt) {
    auto it = non_terminals.find(nt);
    if (it == non_terminals.end()) {
        return;
    }

    stack_symbol(false, it->second);
}

void SyntacticAnalyzerProcedures::program() {
    stack_terminal(TokenType::DotOp);
    stack_non_terminal(NonTerminal::Block);
    stack_terminal(TokenType::SemiColonOp);
    stack_non_terminal(NonTerminal::Identifier);
    stack_terminal(TokenType::ProgramWord);

    record_snapshot("Stacked program non-terminal");

    expect(TokenType::ProgramWord,  "program word not found");
    symbols.pop();
    record_snapshot("Popped program");

    expect(TokenType::Id, "program without id");
    symbols.pop();
    record_snapshot("Popped id");

    expect(TokenType::SemiColonOp, "program without semicolon");
    symbols.pop();
    record_snapshot("Popped ;");

    symbols.pop();
    record_snapshot("Popped <block>");
    block();

    expect(TokenType::DotOp, "program without dot");
    symbols.pop();
    record_snapshot("Popped .");

}

void SyntacticAnalyzerProcedures::record_snapshot(const std::string action) {
    auto snap = Snapshot{
        .curr_token = lookahead,
        .curr_symbols = symbols,
        .curr_errors = errors,
        .action = action,
    };

    snapshots.push_back(snap);
}

void SyntacticAnalyzerProcedures::stack_symbol(bool terminal, const std::string& name) {
    symbols.push(StackElem{
        .terminal = terminal,
        .name = name
    });
}

void SyntacticAnalyzerProcedures::enqueue_error(unsigned int line, unsigned int col, std::string_view message) {
    errors.push(SyntacticError{
        .error = std::string(message),
        .line = line,
        .col = col
    });
}

bool SyntacticAnalyzerProcedures::peek(TokenType type) {
    if (!lookahead.has_value()) {
        return false;
    }

    if (lookahead.value().type != type) {
        return false;
    }

    return true;
}

bool SyntacticAnalyzerProcedures::match(TokenType type) {
    if (!lookahead.has_value()) {
        return false;
    }

    if (lookahead.value().type != type) {
        return false;
    }

    advance();
    return true;
}

void SyntacticAnalyzerProcedures::block() {
    stack_non_terminal(NonTerminal::CompoundCommand);
    stack_non_terminal(NonTerminal::SubroutinesDeclarationPart);
    stack_non_terminal(NonTerminal::VariableDeclarationPart);
    record_snapshot("Stacked block");

    symbols.pop();
    record_snapshot("Popped <variable_declaration_part>");
    variable_declaration_part();

    symbols.pop();
    record_snapshot("Popped <subroutines_declaration_part>");

    symbols.pop();
    record_snapshot("Popped <compound_command>");
}

bool SyntacticAnalyzerProcedures::is_type() {
    return (peek(TokenType::IntWord) || peek(TokenType::BooleanWord));
}

void SyntacticAnalyzerProcedures::type() {
    if (match(TokenType::IntWord)) return;
    expect(TokenType::BooleanWord, "Expected a valid type");
}

void SyntacticAnalyzerProcedures::variable_declaration_part() {
    if (!is_type()) {
        return;
    }

    stack_terminal(TokenType::SemiColonOp);
    stack_non_terminal(NonTerminal::VariableDeclaration);
    record_snapshot("Stacked variable_declaration_part");

    symbols.pop();
    record_snapshot("Popped <variable_declaration>");
    variable_declaration();

    if (!expect(TokenType::SemiColonOp, "variable decl without ';")) {
        return;
    }

    symbols.pop();
    record_snapshot("Popped ;");

    while (is_type()) {
        stack_terminal(TokenType::SemiColonOp);
        stack_non_terminal(NonTerminal::VariableDeclaration);

        symbols.pop();
        record_snapshot("Popped <variable_declaration>");
        variable_declaration();

        if (!expect(TokenType::SemiColonOp, "variable decl without ';")) {
            return;
        }

        symbols.pop();
        record_snapshot("Popped ;");
    }
}

void SyntacticAnalyzerProcedures::variable_declaration() {
    if(!is_type()) {
        return;
    }

    stack_non_terminal(NonTerminal::IdentifierList);
    stack_non_terminal(NonTerminal::Type);
    record_snapshot("Stacked variable_declaration");

    symbols.pop();
    record_snapshot("Popped <type>");
    type();

    symbols.pop();
    record_snapshot("Popped <identifier_list>");
    identifier_list();
}

void SyntacticAnalyzerProcedures::identifier_list() {
    stack_non_terminal(NonTerminal::Identifier);
    record_snapshot("Stacked identifier list");

    expect(TokenType::Id, "identifier list without id");

    symbols.pop();
    record_snapshot("Popped identifier");

    while (true) {
        if (!match(TokenType::CommaOp)) {
            break;
        }
        stack_non_terminal(NonTerminal::Identifier);
        stack_terminal(TokenType::CommaOp);
        record_snapshot("Stacked identifier list");

        symbols.pop();
        record_snapshot("Popped ,");

        if (!expect(TokenType::Id,  "identifier list without id")) {
            break;
        }

        symbols.pop();
        record_snapshot("Popped identifier");
    }
}

void SyntacticAnalyzerProcedures::advance() {
    lookahead = _lexical.get_token();
}

const std::unordered_map<NonTerminal, std::string> SyntacticAnalyzerProcedures::non_terminals = {
    { NonTerminal::Program, "<program>" },
    { NonTerminal::Block, "<block>" },
    { NonTerminal::CompoundCommand, "<compound_command>" },
    { NonTerminal::VariableDeclaration, "<variable_declaration>" },
    { NonTerminal::VariableDeclarationPart, "<variable_declaration_part>" },
    { NonTerminal::SubroutinesDeclarationPart, "<subroutines_declaration_part>"},
    { NonTerminal::IdentifierList, "<identifier_list>"},
    { NonTerminal::Identifier, "<identifier>"},
    { NonTerminal::Type, "<type>"},
};

const std::unordered_map<TokenType, std::string> SyntacticAnalyzerProcedures::terminals = {
    { TokenType::CommaOp, "," },
    { TokenType::SemiColonOp, ";" },
    { TokenType::DotOp, "." },
    { TokenType::ProgramWord, "program" },
};

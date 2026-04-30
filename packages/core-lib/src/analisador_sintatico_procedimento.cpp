#include "analisador_sintatico_procedimento.h"
#include "compilador_lalg.h"

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

    enqueue_error(error_message);

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

void SyntacticAnalyzerProcedures::enqueue_error(std::string_view message) {
    unsigned int line = lookahead ? lookahead->line : 0;
    unsigned int col  = lookahead ? lookahead->col : 0;

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
    compound_command();
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


void SyntacticAnalyzerProcedures::compound_command() {
    stack_terminal(TokenType::EndWord);
    stack_non_terminal(NonTerminal::Command);
    stack_terminal(TokenType::BeginWord);
    record_snapshot("Stacked compound_command");

    expect(TokenType::BeginWord, "Compound command without begin");

    symbols.pop();
    record_snapshot("Popped begin");

    symbols.pop();
    record_snapshot("Popped command");
    command();

    while (true) {
        if (!match(TokenType::SemiColonOp)) {
            break;
        }

        stack_non_terminal(NonTerminal::Command);
        stack_terminal(TokenType::SemiColonOp);
        record_snapshot("Stacked compound_command ext");

        symbols.pop();
        record_snapshot("Popped semicolon");

        symbols.pop();
        record_snapshot("Popped command");
        command();
    }

    expect(TokenType::EndWord, "Compound command without end");
    symbols.pop();
    record_snapshot("Popped end");
}

void SyntacticAnalyzerProcedures::command() {
    if (peek(TokenType::Id)) {
        match(TokenType::Id);

        if (peek(TokenType::OpenParOp)) {
            stack_non_terminal(NonTerminal::ProcedureCall);
            record_snapshot("Stacked procedure_call");

            symbols.pop();
            record_snapshot("Popped procedure_call");

            procedure_call();
            return;
        } else if(peek(TokenType::AssignOp) || peek(TokenType::AddOp) || peek(TokenType::SubOp)) {
            stack_non_terminal(NonTerminal::Assign);
            record_snapshot("Stacked assign");

            symbols.pop();
            record_snapshot("Popped assign");

            assign();
            return;
        }

        enqueue_error("Malformed assign or procedure call");
        return;
    }

    if (peek(TokenType::BeginWord)) {
        stack_non_terminal(NonTerminal::CompoundCommand);
        record_snapshot("Stacked compound_command");

        symbols.pop();
        record_snapshot("Popped compound_command");
        compound_command();
        return;
    }

    if (peek(TokenType::IfWord)) {
        stack_non_terminal(NonTerminal::ConditionalCommand);
        record_snapshot("Stacked conditional_command_1");

        symbols.pop();
        record_snapshot("Popped conditional_command_1");

        conditional_command_1();
        return;
    }

    if (peek(TokenType::WhileWord)) {
        stack_non_terminal(NonTerminal::RepetitiveCommand);
        record_snapshot("Stacked repetitive_command_1");

        symbols.pop();
        record_snapshot("Popped repetitive_command_1");

        repetitive_command_1();
        return;
    }
}

void SyntacticAnalyzerProcedures::assign() {
    stack_non_terminal(NonTerminal::Expression);
    stack_terminal(TokenType::AssignOp);
    stack_non_terminal(NonTerminal::Variable);
    record_snapshot("Stacked assign");

    symbols.pop();
    record_snapshot("Popped variable");
    variable();

    if (!expect(TokenType::AssignOp, "Malformed assing op, missing ':='")) {
        return;
    }
    symbols.pop();
    record_snapshot("Popped AssignOp");

    symbols.pop();
    record_snapshot("Popped expression");
    expression();
}

void SyntacticAnalyzerProcedures::variable() {
    stack_non_terminal(NonTerminal::Identifier);
    record_snapshot("Stacked variable identifier");

    symbols.pop();
    record_snapshot("Popped identifier");

    if (peek(TokenType::AddOp) || peek(TokenType::SubOp) || peek(TokenType::Num) ||
        peek(TokenType::OpenParOp) || peek(TokenType::NotWord) || peek(TokenType::Id)) {
        stack_non_terminal(NonTerminal::Expression);
        record_snapshot("Stacked variable expression");

        symbols.pop();
        record_snapshot("Popped expression");
        expression();
    }
}

void SyntacticAnalyzerProcedures::procedure_call() {
    if (!match(TokenType::OpenParOp)) {
        return;
    }

    stack_terminal(TokenType::CloseParOp);
    stack_non_terminal(NonTerminal::ExpressionList);
    stack_terminal(TokenType::OpenParOp);

    record_snapshot("Stacked procedure_call");

    symbols.pop();
    record_snapshot("Popped (");

    symbols.pop();
    record_snapshot("Popped expression_list");
    expression_list();

    expect(TokenType::CloseParOp, "Malformed procedure call, missing ')'");
    symbols.pop();
    record_snapshot("Popped )");
}

void SyntacticAnalyzerProcedures::conditional_command_1() {
    stack_non_terminal(NonTerminal::Command);
    stack_terminal(TokenType::ThenWord);
    stack_non_terminal(NonTerminal::Expression);
    stack_terminal(TokenType::IfWord);
    record_snapshot("Stacked conditional_command_1");

    if (expect(TokenType::IfWord, "Malformed if command, missing if")) {
        return;
    }

    symbols.pop();
    record_snapshot("Popped if");

    symbols.pop();
    record_snapshot("Popped expression");
    expression();

    if (expect(TokenType::ThenWord, "Malformed if command, missing then")) {
        return;
    }
    symbols.pop();
    record_snapshot("Popped then");

    symbols.pop();
    record_snapshot("Popped command");
    command();

    if (match(TokenType::ElseWord)) {
        stack_terminal(TokenType::ElseWord);
        record_snapshot("Stacked else");

        symbols.pop();
        record_snapshot("Popped command");
        command();
    }
}

void SyntacticAnalyzerProcedures::repetitive_command_1() {
    if (expect(TokenType::WhileWord, "Malformed repetitive_command_1, missing while")) return;
    expression();
    if (expect(TokenType::DoWord, "Malformed repetitive_command_1, missing do")) return;
    command();
}

void SyntacticAnalyzerProcedures::expression_list() {
    stack_non_terminal(NonTerminal::Expression);
    record_snapshot("Stacked expression list");

    symbols.pop();
    record_snapshot("Popped expression list");
    expression();

    while(true) {
        if (match(TokenType::CommaOp)) {
            stack_non_terminal(NonTerminal::Expression);
            stack_terminal(TokenType::CommaOp);
            record_snapshot("Stacked expression list");

            symbols.pop();
            record_snapshot("Popped ,");

            symbols.pop();
            record_snapshot("Popped expression");
            expression();
        }
    }
}

void SyntacticAnalyzerProcedures::expression() {
    stack_non_terminal(NonTerminal::SimpleExpression);
    record_snapshot("Stacked expression");

    symbols.pop();
    record_snapshot("Popped simple_expression");
    simple_expression();

    if (is_relation()) {
        stack_non_terminal(NonTerminal::SimpleExpression);
        stack_non_terminal(NonTerminal::Relation);
        record_snapshot("Stacked expression");

        symbols.pop();
        record_snapshot("Popped relation");
        relation();

        symbols.pop();
        record_snapshot("Popped simple_expression");
        simple_expression();
    }
}

void SyntacticAnalyzerProcedures::simple_expression() {
    stack_non_terminal(NonTerminal::Term);
    if (match(TokenType::AddOp)) {
        stack_terminal(TokenType::AddOp);
        record_snapshot("Stacked AddOp");

        symbols.pop();
        record_snapshot("Popped AddOp");

        return;
    } else if(match(TokenType::SubOp)) {
        stack_terminal(TokenType::SubOp);
        record_snapshot("Stacked SubOp");

        symbols.pop();
        record_snapshot("Popped SubOp");
    } else {
        return;
    }

    symbols.pop();
    record_snapshot("Popped term");
    term();

    while (true) {
        TokenType prev_type = lookahead->type;
        if(match(TokenType::AddOp) || match(TokenType::SubOp) || match(TokenType::DivWord)) {
            stack_non_terminal(NonTerminal::Term);
            if (prev_type == TokenType::AddOp) {
                stack_terminal(TokenType::AddOp);
                record_snapshot("Stacked simple_expression");

                symbols.pop();
                record_snapshot("Popped AddOp");
            } else if(prev_type == TokenType::SubOp) {
                stack_terminal(TokenType::SubOp);
                record_snapshot("Stacked simple_expression");

                symbols.pop();
                record_snapshot("Popped SubOp");
            } else if(prev_type == TokenType::DivWord) {
                stack_terminal(TokenType::DivWord);
                record_snapshot("Stacked simple_expression");

                symbols.pop();
                record_snapshot("Popped DivWord");
            }

            symbols.pop();
            record_snapshot("Popped term");
            term();
        } else {
            return;
        }
    }
}

void SyntacticAnalyzerProcedures::term() {
    stack_non_terminal(NonTerminal::Factor);
    record_snapshot("Stacked term");

    symbols.pop();
    record_snapshot("Popped factor");
    factor();

    while (true) {
        TokenType prev_type = lookahead->type;
        if (match(TokenType::MulOp) || match(TokenType::DivWord) || match(TokenType::AndWord)) {
            stack_non_terminal(NonTerminal::Factor);
            if (prev_type == TokenType::MulOp) {
                stack_terminal(TokenType::MulOp);
                record_snapshot("Stacked MulOp");

                symbols.pop();
                record_snapshot("Popped MulOp");
            } else if(prev_type == TokenType::DivWord) {
                stack_terminal(TokenType::DivWord);
                record_snapshot("Stacked DivWord");

                symbols.pop();
                record_snapshot("Popped DivWord");
            } else if(prev_type == TokenType::AndWord) {
                stack_terminal(TokenType::AndWord);
                record_snapshot("Stacked AndWord");

                symbols.pop();
                record_snapshot("Popped AndWord");
            }

            symbols.pop();
            record_snapshot("Popped Factor");
            factor();
        } else {
            return;
        }
    }
}

bool SyntacticAnalyzerProcedures::is_relation() {
    return (peek(TokenType::EqualOp) ||
    peek(TokenType::DiffOp) ||
    peek(TokenType::LessOp) ||
    peek(TokenType::LessEqualOp) ||
    peek(TokenType::GreaterEqualOp) ||
    peek(TokenType::GreaterOp));
}

void SyntacticAnalyzerProcedures::relation() {
    if (match(TokenType::EqualOp)) return;
    if (match(TokenType::DiffOp)) return;
    if (match(TokenType::LessOp)) return;
    if (match(TokenType::LessEqualOp)) return;
    if (match(TokenType::GreaterEqualOp)) return;
    expect(TokenType::GreaterOp, "Expect a valid relation");
}

void SyntacticAnalyzerProcedures::factor() {
    if (match(TokenType::Num)) {
        stack_terminal(TokenType::Num);
        record_snapshot("Stacked factor");
        return;
    } else if (match(TokenType::OpenParOp)){
        stack_terminal(TokenType::CloseParOp);
        stack_non_terminal(NonTerminal::Expression);
        stack_terminal(TokenType::OpenParOp);
        record_snapshot("Stacked factor");

        symbols.pop();
        record_snapshot("Popped OpenPar");

        symbols.pop();
        record_snapshot("Popped expression");
        expression();

        expect(TokenType::CloseParOp, "Malformed factor, missing ')'");

        symbols.pop();
        record_snapshot("Popped CloseParOp");
        return;
    } else if(match(TokenType::NotWord)) {
        stack_non_terminal(NonTerminal::Factor);
        stack_terminal(TokenType::NotWord);
        record_snapshot("Stacked factor");

        symbols.pop();
        record_snapshot("Popped NotWord");

        symbols.pop();
        record_snapshot("Popped factor");
        factor(); 
        return;
    } else if(match(TokenType::Id)) {
        stack_non_terminal(NonTerminal::Variable);
        record_snapshot("Stacked factor");

        symbols.pop();
        record_snapshot("Popped variable");
        factor();
        return;
    }

    enqueue_error("Malformed factor");
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

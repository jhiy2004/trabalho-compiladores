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

void SyntacticAnalyzerProcedures::pop_symbol() {
    if (symbols.empty()) {
        enqueue_error("Internal error: stack underflow");
        return;
    }
}

void SyntacticAnalyzerProcedures::program() {
    stack_terminal(TokenType::DotOp);
    stack_non_terminal(NonTerminal::Block);
    stack_terminal(TokenType::SemiColonOp);
    stack_non_terminal(NonTerminal::Identifier);
    stack_terminal(TokenType::ProgramWord);

    record_snapshot("Stacked program non-terminal");

    expect(TokenType::ProgramWord,  "program word not found");
    pop_symbol();
    record_snapshot("Popped program");

    expect(TokenType::Id, "program without id");
    pop_symbol();
    record_snapshot("Popped id");

    expect(TokenType::SemiColonOp, "program without semicolon");
    pop_symbol();
    record_snapshot("Popped ;");

    pop_symbol();
    record_snapshot("Popped <block>");
    block();

    expect(TokenType::DotOp, "program without dot");
    pop_symbol();
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

    pop_symbol();
    record_snapshot("Popped <variable_declaration_part>");
    variable_declaration_part();

    pop_symbol();
    record_snapshot("Popped <subroutines_declaration_part>");
    subroutines_declaration_part();

    pop_symbol();
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

    pop_symbol();
    record_snapshot("Popped <variable_declaration>");
    variable_declaration();

    if (!expect(TokenType::SemiColonOp, "variable decl without ';")) {
        return;
    }

    pop_symbol();
    record_snapshot("Popped ;");

    while (is_type()) {
        stack_terminal(TokenType::SemiColonOp);
        stack_non_terminal(NonTerminal::VariableDeclaration);

        pop_symbol();
        record_snapshot("Popped <variable_declaration>");
        variable_declaration();

        if (!expect(TokenType::SemiColonOp, "variable decl without ';")) {
            return;
        }

        pop_symbol();
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

    pop_symbol();
    record_snapshot("Popped <type>");
    type();

    pop_symbol();
    record_snapshot("Popped <identifier_list>");
    identifier_list();
}

void SyntacticAnalyzerProcedures::subroutines_declaration_part() {
    while (peek(TokenType::ProcedureWord)) {
        stack_terminal(TokenType::SemiColonOp);
        stack_non_terminal(NonTerminal::ProcedureDeclaration);
        record_snapshot("Stacked subroutines_declaration_part iteration");

        pop_symbol();
        record_snapshot("Popped <procedure_declaration>");
        procedure_declaration();

        if (!expect(TokenType::SemiColonOp, "Subroutine declaration without ';'")) {
            return;
        }

        pop_symbol();
        record_snapshot("Popped ;");
    }
}

void SyntacticAnalyzerProcedures::procedure_declaration() {
    if (!expect(TokenType::ProcedureWord, "Expected 'procedure' keyword")) return;

    stack_non_terminal(NonTerminal::Identifier);
    record_snapshot("Stacked procedure identifier");

    pop_symbol();
    record_snapshot("Popped <identifier>");
    
    if (!expect(TokenType::Id, "Procedure without identifier")) return;

    if (peek(TokenType::OpenParOp)) {
        stack_non_terminal(NonTerminal::FormalParameters);
        record_snapshot("Stacked <formal_parameters>");

        pop_symbol();
        record_snapshot("Popped <formal_parameters>");
        formal_parameters();
    }

    stack_terminal(TokenType::SemiColonOp);
    record_snapshot("Stacked semicolon for procedure signature");

    pop_symbol();
    record_snapshot("Popped ;");

    if (!expect(TokenType::SemiColonOp, "Procedure signature without ';'")) return;

    stack_non_terminal(NonTerminal::Block);
    record_snapshot("Stacked procedure <block>");

    pop_symbol();
    record_snapshot("Popped <block>");
    block();
}

void SyntacticAnalyzerProcedures::formal_parameters() {
    if (!expect(TokenType::OpenParOp, "Expected '(' for formal parameters")) return;

    stack_non_terminal(NonTerminal::FormalParametersSection);
    record_snapshot("Stacked formal parameters section");

    pop_symbol();
    record_snapshot("Popped <formal_parameters_section>");
    formal_parameters_section();

    while (match(TokenType::SemiColonOp)) {
        stack_non_terminal(NonTerminal::FormalParametersSection);
        record_snapshot("Stacked formal parameters section loop");

        pop_symbol();
        record_snapshot("Popped <formal_parameters_section>");
        formal_parameters_section();
    }

    expect(TokenType::CloseParOp, "Expected ')' ending formal parameters");
}

void SyntacticAnalyzerProcedures::formal_parameters_section() {
    if (match(TokenType::VarWord)) {
        stack_terminal(TokenType::VarWord);
        record_snapshot("Stacked var keyword");
        
        pop_symbol();
        record_snapshot("Popped var keyword");
    }

    stack_non_terminal(NonTerminal::IdentifierList);
    record_snapshot("Stacked identifier list in parameters");

    pop_symbol();
    record_snapshot("Popped <identifier_list>");
    identifier_list();

    if (!expect(TokenType::ColonOp, "Expected ':' in formal parameters section")) return;

    stack_non_terminal(NonTerminal::Identifier);
    record_snapshot("Stacked type identifier in parameters");

    pop_symbol();
    record_snapshot("Popped <identifier>");
    
    expect(TokenType::Id, "Expected type identifier after ':'"); 
}

void SyntacticAnalyzerProcedures::identifier_list() {
    stack_non_terminal(NonTerminal::Identifier);
    record_snapshot("Stacked identifier list");

    expect(TokenType::Id, "identifier list without id");

    pop_symbol();
    record_snapshot("Popped identifier");

    while (true) {
        if (!match(TokenType::CommaOp)) {
            break;
        }
        stack_non_terminal(NonTerminal::Identifier);
        stack_terminal(TokenType::CommaOp);
        record_snapshot("Stacked identifier list");

        pop_symbol();
        record_snapshot("Popped ,");

        if (!expect(TokenType::Id,  "identifier list without id")) {
            break;
        }

        pop_symbol();
        record_snapshot("Popped identifier");
    }
}


void SyntacticAnalyzerProcedures::compound_command() {
    stack_terminal(TokenType::EndWord);
    stack_non_terminal(NonTerminal::Command);
    stack_terminal(TokenType::BeginWord);
    record_snapshot("Stacked compound_command");

    expect(TokenType::BeginWord, "Compound command without begin");

    pop_symbol();
    record_snapshot("Popped begin");

    pop_symbol();
    record_snapshot("Popped command");
    command();

    while (true) {
        if (!match(TokenType::SemiColonOp)) {
            break;
        }

        stack_non_terminal(NonTerminal::Command);
        stack_terminal(TokenType::SemiColonOp);
        record_snapshot("Stacked compound_command ext");

        pop_symbol();
        record_snapshot("Popped semicolon");

        pop_symbol();
        record_snapshot("Popped command");
        command();
    }

    expect(TokenType::EndWord, "Compound command without end");
    pop_symbol();
    record_snapshot("Popped end");
}

void SyntacticAnalyzerProcedures::command() {
    if (peek(TokenType::Id)) {
        match(TokenType::Id);

        if (peek(TokenType::OpenParOp)) {
            stack_non_terminal(NonTerminal::ProcedureCall);
            record_snapshot("Stacked procedure_call");

            pop_symbol();
            record_snapshot("Popped procedure_call");

            procedure_call();
            return;
        } else if(peek(TokenType::AssignOp) || peek(TokenType::AddOp) || peek(TokenType::SubOp)) {
            stack_non_terminal(NonTerminal::Assign);
            record_snapshot("Stacked assign");

            pop_symbol();
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

        pop_symbol();
        record_snapshot("Popped compound_command");
        compound_command();
        return;
    }

    if (peek(TokenType::IfWord)) {
        stack_non_terminal(NonTerminal::ConditionalCommand);
        record_snapshot("Stacked conditional_command_1");

        pop_symbol();
        record_snapshot("Popped conditional_command_1");

        conditional_command_1();
        return;
    }

    if (peek(TokenType::WhileWord)) {
        stack_non_terminal(NonTerminal::RepetitiveCommand);
        record_snapshot("Stacked repetitive_command_1");

        pop_symbol();
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

    pop_symbol();
    record_snapshot("Popped variable");
    variable();

    if (!expect(TokenType::AssignOp, "Malformed assing op, missing ':='")) {
        return;
    }
    pop_symbol();
    record_snapshot("Popped AssignOp");

    pop_symbol();
    record_snapshot("Popped expression");
    expression();
}

void SyntacticAnalyzerProcedures::variable() {
    stack_non_terminal(NonTerminal::Identifier);
    record_snapshot("Stacked variable identifier");

    pop_symbol();
    record_snapshot("Popped identifier");

    if (peek(TokenType::AddOp) || peek(TokenType::SubOp) || peek(TokenType::Num) ||
        peek(TokenType::OpenParOp) || peek(TokenType::NotWord) || peek(TokenType::Id)) {
        stack_non_terminal(NonTerminal::Expression);
        record_snapshot("Stacked variable expression");

        pop_symbol();
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

    pop_symbol();
    record_snapshot("Popped (");

    pop_symbol();
    record_snapshot("Popped expression_list");
    expression_list();

    expect(TokenType::CloseParOp, "Malformed procedure call, missing ')'");
    pop_symbol();
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

    pop_symbol();
    record_snapshot("Popped if");

    pop_symbol();
    record_snapshot("Popped expression");
    expression();

    if (expect(TokenType::ThenWord, "Malformed if command, missing then")) {
        return;
    }
    pop_symbol();
    record_snapshot("Popped then");

    pop_symbol();
    record_snapshot("Popped command");
    command();

    if (match(TokenType::ElseWord)) {
        stack_terminal(TokenType::ElseWord);
        record_snapshot("Stacked else");

        pop_symbol();
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

    pop_symbol();
    record_snapshot("Popped expression list");
    expression();

    while(true) {
        if (match(TokenType::CommaOp)) {
            stack_non_terminal(NonTerminal::Expression);
            stack_terminal(TokenType::CommaOp);
            record_snapshot("Stacked expression list");

            pop_symbol();
            record_snapshot("Popped ,");

            pop_symbol();
            record_snapshot("Popped expression");
            expression();
        }
    }
}

void SyntacticAnalyzerProcedures::expression() {
    stack_non_terminal(NonTerminal::SimpleExpression);
    record_snapshot("Stacked expression");

    pop_symbol();
    record_snapshot("Popped simple_expression");
    simple_expression();

    if (is_relation()) {
        stack_non_terminal(NonTerminal::SimpleExpression);
        stack_non_terminal(NonTerminal::Relation);
        record_snapshot("Stacked expression");

        pop_symbol();
        record_snapshot("Popped relation");
        relation();

        pop_symbol();
        record_snapshot("Popped simple_expression");
        simple_expression();
    }
}

void SyntacticAnalyzerProcedures::simple_expression() {
    stack_non_terminal(NonTerminal::Term);
    if (match(TokenType::AddOp)) {
        stack_terminal(TokenType::AddOp);
        record_snapshot("Stacked AddOp");

        pop_symbol();
        record_snapshot("Popped AddOp");

        return;
    } else if(match(TokenType::SubOp)) {
        stack_terminal(TokenType::SubOp);
        record_snapshot("Stacked SubOp");

        pop_symbol();
        record_snapshot("Popped SubOp");
    } else {
        return;
    }

    pop_symbol();
    record_snapshot("Popped term");
    term();

    while (true) {
        TokenType prev_type = lookahead->type;
        if(match(TokenType::AddOp) || match(TokenType::SubOp) || match(TokenType::DivWord)) {
            stack_non_terminal(NonTerminal::Term);
            if (prev_type == TokenType::AddOp) {
                stack_terminal(TokenType::AddOp);
                record_snapshot("Stacked simple_expression");

                pop_symbol();
                record_snapshot("Popped AddOp");
            } else if(prev_type == TokenType::SubOp) {
                stack_terminal(TokenType::SubOp);
                record_snapshot("Stacked simple_expression");

                pop_symbol();
                record_snapshot("Popped SubOp");
            } else if(prev_type == TokenType::DivWord) {
                stack_terminal(TokenType::DivWord);
                record_snapshot("Stacked simple_expression");

                pop_symbol();
                record_snapshot("Popped DivWord");
            }

            pop_symbol();
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

    pop_symbol();
    record_snapshot("Popped factor");
    factor();

    while (true) {
        TokenType prev_type = lookahead->type;
        if (match(TokenType::MulOp) || match(TokenType::DivWord) || match(TokenType::AndWord)) {
            stack_non_terminal(NonTerminal::Factor);
            if (prev_type == TokenType::MulOp) {
                stack_terminal(TokenType::MulOp);
                record_snapshot("Stacked MulOp");

                pop_symbol();
                record_snapshot("Popped MulOp");
            } else if(prev_type == TokenType::DivWord) {
                stack_terminal(TokenType::DivWord);
                record_snapshot("Stacked DivWord");

                pop_symbol();
                record_snapshot("Popped DivWord");
            } else if(prev_type == TokenType::AndWord) {
                stack_terminal(TokenType::AndWord);
                record_snapshot("Stacked AndWord");

                pop_symbol();
                record_snapshot("Popped AndWord");
            }

            pop_symbol();
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

        pop_symbol();
        record_snapshot("Popped OpenPar");

        pop_symbol();
        record_snapshot("Popped expression");
        expression();

        expect(TokenType::CloseParOp, "Malformed factor, missing ')'");

        pop_symbol();
        record_snapshot("Popped CloseParOp");
        return;
    } else if(match(TokenType::NotWord)) {
        stack_non_terminal(NonTerminal::Factor);
        stack_terminal(TokenType::NotWord);
        record_snapshot("Stacked factor");

        pop_symbol();
        record_snapshot("Popped NotWord");

        pop_symbol();
        record_snapshot("Popped factor");
        factor(); 
        return;
    } else if(match(TokenType::Id)) {
        stack_non_terminal(NonTerminal::Variable);
        record_snapshot("Stacked factor");

        pop_symbol();
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
    { NonTerminal::ProcedureDeclaration, "<procedure_declaration>" },
    { NonTerminal::FormalParameters, "<formal_parameters>" },
    { NonTerminal::FormalParametersSection, "<formal_parameters_section>" },
};

const std::unordered_map<TokenType, std::string> SyntacticAnalyzerProcedures::terminals = {
    { TokenType::CommaOp, "," },
    { TokenType::SemiColonOp, ";" },
    { TokenType::DotOp, "." },
    { TokenType::ProgramWord, "program" },
    { TokenType::ProcedureWord, "procedure" },
    { TokenType::VarWord, "var" },
    { TokenType::ColonOp, ":" },
    { TokenType::OpenParOp, "(" },
    { TokenType::CloseParOp, ")" },
};

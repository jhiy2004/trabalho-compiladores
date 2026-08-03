#include "analisador_sintatico_procedimento.h"
#include "compilador_lalg.h"
#include <iostream>

void SyntacticAnalyzerProcedures::run() {
    advance();
    program();
    analisador_semantico.verificar_variaveis_nao_utilizadas();
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

const TabelaSimbolos& SyntacticAnalyzerProcedures::get_tabela_simbolos() const {
    return tabela_simbolos;
}

std::vector<SemanticError> SyntacticAnalyzerProcedures::get_erros_semanticos() const {
    return analisador_semantico.get_erros();
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
    symbols.pop();
}

void SyntacticAnalyzerProcedures::program() {
    escopo_atual = "global";

    stack_terminal(TokenType::DotOp);
    stack_non_terminal(NonTerminal::Block);
    stack_terminal(TokenType::SemiColonOp);
    stack_non_terminal(NonTerminal::Identifier);
    stack_terminal(TokenType::ProgramWord);

    record_snapshot("Stacked program non-terminal");

    expect(TokenType::ProgramWord, "program word not found");
    pop_symbol();
    record_snapshot("Popped program");

    if (peek(TokenType::Id)) {
        std::string prog_name = lookahead->lexeme;
        unsigned int prog_line = lookahead->line;
        tabela_simbolos.inserir(SimboloEntry{
            .cadeia = prog_name,
            .token = "ProgramWord",
            .categoria = CategoriaSimbolo::Procedimento,
            .tipo = "",
            .valor = "",
            .escopo = "global",
            .utilizada = true,
            .linha = prog_line
        });
    }

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
    return (lookahead.value().type == type);
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
    return (peek(TokenType::IntWord) || peek(TokenType::BooleanWord) || peek(TokenType::RealWord));
}

std::string SyntacticAnalyzerProcedures::type() {
    if (match(TokenType::IntWord)) return "int";
    if (match(TokenType::RealWord)) return "real";
    if (match(TokenType::BooleanWord)) return "boolean";
    expect(TokenType::BooleanWord, "Expected a valid type");
    return "desconhecido";
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
    if (!is_type()) {
        return;
    }

    stack_non_terminal(NonTerminal::IdentifierList);
    stack_non_terminal(NonTerminal::Type);
    record_snapshot("Stacked variable_declaration");

    pop_symbol();
    record_snapshot("Popped <type>");
    std::string var_type = type();

    pop_symbol();
    record_snapshot("Popped <identifier_list>");
    std::vector<IdentInfo> ids = identifier_list();

    for (const auto& id : ids) {
        analisador_semantico.verificar_declaracao_duplicada(id.cadeia, escopo_atual, id.linha, id.col);
        tabela_simbolos.inserir(SimboloEntry{
            .cadeia = id.cadeia,
            .token = "Id",
            .categoria = CategoriaSimbolo::Variavel,
            .tipo = var_type,
            .valor = "",
            .escopo = escopo_atual,
            .utilizada = false,
            .linha = id.linha
        });
    }
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

    IdentInfo proc_info;
    if (lookahead.has_value()) {
        proc_info = { lookahead->lexeme, lookahead->line, lookahead->col };
    }

    if (!expect(TokenType::Id, "Procedure without identifier")) return;

    std::string prev_scope = escopo_atual;
    escopo_atual = proc_info.cadeia;

    analisador_semantico.verificar_declaracao_duplicada(proc_info.cadeia, "global", proc_info.linha, proc_info.col);

    std::vector<ParametroSimbolo> params;
    if (peek(TokenType::OpenParOp)) {
        stack_non_terminal(NonTerminal::FormalParameters);
        record_snapshot("Stacked <formal_parameters>");

        pop_symbol();
        record_snapshot("Popped <formal_parameters>");
        formal_parameters(params);
    }

    tabela_simbolos.inserir(SimboloEntry{
        .cadeia = proc_info.cadeia,
        .token = "ProcedureWord",
        .categoria = CategoriaSimbolo::Procedimento,
        .tipo = "",
        .valor = "",
        .escopo = "global",
        .utilizada = false,
        .linha = proc_info.linha,
        .parametros = params
    });

    stack_terminal(TokenType::SemiColonOp);
    record_snapshot("Stacked semicolon for procedure signature");

    pop_symbol();
    record_snapshot("Popped ;");

    if (!expect(TokenType::SemiColonOp, "Procedure signature without ';'")) {
        escopo_atual = prev_scope;
        return;
    }

    stack_non_terminal(NonTerminal::Block);
    record_snapshot("Stacked procedure <block>");

    pop_symbol();
    record_snapshot("Popped <block>");
    block();

    escopo_atual = prev_scope;
}

void SyntacticAnalyzerProcedures::formal_parameters(std::vector<ParametroSimbolo>& params) {
    if (!expect(TokenType::OpenParOp, "Expected '(' for formal parameters")) return;

    stack_non_terminal(NonTerminal::FormalParametersSection);
    record_snapshot("Stacked formal parameters section");

    pop_symbol();
    record_snapshot("Popped <formal_parameters_section>");
    formal_parameters_section(params);

    while (match(TokenType::SemiColonOp)) {
        stack_non_terminal(NonTerminal::FormalParametersSection);
        record_snapshot("Stacked formal parameters section loop");

        pop_symbol();
        record_snapshot("Popped <formal_parameters_section>");
        formal_parameters_section(params);
    }

    expect(TokenType::CloseParOp, "Expected ')' ending formal parameters");
}

void SyntacticAnalyzerProcedures::formal_parameters_section(std::vector<ParametroSimbolo>& params) {
    bool por_ref = false;
    if (match(TokenType::VarWord)) {
        por_ref = true;
        stack_terminal(TokenType::VarWord);
        record_snapshot("Stacked var keyword");

        pop_symbol();
        record_snapshot("Popped var keyword");
    }

    stack_non_terminal(NonTerminal::IdentifierList);
    record_snapshot("Stacked identifier list in parameters");

    pop_symbol();
    record_snapshot("Popped <identifier_list>");
    std::vector<IdentInfo> ids = identifier_list();

    if (!expect(TokenType::ColonOp, "Expected ':' in formal parameters section")) return;

    stack_non_terminal(NonTerminal::Type);
    record_snapshot("Stacked type identifier in parameters");

    pop_symbol();
    record_snapshot("Popped <type>");
    std::string param_type = type();

    for (const auto& id : ids) {
        params.push_back(ParametroSimbolo{
            .cadeia = id.cadeia,
            .tipo = param_type,
            .por_referencia = por_ref
        });

        analisador_semantico.verificar_declaracao_duplicada(id.cadeia, escopo_atual, id.linha, id.col);
        tabela_simbolos.inserir(SimboloEntry{
            .cadeia = id.cadeia,
            .token = "Id",
            .categoria = CategoriaSimbolo::Parametro,
            .tipo = param_type,
            .valor = "",
            .escopo = escopo_atual,
            .utilizada = false,
            .linha = id.linha
        });
    }
}

std::vector<IdentInfo> SyntacticAnalyzerProcedures::identifier_list() {
    std::vector<IdentInfo> ids;
    stack_non_terminal(NonTerminal::Identifier);
    record_snapshot("Stacked identifier list");

    if (lookahead.has_value() && lookahead->type == TokenType::Id) {
        ids.push_back({ lookahead->lexeme, lookahead->line, lookahead->col });
    }

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

        if (lookahead.has_value() && lookahead->type == TokenType::Id) {
            ids.push_back({ lookahead->lexeme, lookahead->line, lookahead->col });
        }

        if (!expect(TokenType::Id, "identifier list without id")) {
            break;
        }

        pop_symbol();
        record_snapshot("Popped identifier");
    }
    return ids;
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
    if (peek(TokenType::ReadWord)) {
        unsigned int line = lookahead->line;
        unsigned int col = lookahead->col;
        advance();
        expect(TokenType::OpenParOp, "Read command expects '('");
        std::vector<ArgExpr> args = expression_list();
        expect(TokenType::CloseParOp, "Read command expects ')'");
        analisador_semantico.verificar_read(args, escopo_atual, line, col);
        return;
    }

    if (peek(TokenType::WriteWord)) {
        unsigned int line = lookahead->line;
        unsigned int col = lookahead->col;
        advance();
        expect(TokenType::OpenParOp, "Write command expects '('");
        std::vector<ArgExpr> args = expression_list();
        expect(TokenType::CloseParOp, "Write command expects ')'");
        analisador_semantico.verificar_write(args, escopo_atual, line, col);
        return;
    }

    if (peek(TokenType::Id)) {
        IdentInfo id_info = { lookahead->lexeme, lookahead->line, lookahead->col };
        advance();

        if (peek(TokenType::OpenParOp)) {
            stack_non_terminal(NonTerminal::ProcedureCall);
            record_snapshot("Stacked procedure_call");

            pop_symbol();
            record_snapshot("Popped procedure_call");

            procedure_call(id_info);
            return;
        } else if (peek(TokenType::AssignOp) || peek(TokenType::AddOp) || peek(TokenType::SubOp)) {
            stack_non_terminal(NonTerminal::Assign);
            record_snapshot("Stacked assign");

            pop_symbol();
            record_snapshot("Popped assign");

            assign(id_info);
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

void SyntacticAnalyzerProcedures::assign(const IdentInfo& id_info) {
    stack_non_terminal(NonTerminal::Expression);
    stack_terminal(TokenType::AssignOp);
    stack_non_terminal(NonTerminal::Variable);
    record_snapshot("Stacked assign");

    pop_symbol();
    record_snapshot("Popped variable");

    SimboloEntry* var_entry = analisador_semantico.verificar_uso_identificador(id_info.cadeia, escopo_atual, id_info.linha, id_info.col);

    if (!expect(TokenType::AssignOp, "Malformed assign op, missing ':='")) {
        return;
    }
    pop_symbol();
    record_snapshot("Popped AssignOp");

    pop_symbol();
    record_snapshot("Popped expression");
    ArgExpr expr_val = expression();

    if (var_entry) {
        analisador_semantico.verificar_atribuicao(var_entry->tipo, expr_val.tipo, id_info.linha, id_info.col);
    }
}

ArgExpr SyntacticAnalyzerProcedures::variable() {
    stack_non_terminal(NonTerminal::Identifier);
    record_snapshot("Stacked variable identifier");

    pop_symbol();
    record_snapshot("Popped identifier");

    ArgExpr result;
    if (lookahead.has_value() && lookahead->type == TokenType::Id) {
        IdentInfo id_info = { lookahead->lexeme, lookahead->line, lookahead->col };
        SimboloEntry* s = analisador_semantico.verificar_uso_identificador(id_info.cadeia, escopo_atual, id_info.linha, id_info.col);
        result.tipo = s ? s->tipo : "desconhecido";
        result.cadeia = id_info.cadeia;
        result.eh_id = true;
        result.linha = id_info.linha;
        result.col = id_info.col;
    }

    if (peek(TokenType::AddOp) || peek(TokenType::SubOp) || peek(TokenType::Num) ||
        peek(TokenType::OpenParOp) || peek(TokenType::NotWord) || peek(TokenType::Id)) {
        stack_non_terminal(NonTerminal::Expression);
        record_snapshot("Stacked variable expression");

        pop_symbol();
        record_snapshot("Popped expression");
        expression();
    }
    return result;
}

void SyntacticAnalyzerProcedures::procedure_call(const IdentInfo& proc_info) {
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
    std::vector<ArgExpr> args = expression_list();

    expect(TokenType::CloseParOp, "Malformed procedure call, missing ')'");
    pop_symbol();
    record_snapshot("Popped )");

    analisador_semantico.verificar_chamada_procedimento(proc_info.cadeia, args, escopo_atual, proc_info.linha, proc_info.col);
}

void SyntacticAnalyzerProcedures::conditional_command_1() {
    stack_non_terminal(NonTerminal::Command);
    stack_terminal(TokenType::ThenWord);
    stack_non_terminal(NonTerminal::Expression);
    stack_terminal(TokenType::IfWord);
    record_snapshot("Stacked conditional_command_1");

    if (!expect(TokenType::IfWord, "Malformed if command, missing if")) {
        return;
    }

    pop_symbol();
    record_snapshot("Popped if");

    pop_symbol();
    record_snapshot("Popped expression");
    expression();

    if (!expect(TokenType::ThenWord, "Malformed if command, missing then")) {
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
    if (!expect(TokenType::WhileWord, "Malformed repetitive_command_1, missing while")) return;
    expression();
    if (!expect(TokenType::DoWord, "Malformed repetitive_command_1, missing do")) return;
    command();
}

std::vector<ArgExpr> SyntacticAnalyzerProcedures::expression_list() {
    std::vector<ArgExpr> list;
    stack_non_terminal(NonTerminal::Expression);
    record_snapshot("Stacked expression list");

    pop_symbol();
    record_snapshot("Popped expression list");
    list.push_back(expression());

    while (true) {
        if (!match(TokenType::CommaOp)) {
            break;
        }

        stack_non_terminal(NonTerminal::Expression);
        stack_terminal(TokenType::CommaOp);
        record_snapshot("Stacked expression list");

        pop_symbol();
        record_snapshot("Popped ,");

        pop_symbol();
        record_snapshot("Popped expression");
        list.push_back(expression());
    }
    return list;
}

ArgExpr SyntacticAnalyzerProcedures::expression() {
    stack_non_terminal(NonTerminal::SimpleExpression);
    record_snapshot("Stacked expression");

    pop_symbol();
    record_snapshot("Popped simple_expression");
    ArgExpr left = simple_expression();

    if (is_relation()) {
        stack_non_terminal(NonTerminal::SimpleExpression);
        stack_non_terminal(NonTerminal::Relation);
        record_snapshot("Stacked expression");

        pop_symbol();
        record_snapshot("Popped relation");
        relation();

        pop_symbol();
        record_snapshot("Popped simple_expression");
        ArgExpr right = simple_expression();

        return ArgExpr{ .tipo = "boolean", .cadeia = "", .eh_id = false, .linha = left.linha, .col = left.col };
    }
    return left;
}

ArgExpr SyntacticAnalyzerProcedures::simple_expression() {
    stack_non_terminal(NonTerminal::Term);
    bool has_sign = false;
    if (match(TokenType::AddOp)) {
        has_sign = true;
        stack_terminal(TokenType::AddOp);
        record_snapshot("Stacked AddOp");

        pop_symbol();
        record_snapshot("Popped AddOp");
    } else if (match(TokenType::SubOp)) {
        has_sign = true;
        stack_terminal(TokenType::SubOp);
        record_snapshot("Stacked SubOp");

        pop_symbol();
        record_snapshot("Popped SubOp");
    }

    pop_symbol();
    record_snapshot("Popped term");
    ArgExpr current = term();

    while (true) {
        if (!lookahead.has_value()) break;
        TokenType prev_type = lookahead->type;
        if (match(TokenType::AddOp) || match(TokenType::SubOp) || match(TokenType::OrWord)) {
            stack_non_terminal(NonTerminal::Term);
            if (prev_type == TokenType::AddOp) {
                stack_terminal(TokenType::AddOp);
                record_snapshot("Stacked simple_expression");

                pop_symbol();
                record_snapshot("Popped AddOp");
            } else if (prev_type == TokenType::SubOp) {
                stack_terminal(TokenType::SubOp);
                record_snapshot("Stacked simple_expression");

                pop_symbol();
                record_snapshot("Popped SubOp");
            } else if (prev_type == TokenType::OrWord) {
                stack_terminal(TokenType::OrWord);
                record_snapshot("Stacked OrWord");

                pop_symbol();
                record_snapshot("Popped OrWord");
            }

            pop_symbol();
            record_snapshot("Popped term");
            ArgExpr next_term = term();

            if (prev_type == TokenType::OrWord) {
                current = ArgExpr{ .tipo = "boolean", .cadeia = "", .eh_id = false, .linha = current.linha, .col = current.col };
            } else {
                if (current.tipo == "real" || next_term.tipo == "real") {
                    current.tipo = "real";
                } else if (current.tipo == "int" && next_term.tipo == "int") {
                    current.tipo = "int";
                }
                current.eh_id = false;
            }
        } else {
            break;
        }
    }
    return current;
}

ArgExpr SyntacticAnalyzerProcedures::term() {
    stack_non_terminal(NonTerminal::Factor);
    record_snapshot("Stacked term");

    pop_symbol();
    record_snapshot("Popped factor");
    ArgExpr current = factor();

    while (true) {
        if (!lookahead.has_value()) break;
        TokenType prev_type = lookahead->type;
        unsigned int op_line = lookahead->line;
        unsigned int op_col = lookahead->col;

        if (match(TokenType::MulOp) || match(TokenType::DivWord) || match(TokenType::AndWord)) {
            stack_non_terminal(NonTerminal::Factor);
            if (prev_type == TokenType::MulOp) {
                stack_terminal(TokenType::MulOp);
                record_snapshot("Stacked MulOp");

                pop_symbol();
                record_snapshot("Popped MulOp");
            } else if (prev_type == TokenType::DivWord) {
                stack_terminal(TokenType::DivWord);
                record_snapshot("Stacked DivWord");

                pop_symbol();
                record_snapshot("Popped DivWord");
            } else if (prev_type == TokenType::AndWord) {
                stack_terminal(TokenType::AndWord);
                record_snapshot("Stacked AndWord");

                pop_symbol();
                record_snapshot("Popped AndWord");
            }

            pop_symbol();
            record_snapshot("Popped Factor");
            ArgExpr next_factor = factor();

            if (prev_type == TokenType::DivWord) {
                current.tipo = analisador_semantico.verificar_operacao_divisao(current.tipo, next_factor.tipo, op_line, op_col);
                current.eh_id = false;
            } else if (prev_type == TokenType::AndWord) {
                current = ArgExpr{ .tipo = "boolean", .cadeia = "", .eh_id = false, .linha = current.linha, .col = current.col };
            } else if (prev_type == TokenType::MulOp) {
                if (current.tipo == "real" || next_factor.tipo == "real") {
                    current.tipo = "real";
                } else if (current.tipo == "int" && next_factor.tipo == "int") {
                    current.tipo = "int";
                }
                current.eh_id = false;
            }
        } else {
            break;
        }
    }
    return current;
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

ArgExpr SyntacticAnalyzerProcedures::factor() {
    if (lookahead.has_value() && lookahead->type == TokenType::Num) {
        ArgExpr res;
        res.cadeia = lookahead->lexeme;
        res.linha = lookahead->line;
        res.col = lookahead->col;
        res.eh_id = false;
        res.tipo = (res.cadeia.find('.') != std::string::npos) ? "real" : "int";

        match(TokenType::Num);
        stack_terminal(TokenType::Num);
        record_snapshot("Stacked factor");

        pop_symbol();
        record_snapshot("Popped number");
        return res;
    } else if (peek(TokenType::OpenParOp)) {
        match(TokenType::OpenParOp);
        stack_terminal(TokenType::CloseParOp);
        stack_non_terminal(NonTerminal::Expression);
        stack_terminal(TokenType::OpenParOp);
        record_snapshot("Stacked factor");

        pop_symbol();
        record_snapshot("Popped OpenPar");

        pop_symbol();
        record_snapshot("Popped expression");
        ArgExpr expr_res = expression();

        expect(TokenType::CloseParOp, "Malformed factor, missing ')'");

        pop_symbol();
        record_snapshot("Popped CloseParOp");
        expr_res.eh_id = false;
        return expr_res;
    } else if (match(TokenType::NotWord)) {
        stack_non_terminal(NonTerminal::Factor);
        stack_terminal(TokenType::NotWord);
        record_snapshot("Stacked factor");

        pop_symbol();
        record_snapshot("Popped NotWord");

        pop_symbol();
        record_snapshot("Popped factor");
        factor();
        return ArgExpr{ .tipo = "boolean", .cadeia = "", .eh_id = false };
    } else if (lookahead.has_value() && (lookahead->type == TokenType::Id || lookahead->type == TokenType::FalseWord || lookahead->type == TokenType::TrueWord)) {
        ArgExpr res;
        res.linha = lookahead->line;
        res.col = lookahead->col;
        res.cadeia = lookahead->lexeme;

        if (lookahead->type == TokenType::TrueWord || lookahead->type == TokenType::FalseWord) {
            res.tipo = "boolean";
            res.eh_id = false;
            match(lookahead->type);
        } else {
            res.eh_id = true;
            IdentInfo id_info = { lookahead->lexeme, lookahead->line, lookahead->col };
            match(TokenType::Id);

            SimboloEntry* s = analisador_semantico.verificar_uso_identificador(id_info.cadeia, escopo_atual, id_info.linha, id_info.col);
            res.tipo = s ? s->tipo : "desconhecido";
        }

        stack_non_terminal(NonTerminal::Variable);
        record_snapshot("Stacked factor");

        pop_symbol();
        record_snapshot("Popped variable");
        return res;
    }

    enqueue_error("Malformed factor");
    return ArgExpr{ .tipo = "desconhecido", .eh_id = false };
}

void SyntacticAnalyzerProcedures::advance() {
    lookahead = _lexical.get_token();
}

void SyntacticAnalyzerProcedures::print_current_lexeme() {
    if (!lookahead.has_value()) {
        std::cout << "All tokens consumed!" << std::endl;
        return;
    }
    std::cout << "lookahead: " << lookahead->lexeme << std::endl;
    std::cout << "type: " << terminals.find(lookahead->type)->second << std::endl;
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
    { NonTerminal::Command, "<command>" },
    { NonTerminal::ProcedureCall, "<procedure_call>" },
    { NonTerminal::ConditionalCommand, "<conditional_command>" },
    { NonTerminal::RepetitiveCommand, "<repetitive_command>" },
    { NonTerminal::Expression, "<expression>" },
    { NonTerminal::Variable, "<variable>" },
    { NonTerminal::ExpressionList, "<expression_list>" },
    { NonTerminal::SimpleExpression, "<simple_expression>" },
    { NonTerminal::Relation, "<relation>" },
    { NonTerminal::Term, "<term>" },
    { NonTerminal::Factor, "<factor>" },
    { NonTerminal::Assign, "<assign>"},
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
    { TokenType::BeginWord, "begin" },
    { TokenType::EndWord, "end" },
    { TokenType::AssignOp, ":=" },
    { TokenType::ThenWord, "then" },
    { TokenType::IfWord, "if" },
    { TokenType::ElseWord, "else" },
    { TokenType::AddOp, "+" },
    { TokenType::SubOp, "-" },
    { TokenType::DivWord, "div" },
    { TokenType::MulOp, "*" },
    { TokenType::AndWord, "and" },
    { TokenType::NotWord, "not" },
    { TokenType::Num, "<number>" },
    { TokenType::TrueWord, "true" },
    { TokenType::FalseWord, "false" },
    { TokenType::Id, "identifier" },
    { TokenType::ReadWord, "read" },
    { TokenType::WriteWord, "write" },
};

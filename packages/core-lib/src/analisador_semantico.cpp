#include "analisador_semantico.h"
#include <sstream>

void AnalisadorSemantico::adicionar_erro(const std::string& mensagem, unsigned int linha, unsigned int col, TipoErroSemantico tipo) {
    erros_.push_back(SemanticError{
        .mensagem = mensagem,
        .linha = linha,
        .col = col,
        .tipo_erro = tipo
    });
}

SimboloEntry* AnalisadorSemantico::verificar_uso_identificador(const std::string& cadeia, const std::string& escopo_atual, unsigned int linha, unsigned int col) {
    SimboloEntry* entry = tabela_.buscar(cadeia, escopo_atual);
    if (!entry) {
        // Verificar se existe em algum outro escopo local para dar erro de escopo específico
        for (const auto& s : tabela_.get_entradas()) {
            if (s.cadeia == cadeia && s.escopo != escopo_atual && s.escopo != "global") {
                std::ostringstream ss;
                ss << "Uso de variável de escopo inadequado: '" << cadeia 
                   << "' é local ao procedimento '" << s.escopo 
                   << "' e não pode ser utilizada em '" << escopo_atual << "'";
                adicionar_erro(ss.str(), linha, col, TipoErroSemantico::EscopoInadequado);
                return nullptr;
            }
        }

        std::ostringstream ss;
        ss << "Variável ou procedimento não declarado: '" << cadeia << "'";
        adicionar_erro(ss.str(), linha, col, TipoErroSemantico::NaoDeclarado);
        return nullptr;
    }

    entry->utilizada = true;
    return entry;
}

bool AnalisadorSemantico::verificar_declaracao_duplicada(const std::string& cadeia, const std::string& escopo_atual, unsigned int linha, unsigned int col) {
    SimboloEntry* existente = tabela_.buscar_no_escopo(cadeia, escopo_atual);
    if (existente) {
        std::ostringstream ss;
        ss << "Variável ou procedimento declarado mais de uma vez: '" << cadeia 
           << "' já foi declarado no escopo '" << escopo_atual << "'";
        adicionar_erro(ss.str(), linha, col, TipoErroSemantico::DeclaracaoDuplicada);
        return true;
    }
    return false;
}

void AnalisadorSemantico::verificar_chamada_procedimento(const std::string& nome_proc, const std::vector<ArgExpr>& args, const std::string& escopo_atual, unsigned int linha, unsigned int col) {
    SimboloEntry* proc = tabela_.buscar_procedimento(nome_proc);
    if (!proc) {
        std::ostringstream ss;
        ss << "Procedimento não declarado: '" << nome_proc << "'";
        adicionar_erro(ss.str(), linha, col, TipoErroSemantico::NaoDeclarado);
        return;
    }

    proc->utilizada = true;

    if (args.size() != proc->parametros.size()) {
        std::ostringstream ss;
        ss << "Incompatibilidade de parâmetros formais e reais: procedimento '" << nome_proc 
           << "' espera " << proc->parametros.size() << " argumento(s), mas recebeu " << args.size();
        adicionar_erro(ss.str(), linha, col, TipoErroSemantico::IncompatibilidadeParametros);
        return;
    }

    for (size_t i = 0; i < args.size(); ++i) {
        const auto& param = proc->parametros[i];
        const auto& arg = args[i];

        if (param.por_referencia && !arg.eh_id) {
            std::ostringstream ss;
            ss << "Incompatibilidade de parâmetros formais e reais: parâmetro por referência '" 
               << param.cadeia << "' do procedimento '" << nome_proc << "' exige uma variável";
            adicionar_erro(ss.str(), arg.linha > 0 ? arg.linha : linha, arg.col > 0 ? arg.col : col, TipoErroSemantico::IncompatibilidadeParametros);
        }

        if (arg.tipo != "desconhecido" && arg.tipo != param.tipo) {
            // Permitir conversão automática int -> real para valor por valor se der, mas reportar erro de tipo de parâmetro
            if (!(param.tipo == "real" && arg.tipo == "int" && !param.por_referencia)) {
                std::ostringstream ss;
                ss << "Incompatibilidade de parâmetros formais e reais: ordem/tipo incorreto na posição " 
                   << (i + 1) << " do procedimento '" << nome_proc 
                   << "' (esperado '" << param.tipo << "', recebido '" << arg.tipo << "')";
                adicionar_erro(ss.str(), arg.linha > 0 ? arg.linha : linha, arg.col > 0 ? arg.col : col, TipoErroSemantico::IncompatibilidadeParametros);
            }
        }
    }
}

void AnalisadorSemantico::verificar_atribuicao(const std::string& tipo_var, const std::string& tipo_expr, unsigned int linha, unsigned int col) {
    if (tipo_var == "int" && tipo_expr == "real") {
        adicionar_erro("Atribuição de um real a um inteiro", linha, col, TipoErroSemantico::AtribuicaoRealParaInteiro);
    } else if (tipo_var != "desconhecido" && tipo_expr != "desconhecido" && tipo_var != tipo_expr) {
        if (!(tipo_var == "real" && tipo_expr == "int")) {
            std::ostringstream ss;
            ss << "Tipos incompatíveis na atribuição: tentando atribuir '" << tipo_expr << "' para '" << tipo_var << "'";
            adicionar_erro(ss.str(), linha, col, TipoErroSemantico::AtribuicaoRealParaInteiro);
        }
    }
}

std::string AnalisadorSemantico::verificar_operacao_divisao(const std::string& tipo_esq, const std::string& tipo_dir, unsigned int linha, unsigned int col) {
    if (tipo_esq != "int" || tipo_dir != "int") {
        adicionar_erro("Divisão que não é entre números inteiros (operador 'div' exige operandos do tipo 'int')", linha, col, TipoErroSemantico::DivisaoNaoInteira);
    }
    return "int";
}

void AnalisadorSemantico::verificar_variaveis_nao_utilizadas() {
    for (const auto& s : tabela_.get_entradas()) {
        if ((s.categoria == CategoriaSimbolo::Variavel || s.categoria == CategoriaSimbolo::Parametro) && !s.utilizada) {
            std::ostringstream ss;
            ss << "Variável declarada e nunca utilizada: '" << s.cadeia << "' no escopo '" << s.escopo << "'";
            adicionar_erro(ss.str(), s.linha, 0, TipoErroSemantico::VariavelNaoUtilizada);
        }
    }
}

void AnalisadorSemantico::verificar_read(const std::vector<ArgExpr>& vars, const std::string& escopo_atual, unsigned int linha, unsigned int col) {
    if (vars.empty()) return;
    std::string primeiro_tipo = "";
    for (const auto& v : vars) {
        if (!v.eh_id) {
            adicionar_erro("Comando 'read' exige uma variável como parâmetro", v.linha > 0 ? v.linha : linha, v.col > 0 ? v.col : col, TipoErroSemantico::ReadWriteTipoIncompativel);
            continue;
        }
        if (primeiro_tipo.empty()) {
            primeiro_tipo = v.tipo;
        } else if (v.tipo != "desconhecido" && v.tipo != primeiro_tipo) {
            std::ostringstream ss;
            ss << "Read com variáveis de tipo diferentes: '" << primeiro_tipo << "' e '" << v.tipo << "'";
            adicionar_erro(ss.str(), v.linha > 0 ? v.linha : linha, v.col > 0 ? v.col : col, TipoErroSemantico::ReadWriteTipoIncompativel);
        }
    }
}

void AnalisadorSemantico::verificar_write(const std::vector<ArgExpr>& exprs, const std::string& escopo_atual, unsigned int linha, unsigned int col) {
    if (exprs.empty()) return;
    std::string primeiro_tipo = "";
    for (const auto& e : exprs) {
        if (primeiro_tipo.empty()) {
            primeiro_tipo = e.tipo;
        } else if (e.tipo != "desconhecido" && e.tipo != primeiro_tipo) {
            std::ostringstream ss;
            ss << "Write com expressões de tipo diferentes: '" << primeiro_tipo << "' e '" << e.tipo << "'";
            adicionar_erro(ss.str(), e.linha > 0 ? e.linha : linha, e.col > 0 ? e.col : col, TipoErroSemantico::ReadWriteTipoIncompativel);
        }
    }
}

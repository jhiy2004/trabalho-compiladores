#pragma once

#include "tabela_simbolos.h"
#include <string>
#include <vector>

enum class TipoErroSemantico {
    NaoDeclarado,
    DeclaracaoDuplicada,
    IncompatibilidadeParametros,
    EscopoInadequado,
    AtribuicaoRealParaInteiro,
    DivisaoNaoInteira,
    VariavelNaoUtilizada,
    ReadWriteTipoIncompativel,
    ErroEscopo
};

struct SemanticError {
    std::string mensagem;
    unsigned int linha{0};
    unsigned int col{0};
    TipoErroSemantico tipo_erro;
};

struct ArgExpr {
    std::string tipo;
    std::string cadeia;
    bool eh_id{false};
    unsigned int linha{0};
    unsigned int col{0};
};

class AnalisadorSemantico {
public:
    AnalisadorSemantico(TabelaSimbolos& tabela) : tabela_(tabela) {}

    void adicionar_erro(const std::string& mensagem, unsigned int linha, unsigned int col, TipoErroSemantico tipo);

    SimboloEntry* verificar_uso_identificador(const std::string& cadeia, const std::string& escopo_atual, unsigned int linha, unsigned int col);

    bool verificar_declaracao_duplicada(const std::string& cadeia, const std::string& escopo_atual, unsigned int linha, unsigned int col);

    void verificar_chamada_procedimento(const std::string& nome_proc, const std::vector<ArgExpr>& args, const std::string& escopo_atual, unsigned int linha, unsigned int col);

    void verificar_atribuicao(const std::string& tipo_var, const std::string& tipo_expr, unsigned int linha, unsigned int col);

    std::string verificar_operacao_divisao(const std::string& tipo_esq, const std::string& tipo_dir, unsigned int linha, unsigned int col);

    void verificar_variaveis_nao_utilizadas();

    void verificar_read(const std::vector<ArgExpr>& vars, const std::string& escopo_atual, unsigned int linha, unsigned int col);
    void verificar_write(const std::vector<ArgExpr>& exprs, const std::string& escopo_atual, unsigned int linha, unsigned int col);

    const std::vector<SemanticError>& get_erros() const { return erros_; }
    void clear() { erros_.clear(); }

private:
    TabelaSimbolos& tabela_;
    std::vector<SemanticError> erros_;
};

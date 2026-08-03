#pragma once

#include "compilador_lalg.h"
#include <string>
#include <vector>
#include <optional>

enum class CategoriaSimbolo {
    Variavel,
    Procedimento,
    Parametro
};

struct ParametroSimbolo {
    std::string cadeia;
    std::string tipo;
    bool por_referencia;
};

struct SimboloEntry {
    std::string cadeia;
    std::string token;
    CategoriaSimbolo categoria;
    std::string tipo;
    std::string valor;
    std::string escopo;
    bool utilizada{false};
    unsigned int linha{0};

    std::vector<ParametroSimbolo> parametros;

    std::string categoria_str() const {
        switch (categoria) {
            case CategoriaSimbolo::Variavel: return "Variavel";
            case CategoriaSimbolo::Procedimento: return "Procedimento";
            case CategoriaSimbolo::Parametro: return "Parametro";
            default: return "Desconhecido";
        }
    }
};

class TabelaSimbolos {
public:
    TabelaSimbolos() = default;

    bool inserir(const SimboloEntry& entry);

    SimboloEntry* buscar(const std::string& cadeia, const std::string& escopo_atual);
    SimboloEntry* buscar_procedimento(const std::string& cadeia);
    SimboloEntry* buscar_no_escopo(const std::string& cadeia, const std::string& escopo);

    void marcar_utilizada(const std::string& cadeia, const std::string& escopo_atual);

    const std::vector<SimboloEntry>& get_entradas() const { return entradas_; }
    std::vector<SimboloEntry>& get_entradas_mutable() { return entradas_; }

    void clear() { entradas_.clear(); }

private:
    std::vector<SimboloEntry> entradas_;
};

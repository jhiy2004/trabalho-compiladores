#include "tabela_simbolos.h"
#include <algorithm>

bool TabelaSimbolos::inserir(const SimboloEntry& entry) {
    for (auto& s : entradas_) {
        if (s.cadeia == entry.cadeia && s.escopo == entry.escopo) {
            return false;
        }
    }
    entradas_.push_back(entry);
    return true;
}

SimboloEntry* TabelaSimbolos::buscar_no_escopo(const std::string& cadeia, const std::string& escopo) {
    for (auto& s : entradas_) {
        if (s.cadeia == cadeia && s.escopo == escopo) {
            return &s;
        }
    }
    return nullptr;
}

SimboloEntry* TabelaSimbolos::buscar(const std::string& cadeia, const std::string& escopo_atual) {
    if (escopo_atual != "global") {
        SimboloEntry* s = buscar_no_escopo(cadeia, escopo_atual);
        if (s) return s;
    }

    SimboloEntry* s_global = buscar_no_escopo(cadeia, "global");
    if (s_global) return s_global;

    return nullptr;
}

SimboloEntry* TabelaSimbolos::buscar_procedimento(const std::string& cadeia) {
    for (auto& s : entradas_) {
        if (s.cadeia == cadeia && s.categoria == CategoriaSimbolo::Procedimento) {
            return &s;
        }
    }
    return nullptr;
}

void TabelaSimbolos::marcar_utilizada(const std::string& cadeia, const std::string& escopo_atual) {
    SimboloEntry* s = buscar(cadeia, escopo_atual);
    if (s) {
        s->utilizada = true;
    }
}

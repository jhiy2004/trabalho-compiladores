#include "analisador_sintatico.h"
#include <memory>

Terminal::Terminal() {}
Terminal::Terminal(const std::string& n, TokenType t) : nome(n), token(t) {}
Terminal::~Terminal() {}

NaoTerminal::NaoTerminal() {}
NaoTerminal::NaoTerminal(const std::string& n, bool s) : nome(n), start(s) {};
NaoTerminal::~NaoTerminal() {}

bool NaoTerminal::has_elem(std::shared_ptr<Elemento> elem) const {
    for (const Regra& regra : producoes) {
        if (regra.has_elem(elem)) {
            return true;
        }
    }

    return false;
}

bool NaoTerminal::isStart() const{
    return start;
}

std::optional<std::shared_ptr<NaoTerminal>> Regra::get_next_non_terminal(std::shared_ptr<Elemento> elem) const {
    int res = -1;
    for (int i=0; i < elementos.size(); i++) {
        if (elementos[i] == elem) {
            res = i; 
            break;
        }
    }

    if (res >= 0 && res+1 < elementos.size()){
        auto next = elementos[res + 1];
        if (auto next_nt = std::dynamic_pointer_cast<NaoTerminal>(next); next_nt != nullptr){
            return next_nt;
        }
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<Terminal>> Regra::get_next_terminal(std::shared_ptr<Elemento> elem) const {
    int res = -1;
    for (int i=0; i < elementos.size(); i++) {
        if (elementos[i] == elem) {
            res = i; 
            break;
        }
    }

    if (res >= 0 && res+1 < elementos.size()){
        auto next = elementos[res + 1];
        if (auto next_t = std::dynamic_pointer_cast<Terminal>(next); next_t != nullptr){
            return next_t;
        }
    }
    return std::nullopt;
}


bool Regra::has_elem(std::shared_ptr<Elemento> elem) const {
    for (const auto e : elementos) {
        if (elem == e) {
            return true;
        }
    }
    return false;
}

std::unordered_map<std::string, std::shared_ptr<Terminal>> GrammarLALG::terminals = {
    {"if", std::make_shared<Terminal>("if", TokenType::IfWord)},
    {"id", std::make_shared<Terminal>("id", TokenType::Id)},
    {"$", std::make_shared<Terminal>("$", TokenType::UNK)},
    {"epsilon", std::make_shared<Terminal>("epsilon", TokenType::UNK)},
};

std::unordered_map<std::string, std::shared_ptr<NaoTerminal>> GrammarLALG::non_terminals = {
    {"S", std::make_shared<NaoTerminal>("S", true)},
    {"A", std::make_shared<NaoTerminal>("A")},
    {"B", std::make_shared<NaoTerminal>("B")},
};

GrammarLALG::GrammarLALG() {
    non_terminals["S"]->add_regra(Regra({ non_terminals["A"], terminals["if"]}));
    non_terminals["S"]->add_regra(Regra({ terminals["id"] }));

    non_terminals["A"]->add_regra(Regra({ non_terminals["B"], terminals["if"]}));
    non_terminals["A"]->add_regra(Regra({ terminals["if"] }));
}

bool GrammarLALG::compile() {
    //Primeiro devem ser encontrados todos os firsts de todos os nao-terminais
    for (const auto& pair : non_terminals) {
        auto nt_ptr = pair.second;
        first(nt_ptr);
    }

    //Tendo todos os first é possível encontrar todos os follows
    for (const auto& pair : non_terminals) {
        auto nt_ptr = pair.second;
        follow(nt_ptr);
    }

    return false;
}

void GrammarLALG::first(std::shared_ptr<NaoTerminal> ptr) {
    return; 
}

void GrammarLALG::follow(std::shared_ptr<NaoTerminal> ptr) {
    std::unordered_set<std::shared_ptr<Elemento>> follows;
    std::unordered_set<std::shared_ptr<NaoTerminal>> to_analyze;

    if (ptr->isStart()) {
        follows.insert(terminals["$"]);
    }

    for (const auto& pair : non_terminals) {
        auto nt_ptr = pair.second;

        auto regras = nt_ptr->get_regras();
        // essas funcs get assumem que tera apenas um NaoTerminal de um tipo por regra.
        // Aceita. Exemplo: S->Aa
        // Nao Aceita. Exemplo: S->AbAa

        for (const Regra& r : regras) {
            auto next_nt = r.get_next_non_terminal(ptr);
            if (next_nt.has_value()) {
                to_analyze.insert(next_nt.value());
            } else if (r.has_elem(ptr)){
                to_analyze.insert(nt_ptr);
            }

            auto next_t = r.get_next_terminal(ptr);
            if (next_t.has_value()) {
                follows.insert(next_t.value());
            }
        }
    }

    for (const auto elem : to_analyze) {
        if (elem->has_elem(terminals["epsilon"])) {
            for (const auto elem : elem->get_follows()) {
                follows.insert(elem);
            }
        } else {
            for (const auto elem : elem->get_firsts()) {
                follows.insert(elem);
            }
        }
    }
}

std::unordered_set<std::shared_ptr<Elemento>> GrammarLALG::head(std::shared_ptr<NaoTerminal> ptr) {
    return ptr->get_head();
}

std::unordered_set<std::shared_ptr<Elemento>> GrammarLALG::last(std::shared_ptr<NaoTerminal> ptr) {
    return ptr->get_last();
}

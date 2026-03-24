#pragma once

#include "compilador_lalg.h"
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>
#include <memory>

class Elemento {
public:
    virtual std::string get_nome() const = 0;
    virtual ~Elemento() = default;
};

class NaoTerminal;
class Terminal;

class Regra {
public:
    Regra(const std::vector<std::shared_ptr<Elemento>>& elems) : elementos(elems) {}
    
    std::optional<std::shared_ptr<NaoTerminal>> get_next_non_terminal(std::shared_ptr<Elemento> elem) const;
    std::optional<std::shared_ptr<Terminal>> get_next_terminal(std::shared_ptr<Elemento> elem) const;

    bool has_elem(std::shared_ptr<Elemento> elem) const;

    std::shared_ptr<Elemento> get_head() const {
        return elementos.front();
    }

    std::shared_ptr<Elemento> get_tail() const {
        return elementos.back();
    }

private:
    std::vector<std::shared_ptr<Elemento>> elementos;
};


class Terminal : public Elemento {
public:
    Terminal();
    Terminal(const std::string& n, TokenType t);
    ~Terminal();

    virtual std::string get_nome() const override{
        return nome;
    };
private:
    TokenType token;
    std::string nome;
};

class NaoTerminal : public Elemento {
public:
    std::vector<Regra> producoes;

    NaoTerminal();
    NaoTerminal(const std::string& n, bool s = false);
    ~NaoTerminal();

    std::unordered_set<std::shared_ptr<Elemento>> get_head() const;

    std::unordered_set<std::shared_ptr<Elemento>> get_last() const {
        std::unordered_set<std::shared_ptr<Elemento>> res;
        for (const auto r : producoes) {
            res.insert(r.get_tail());
        }

        return res;
    }

    std::unordered_set<std::shared_ptr<Elemento>> get_firsts() const {
        return firsts;
    }

    std::unordered_set<std::shared_ptr<Elemento>> get_follows() const {
        return follows;
    }

    bool has_elem(const std::shared_ptr<Elemento> elem) const;

    void add_regra(const Regra& r) {
        producoes.push_back(r);
    }

    std::vector<Regra> get_regras() const {
        return producoes;
    }

    bool isStart() const;

    virtual std::string get_nome() const override{
        return nome;
    };
private:
    bool start;
    std::string nome;
    std::unordered_set<std::shared_ptr<Elemento>> firsts;
    std::unordered_set<std::shared_ptr<Elemento>> follows;
};


class GrammarLALG {
public:
    GrammarLALG();
    bool compile();
private:
    void first(std::shared_ptr<NaoTerminal> ptr);
    void follow(std::shared_ptr<NaoTerminal> ptr);
    std::unordered_set<std::shared_ptr<Elemento>> head(std::shared_ptr<NaoTerminal> ptr);
    std::unordered_set<std::shared_ptr<Elemento>> last(std::shared_ptr<NaoTerminal> ptr);

    static std::unordered_map<std::string, std::shared_ptr<Terminal>> terminals;
    static std::unordered_map<std::string, std::shared_ptr<NaoTerminal>> non_terminals;
};

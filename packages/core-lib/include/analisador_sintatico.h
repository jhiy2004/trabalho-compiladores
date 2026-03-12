#pragma once

#include <string>
#include <vector>
#include <memory>

class Elemento {
public:
    std::string nome;

    Elemento();
    Elemento(const std::string& n);
    virtual ~Elemento();
};

class Terminal : public Elemento {
public:
    int token;

    Terminal();
    Terminal(const std::string& n, int t);
    ~Terminal();
};

class NaoTerminal : public Elemento {
public:
    std::vector<std::vector<std::shared_ptr<Elemento>>> producoes;

    NaoTerminal();
    NaoTerminal(const std::string& n);
    ~NaoTerminal();
};
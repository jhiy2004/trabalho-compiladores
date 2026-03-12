#include "analisador_sintatico.h"

Elemento::Elemento() {}

Elemento::Elemento(const std::string& n) : nome(n) {}

Elemento::~Elemento() {}

Terminal::Terminal() {}

Terminal::Terminal(const std::string& n, int t) : Elemento(n), token(t) {}

Terminal::~Terminal() {}

NaoTerminal::NaoTerminal() {}

NaoTerminal::NaoTerminal(const std::string& n) : Elemento(n) {}

NaoTerminal::~NaoTerminal() {}
#include "compilador.h"
#include <iostream>

int main() {
    LexicalAnalysis analise("(1.2+23) / 123.41111 * 0");
    bool success = analise.analyze();
    if (success) {
        analise.print();
    } else {
        std::cout << "Falha na analise";
    }
    
    return 0;
}
#include "compilador.h"
#include <iostream>

int main() {
    LexicalAnalysis analise("10+2\n5.2-3.1");
    bool success = analise.analyze();
    if (success) {
        analise.print();
    } else {
        std::cout << "Falha na analise";
    }
    
    return 0;
}
#include "util.h"

std::string parse_file_to_string(std::string_view filepath) {
    std::ifstream file(filepath.data());
    std::ostringstream buffer;

    if (!file.is_open()) {
        std::cerr << "File not found\n";
        return std::string();
    }

    std::string line;
    while(std::getline(file, line)) {
        buffer << line << "\n";
    }

    return buffer.str();
}

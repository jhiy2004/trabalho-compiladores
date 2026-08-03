#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include "compilador_lalg.h"
#include "compilador.h"
#include "command.h"

std::ostream& operator<<(std::ostream& os, const Token& t);
std::string parse_file_to_string(const std::filesystem::path& filepath);
std::string type_to_str(TokenType t);
void print_tokens(const std::vector<Token>& tokens);
void print_tokens_calc(const std::vector<TokenCalc>& tokens);

template<typename T>
void writeBinary(std::ostream& os, const T& value) {
    os.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

void save_program(const std::vector<Command>& cmds, const std::filesystem::path& filename);
std::vector<Command> generate_example1_builder();
std::vector<Command> generate_example2_builder();

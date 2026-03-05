#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include "compilador_lalg.h"

std::ostream& operator<<(std::ostream& os, const Token& t);
std::string parse_file_to_string(const std::filesystem::path& filepath);
std::string type_to_str(TokenType t);
void print_tokens(const std::vector<Token>& tokens);

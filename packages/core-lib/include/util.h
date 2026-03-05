#pragma once

#include <string>
#include <string_view>

#include <fstream>
#include <sstream>
#include <iostream>

std::string parse_file_to_string(std::string_view filepath);

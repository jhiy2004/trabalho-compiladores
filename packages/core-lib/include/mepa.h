#pragma once

#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <cstddef>
#include <optional>

// TODO: Maybe explicitly disable padding for this structure
struct Command {
    enum Type {
        CRCT, //1 arg
        CRVL, //1 arg
        ARMZ, //1 arg

        // Arithmetic ops
        SOMA, //0 arg
        SUBT, //0 arg
        MULT, //0 arg
        DIVI, //0 arg
        MODI, //0 arg

        //Sets ops
        INVR, //0 arg
        CONJ, //0 arg
        DISJ, //0 arg
        NEGA, //0 arg
        CMME, //0 arg
        CMMA, //0 arg
        CMIG, //0 arg
        CMDG, //0 arg
        CMAG, //0 arg
        CMEG, //0 arg

        //Jumps ops
        DSVS, //1 arg
        DSVF, //1 arg
        NOPE, //0 arg

        //I/O ops
        LEIT, //0 arg
        LECH, //0 arg
        IMPR, //0 arg
        IMPC, //0 arg
        IMPE, //0 arg

        //Ops
        INPP, //0 arg
        AMEM, //1 arg
        CDEM, //1 arg
        PARA, //0 arg
    };

    int get_number_args() {
        switch(type) {
            case CRCT:
            case CRVL:
            case ARMZ:
            case AMEM:
            case CDEM:
            case DSVF:
            case DSVS:
                return 1;
            default:
                return 0;
        };
    }

    Type type;
    std::optional<int> arg;
};

// TODO: Refatorar a logica de armazenamento de dados, deve suportar int e char, operacoes push, pop, e set
class Mepa {
public:
    Mepa(const std::filesystem::path& filename) : file(filename, std::ios::binary), data_area(100) {
        if (!file.is_open()) {
            throw std::runtime_error("Failed to read .lar file");
        }

        while (true) {
            Command cmd;

            if (!file.read(reinterpret_cast<char*>(&cmd.type), sizeof(cmd.type))) {
                break;
            }

            if (cmd.get_number_args() == 1) {
                int arg;
                if (!file.read(reinterpret_cast<char*>(&arg), sizeof(arg))) {
                    throw std::runtime_error("Unexpected end of file");
                }
                cmd.arg = arg;
            }

            code_area.push_back(cmd);
        }

        if (code_area.size() > 0 && code_area[0].type != Command::INPP) {
            throw std::runtime_error("First command isn't a INPP");
        }

        if (code_area.size() > 0 && code_area[code_area.size() - 1].type != Command::PARA) {
            throw std::runtime_error("Last command isn't a PARA");
        }
    }
    ~Mepa() {
        file.close();
    }


    void run() {
        while (pc < code_area.size()) {
            handle_cmd();
#ifdef DEBUG
            std::cerr << code_area[pc] << "\n";
#endif
        }
    }
private:
    int pop_int(int start) {
        int ans{};

        memcpy(&ans, data_area.data() + start, sizeof(int));

        st -= sizeof(int);

        return ans;
    }

    char pop_char(int start) {
        char ans{static_cast<char>(data_area[start])};

        st -= sizeof(char);

        return ans;
    }

    void push_int(int n) {
        std::size_t oldSize{data_area.size()};
        data_area.resize(oldSize + sizeof(int));

        memcpy(data_area.data() + oldSize, &n, sizeof(int));

        st += sizeof(int);
    }

    void push_char(char c) {
        std::size_t oldSize{data_area.size()};
        data_area.resize(oldSize + sizeof(char));

        memcpy(data_area.data() + oldSize, &c, sizeof(char));

        st += sizeof(char);
    }

    void handle_cmd() {
        if (pc < 0 || pc >= code_area.size()) {
#ifdef DEBUG
            std::cerr << "PC out of bounds: " << pc << "\n";
#endif
            return;
        }

        Command cmd{code_area[pc]};
        switch(cmd.type) {
            case Command::CRCT:
                push_int(*cmd.arg);
                ++pc;
                break;

            case Command::CRVL:
                int value{}
                push_int(data_area[*cmd.arg]);
                ++pc;
                break;

            case Command::ARMZ:
                data_area[*cmd.arg] = data_area[st];
                ++pc;
                break;

            case Command::SOMA:
                data_area[st-1] = data_area[st-1] + data_area[st];
                ++pc;
                break;

            case Command::SUBT:
                data_area[st-1] = data_area[st-1] - data_area[st];
                --st;
                ++pc;
                break;

            case Command::MULT:
                data_area[st-1] = data_area[st-1] * data_area[st];
                --st;
                ++pc;
                break;

            case Command::DIVI:
                data_area[st-1] = data_area[st-1] / data_area[st];
                --st;
                ++pc;
                break;

            case Command::MODI:
                data_area[st-1] = data_area[st-1] % data_area[st];
                --st;
                ++pc;
                break;

            case Command::INVR:
                data_area[st] = -data_area[st];
                ++pc;
                break;

            case Command::CONJ:
                data_area[st-1] = data_area[st-1] && data_area[st];
                --st;
                ++pc;
                break;

            case Command::DISJ:
                data_area[st-1] = data_area[st-1] || data_area[st];
                --st;
                ++pc;
                break;

            case Command::NEGA:
                data_area[st] = !data_area[st];
                ++pc;
                break;

            case Command::CMME:
                data_area[st-1] = (data_area[st-1] < data_area[st]) ? 1 : 0;
                --st;
                ++pc;
                break;

            case Command::CMMA:
                data_area[st-1] = (data_area[st-1] > data_area[st]) ? 1 : 0;
                --st;
                ++pc;
                break;

            case Command::CMIG:
                data_area[st-1] = (data_area[st-1] == data_area[st]) ? 1 : 0;
                --st;
                ++pc;
                break;

            case Command::CMDG:
                data_area[st-1] = (data_area[st-1] != data_area[st]) ? 1 : 0;
                --st;
                ++pc;
                break;

            case Command::CMAG:
                data_area[st-1] = (data_area[st-1] >= data_area[st]) ? 1 : 0;
                --st;
                ++pc;
                break;

            case Command::CMEG:
                data_area[st-1] = (data_area[st-1] <= data_area[st]) ? 1 : 0;
                --st;
                ++pc;
                break;

            case Command::DSVS:
                pc = *cmd.arg;
                break;

            case Command::DSVF:
                if (data_area[st] == 0) {
                    pc = *cmd.arg;
                } else {
                    ++pc;
                }
                --st;
                break;

            case Command::NOPE:
#ifdef DEBUG
                std::cerr << "NOPE\n";
#endif
                break;
            default:
                break;
        }
    }

    int pc{}; // Program Counter
    int st{-1}; // Stack Top

    std::fstream file;

    std::vector<Command> code_area;
    std::vector<std::byte> data_area;
};

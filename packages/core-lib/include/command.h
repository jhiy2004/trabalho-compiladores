#pragma once

#include <cstdint>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <vector>

// TODO: Maybe explicitly disable padding for this structure
struct Command {
    enum Type : std::uint8_t {
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
        NADA, //0 arg

        //I/O ops
        LEIT, //0 arg
        LECH, //0 arg
        IMPR, //0 arg
        IMPC, //0 arg
        IMPE, //0 arg

        //Ops
        INPP, //0 arg
        AMEM, //1 arg
        DMEM, //1 arg
        PARA, //0 arg
    };

    int get_number_args() {
        switch(type) {
            case CRCT:
            case CRVL:
            case ARMZ:
            case AMEM:
            case DMEM:
            case DSVF:
            case DSVS:
                return 1;
            default:
                return 0;
        };
    }

    const char* to_string() const {
        switch(type) {
            case CRCT: return "CRCT";
            case CRVL: return "CRVL";
            case ARMZ: return "ARMZ";
            case SOMA: return "SOMA";
            case SUBT: return "SUBT";
            case MULT: return "MULT";
            case DIVI: return "DIVI";
            case MODI: return "MODI";
            case INVR: return "INVR";
            case CONJ: return "CONJ";
            case DISJ: return "DISJ";
            case NEGA: return "NEGA";
            case CMME: return "CMME";
            case CMMA: return "CMMA";
            case CMIG: return "CMIG";
            case CMDG: return "CMDG";
            case CMAG: return "CMAG";
            case CMEG: return "CMEG";
            case DSVS: return "DSVS";
            case DSVF: return "DSVF";
            case NADA: return "NADA";
            case LEIT: return "LEIT";
            case LECH: return "LECH";
            case IMPR: return "IMPR";
            case IMPC: return "IMPC";
            case IMPE: return "IMPE";
            case INPP: return "INPP";
            case AMEM: return "AMEM";
            case DMEM: return "DMEM";
            case PARA: return "PARA";
            default: return "UNK";
        }
    }

    bool is_jump() const {
        return (type == DSVS || type == DSVF);
    }

    friend std::ostream& operator<<(std::ostream& out, const Command& cmd) {
        out << cmd.to_string();

        if (cmd.arg) {
            out << " " << *cmd.arg;
        }

        return out;
    }

    Type type;
    std::optional<int> arg;
    std::optional<std::string> dst;
};


class CodeBuilder {
public:
    CodeBuilder& crct(int n) { 
        commands.emplace_back(Command{Command::CRCT, n});
        return *this;
    } 

    CodeBuilder& crvl(int n) { 
        commands.emplace_back(Command{Command::CRVL, n});
        return *this;
    } 

    CodeBuilder& armz(int n) { 
        commands.emplace_back(Command{Command::ARMZ, n});
        return *this;
    }

    CodeBuilder& soma() {
        commands.emplace_back(Command{Command::SOMA});
        return *this;
    }

    CodeBuilder& subt() {
        commands.emplace_back(Command{Command::SUBT});
        return *this;
    }

    CodeBuilder& mult() {
        commands.emplace_back(Command{Command::MULT});
        return *this;
    }

    CodeBuilder& divi() {
        commands.emplace_back(Command{Command::DIVI});
        return *this;
    }

    CodeBuilder& modi() {
        commands.emplace_back(Command{Command::MODI});
        return *this;
    }

    CodeBuilder& invr() {
        commands.emplace_back(Command{Command::INVR});
        return *this;
    }

    CodeBuilder& conj() {
        commands.emplace_back(Command{Command::CONJ});
        return *this;
    }

    CodeBuilder& disj() {
        commands.emplace_back(Command{Command::DISJ});
        return *this;
    }

    CodeBuilder& nega() {
        commands.emplace_back(Command{Command::NEGA});
        return *this;
    }

    CodeBuilder& cmme() {
        commands.emplace_back(Command{Command::CMME});
        return *this;
    }

    CodeBuilder& cmma() {
        commands.emplace_back(Command{Command::CMMA});
        return *this;
    }

    CodeBuilder& cmig() {
        commands.emplace_back(Command{Command::CMIG});
        return *this;
    }

    CodeBuilder& cmdg() {
        commands.emplace_back(Command{Command::CMDG});
        return *this;
    }

    CodeBuilder& cmag() {
        commands.emplace_back(Command{Command::CMAG});
        return *this;
    }

    CodeBuilder& cmeg() {
        commands.emplace_back(Command{Command::CMEG});
        return *this;
    }

    CodeBuilder& dsvs(const std::string& dst) {
        commands.emplace_back(Command{Command::DSVS, std::nullopt, dst});
        return *this;
    }

    CodeBuilder& dsvf(const std::string& dst) {
        commands.emplace_back(Command{Command::DSVF, std::nullopt, dst});
        return *this;
    }

    CodeBuilder& nada(const std::string& label) {
        commands.emplace_back(Command{Command::NADA});
        label_to_pos[label] = commands.size() - 1;
        return *this;
    }

    CodeBuilder& leit() {
        commands.emplace_back(Command{Command::LEIT});
        return *this;
    }

    CodeBuilder& lech() {
        commands.emplace_back(Command{Command::LEIT});
        return *this;
    }

    CodeBuilder& impr() {
        commands.emplace_back(Command{Command::IMPR});
        return *this;
    }

    CodeBuilder& impc() {
        commands.emplace_back(Command{Command::IMPC});
        return *this;
    }

    CodeBuilder& impe() {
        commands.emplace_back(Command{Command::IMPE});
        return *this;
    }

    CodeBuilder& inpp() {
        commands.emplace_back(Command{Command::INPP});
        return *this;
    }

    CodeBuilder& amem(int n) {
        commands.emplace_back(Command{Command::AMEM, n});
        return *this;
    }

    CodeBuilder& dmem(int n) {
        commands.emplace_back(Command{Command::DMEM, n});
        return *this;
    }

    CodeBuilder& para() {
        commands.emplace_back(Command{Command::PARA});
        return *this;
    }

    std::string get_label() {
        return "L" + std::to_string(count++);
    }

    std::vector<Command> get_commands() {
        std::vector<Command> ans(commands.size());
        for (int i{}; i < commands.size(); ++i) {
            Command& c{commands[i]};
            if (c.is_jump()) {
                c.arg = label_to_pos[*c.dst];
            }
            ans[i] = c;
        }

        return ans;
    }
private:
    // Labeled commands
    std::vector<Command> commands;

    // Hashmap of label to position in array
    std::unordered_map<std::string, int> label_to_pos;
    int count{1};
};

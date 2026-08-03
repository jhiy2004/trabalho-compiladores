#pragma once

#include "command.h"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <cstddef>
#include <optional>
#include <queue>
#include <string>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <utility>

template <typename T>
class ConcurrentQueue {
public:
    void push(const T& value) {
        {
            std::lock_guard lock(mutex);
            queue.push(value);
        }
        cv.notify_one();
    }

    T pop() {
        std::unique_lock lock(mutex);

        cv.wait(lock, [&] {
            return !queue.empty();
        });

        T value = std::move(queue.front());
        queue.pop();
        return value;
    }

    void wait_until_size(std::size_t n) {
        std::unique_lock lock(mutex);

        cv.wait(lock, [&] {
            return queue.size() >= n;
        });
    }

private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cv;
};

class RawQueue {
public:
    void push(char c) {
        const std::byte* ptr =
            reinterpret_cast<const std::byte*>(&c);

        for (std::size_t i = 0; i < sizeof(char); ++i)
            bytes.push(ptr[i]);
    }

    void push(int n) {
        const std::byte* ptr =
            reinterpret_cast<const std::byte*>(&n);

        for (std::size_t i = 0; i < sizeof(int); ++i)
            bytes.push(ptr[i]);
    }

    char pop_char() {
        return static_cast<char>(bytes.pop());
    }

    int pop_int() {
        bytes.wait_until_size(sizeof(int));

        int value{};
        std::byte* ptr = reinterpret_cast<std::byte*>(&value);

        for (std::size_t i = 0; i < sizeof(int); ++i)
            ptr[i] = bytes.pop();

        return value;
    }

private:
    ConcurrentQueue<std::byte> bytes;
};

class DataArea {
public: 
    DataArea() : vec{}, st{-1} {
    };

    void set_value(int value, int idx) {
        idx *= 4;
        memcpy(&vec[idx], &value, sizeof(int));
    }

    void set_value(char value, int idx) {
        vec[idx] = static_cast<std::byte>(value);
    }

    int load(int idx) {
        int ans{};

        memcpy(&ans, vec.data() + sizeof(int) * idx, sizeof(int));

        return ans;
    }

    int pop_int() {
        int ans{};

        memcpy(&ans, vec.data() + (st - (sizeof(int) - 1)), sizeof(int));

        st -= sizeof(int);
        vec.resize(vec.size() - sizeof(int));

        return ans;
    }

    char pop_char() {
        char ans{static_cast<char>(vec[st])};

        st -= sizeof(char);
        vec.resize(vec.size() - sizeof(char));

        return ans;
    }

    void push(int n) {
        std::size_t oldSize{vec.size()};
        vec.resize(oldSize + sizeof(int));

        memcpy(vec.data() + oldSize, &n, sizeof(int));

        st += sizeof(int);
    }

    void push(char c) {
        std::size_t oldSize{vec.size()};
        vec.resize(oldSize + sizeof(char));

        memcpy(vec.data() + oldSize, &c, sizeof(char));

        st += sizeof(char);
    }

    void reset() {
        st = -1;
    }

    void alloc(int m) {
        vec.resize(vec.size() + m * sizeof(int));
        st += m * sizeof(int);

#ifdef DEBUG
    std::cerr << "\nst: " << st << "\n";
#endif
    }

    void free(int m) {
        vec.resize(vec.size() - m * sizeof(int));
        st -= m * sizeof(int);

#ifdef DEBUG
    std::cerr << "\nst: " << st << "\n";
#endif
    }
private:
    std::vector<std::byte> vec;
    int st;
};

class Mepa {
public:
    enum class InputType {
        Int,
        Char,
        None,
        End,
    };

    Mepa(const std::vector<Command>& cmds) : code_area(cmds) {
        if (code_area.size() > 0 && code_area[0].type != Command::INPP) {
            throw std::runtime_error("First command isn't a INPP");
        }

        if (code_area.size() > 0 && code_area[code_area.size() - 1].type != Command::PARA) {
            throw std::runtime_error("Last command isn't a PARA");
        }
    }

    Mepa(const std::filesystem::path& filename) : file(filename, std::ios::in | std::ios::binary), data_area{} {
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

    std::string pop_output() {
        return output.pop();
    }

    void push_input(char c) {
        input.push(c);
    }

    void push_input(int n) {
        input.push(n);
    }

    InputType next_input_type() const {
        if (pc >= code_area.size())
            return InputType::End;

        switch (code_area[pc].type) {
            case Command::LEIT:
                return InputType::Int;

            case Command::LECH:
                return InputType::Char;

            default:
                return InputType::None;
        }
    }

    void run() {
        while (pc < code_area.size()) {
#ifdef DEBUG
            std::cerr << code_area[pc] << "\n";
#endif
            handle_cmd();
        }

#ifdef DEBUG
        std::cerr << "MEPA Program finished execution" << "\n";
#endif
        output.push("__EXIT__");
    }
private:
    void handle_cmd() {
        if (pc < 0 || pc >= code_area.size()) {
#ifdef DEBUG
            std::cerr << "PC out of bounds: " << pc << "\n";
#endif
            return;
        }

        int value{};
        int value2{};
        std::stringstream ss;

        Command cmd{code_area[pc]};
        switch(cmd.type) {
            case Command::CRCT:
                data_area.push(*cmd.arg);
                ++pc;
                break;

            case Command::CRVL:
                value = data_area.load(*cmd.arg);
                data_area.push(value);
                ++pc;
                break;

            case Command::ARMZ:
                value = data_area.pop_int();
                data_area.set_value(value, *cmd.arg);
                ++pc;
                break;

            case Command::SOMA:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push(value2 + value);
                ++pc;
                break;

            case Command::SUBT:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push(value2 - value);
                ++pc;
                break;

            case Command::MULT:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push(value2 * value);
                ++pc;
                break;

            case Command::DIVI:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push(value2 / value);
                ++pc;
                break;

            case Command::MODI:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push(value2 % value);
                ++pc;
                break;

            case Command::INVR:
                value = data_area.pop_int();

                data_area.push(-value);
                ++pc;
                break;

            case Command::CONJ:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push(value2 && value);
                ++pc;
                break;

            case Command::DISJ:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push(value2 || value);
                ++pc;
                break;

            case Command::NEGA:
                value = data_area.pop_int();

                data_area.push(!value);
                ++pc;
                break;

            case Command::CMME:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push((value2 < value) ? 1 : 0);
                ++pc;
                break;

            case Command::CMMA:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push((value2 > value) ? 1 : 0);
                ++pc;
                break;

            case Command::CMIG:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push((value2 == value) ? 1 : 0);
                ++pc;
                break;

            case Command::CMDG:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push((value2 != value) ? 1 : 0);
                ++pc;
                break;

            case Command::CMAG:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push((value2 >= value) ? 1 : 0);
                ++pc;
                break;

            case Command::CMEG:
                value = data_area.pop_int();
                value2 = data_area.pop_int();

                data_area.push((value2 <= value) ? 1 : 0);
                ++pc;
                break;

            case Command::DSVS:
                pc = *cmd.arg;
                break;

            case Command::DSVF:
                value = data_area.pop_int();

                if (value == 0) {
                    pc = *cmd.arg;
                } else {
                    ++pc;
                }
                break;

            case Command::LEIT:
                output.push("Enter a int: ");
                data_area.push(input.pop_int());
                ++pc;
                break;

            case Command::LECH:
                output.push("Enter a char: ");
                data_area.push(input.pop_char());
                ++pc;
                break;

            case Command::IMPR:
                ss << data_area.pop_int();

                output.push(ss.str());
                ++pc;
                break;

            case Command::IMPC:
                ss << data_area.pop_char();

                output.push(ss.str());
                ++pc;
                break;
            
            case Command::IMPE:
                ss << data_area.pop_int() << "\n";

                output.push(ss.str());
                ++pc;
                break;

            case Command::INPP:
                data_area.reset();
                ++pc;
                break;

            case Command::AMEM:
                data_area.alloc(*cmd.arg);
                ++pc;
                break;

            case Command::DMEM:
                data_area.free(*cmd.arg);
                ++pc;
                break;

            case Command::PARA:
                ++pc;
                break;

            case Command::NADA:
                ++pc;
                break;
            default:
                break;
        }
    }

    int pc{}; // Program Counter

    std::fstream file;

    std::vector<Command> code_area;
    DataArea data_area;

    RawQueue input;
    ConcurrentQueue<std::string> output;
};


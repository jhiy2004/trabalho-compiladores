#include "compilador.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include "util.h"

#define NDEBUG
#include "mepa.h"

template<typename T>
void writeBinary(std::ostream& os, const T& value) {
    os.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

void background_output(Mepa& mepa) {
    while (true) {
        std::string s = mepa.pop_output();

        if (s == "__EXIT__")
            break;

        std::cout << s << std::flush;
    }
}

void background_input(Mepa& mepa) {
    while (true) {
        switch (mepa.next_input_type()) {
            case Mepa::InputType::Int: {
                int value;
                std::cin >> value;
                mepa.push_input(value);
                break;
            }

            case Mepa::InputType::Char: {
                char value;
                std::cin >> value;
                mepa.push_input(value);
                break;
            }
        
            case Mepa::InputType::End: return;
        }
    }
}

void generate_mepa_example1(const std::filesystem::path& filename) {
    std::fstream file(filename, std::ios::binary | std::ios::out);

    writeBinary(file, Command::INPP);

    writeBinary(file, Command::AMEM);
    writeBinary(file, 1);

    writeBinary(file, Command::AMEM);
    writeBinary(file, 1);

    writeBinary(file, Command::LEIT);

    writeBinary(file, Command::ARMZ);
    writeBinary(file, 0);

    writeBinary(file, Command::LEIT);

    writeBinary(file, Command::ARMZ);
    writeBinary(file, 1);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 0);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 1);

    writeBinary(file, Command::CMMA);

    writeBinary(file, Command::DSVF);
    writeBinary(file, 0);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 0);

    writeBinary(file, Command::IMPE);
    writeBinary(file, Command::NOPE);
    writeBinary(file, Command::PARA);

    file.close();
}

void generate_mepa_example2(const std::filesystem::path& filename) {
    std::fstream file(filename, std::ios::binary | std::ios::out);

    writeBinary(file, Command::INPP);

    writeBinary(file, Command::AMEM);
    writeBinary(file, 1);

    writeBinary(file, Command::AMEM);
    writeBinary(file, 1);

    writeBinary(file, Command::AMEM);
    writeBinary(file, 1);

    writeBinary(file, Command::LEIT);

    writeBinary(file, Command::ARMZ);
    writeBinary(file, 1);

    writeBinary(file, Command::LEIT);

    writeBinary(file, Command::ARMZ);
    writeBinary(file, 2);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 0);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 2);

    writeBinary(file, Command::CRCT);
    writeBinary(file, 10);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 1);

    writeBinary(file, Command::DIVI);
    
    writeBinary(file, Command::CRCT);
    writeBinary(file, 1);

    writeBinary(file, Command::SOMA);
    writeBinary(file, Command::MULT);
    writeBinary(file, Command::SOMA);

    writeBinary(file, Command::ARMZ);
    writeBinary(file, 0);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 0);

    writeBinary(file, Command::IMPE);
    
    writeBinary(file, Command::CRVL);
    writeBinary(file, 1);

    writeBinary(file, Command::IMPE);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 2);

    writeBinary(file, Command::IMPE);
   
    writeBinary(file, Command::CRVL);
    writeBinary(file, 0);

    writeBinary(file, Command::CRCT);
    writeBinary(file, 0);

    writeBinary(file, Command::CMMA);

    writeBinary(file, Command::DSVF);
    writeBinary(file, 44);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 1);

    writeBinary(file, Command::CRCT);
    writeBinary(file, 0);

    writeBinary(file, Command::CMMA);

    writeBinary(file, Command::DSVF);
    writeBinary(file, 37);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 0);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 1);

    writeBinary(file, Command::MULT);

    writeBinary(file, Command::ARMZ);
    writeBinary(file, 1);

    writeBinary(file, Command::DSVS);
    writeBinary(file, 42);

    writeBinary(file, Command::NOPE);
    
    writeBinary(file, Command::CRVL);
    writeBinary(file, 1);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 0);

    writeBinary(file, Command::SOMA);
    
    writeBinary(file, Command::ARMZ);
    writeBinary(file, 1);

    writeBinary(file, Command::NOPE);

    writeBinary(file, Command::DSVS);
    writeBinary(file, 60);

    writeBinary(file, Command::NOPE);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 1);

    writeBinary(file, Command::CRCT);
    writeBinary(file, 0);

    writeBinary(file, Command::CMMA);

    writeBinary(file, Command::DSVF);
    writeBinary(file, 54);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 1);

    writeBinary(file, Command::CRCT);
    writeBinary(file, -1);

    writeBinary(file, Command::MULT);

    writeBinary(file, Command::ARMZ);
    writeBinary(file, 1);

    writeBinary(file, Command::DSVS);
    writeBinary(file, 59);

    writeBinary(file, Command::NOPE);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 1);

    writeBinary(file, Command::CRVL);
    writeBinary(file, 2);

    writeBinary(file, Command::SOMA);

    writeBinary(file, Command::ARMZ);
    writeBinary(file, 1);

    writeBinary(file, Command::NOPE);
    writeBinary(file, Command::NOPE);

    writeBinary(file, Command::PARA);

    file.close();
}

void execute_generate_mepa_example(const std::filesystem::path& filename) {
    try {
        Mepa mepa(filename);

        std::thread output_thread(background_output, std::ref(mepa));
        std::thread input_thread(background_input, std::ref(mepa));

        mepa.run();

        input_thread.join();
        output_thread.join();
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }
}

int main() {
    auto filepath1{std::filesystem::path(EXAMPLES_DIR) / "mepa_example_1.lar"};
    auto filepath2{std::filesystem::path(EXAMPLES_DIR) / "mepa_example_2.lar"};


    generate_mepa_example2(filepath2);
    //generate_mepa_example1(filepath1);
    execute_generate_mepa_example(filepath2);

    return 0;
}

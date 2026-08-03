#include "compilador.h"
#include <iostream>
#include <filesystem>
#include "util.h"
#include <thread>

#define NDEBUG
#include "mepa.h"

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

void execute_generate_mepa_example_file(const std::filesystem::path& filename) {
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

void execute_generate_mepa_example_vec(const std::vector<Command>& cmds) {
    try {
        Mepa mepa(cmds);

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

    auto vec1{generate_example1_builder()};
    auto vec2{generate_example2_builder()};

    save_program(vec1, filepath1);
    save_program(vec2, filepath1);

    execute_generate_mepa_example_file(filepath2);
    execute_generate_mepa_example_file(filepath2);

    //execute_generate_mepa_example_vec(vec2);

    return 0;
}

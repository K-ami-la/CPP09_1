#include "RPN.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Error: wrong number of arguments." << std::endl;
        return 1;
    }

    RPN rpn;
    try {
        std::cout << rpn.evaluate(argv[1]) << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

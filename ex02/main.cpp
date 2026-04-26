#include "PmergeMe.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: no arguments provided." << std::endl;
        return 1;
    }

    PmergeMe pm;
    try {
        pm.parse(argc, argv);
        pm.sort();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

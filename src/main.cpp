#include <iostream>
#include <string>
#include <cstdlib>
#include "avatar_generator.hpp"
#include "md5.hpp"

void print_usage(const char* program_name) {
    std::cout << "HashFace - GitHub-style Avatar Generator\n\n";
    std::cout << "Usage: " << program_name << " [options] <input_string>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o <file>     Output filename (default: avatar.png)\n";
    std::cout << "  -s <size>     Image size in pixels (default: 420)\n";
    std::cout << "  -g <grid>     Grid size (default: 5)\n";
    std::cout << "  -h, --help    Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " \"john@example.com\"\n";
    std::cout << "  " << program_name << " -o user123.png -s 256 \"user123\"\n";
    std::cout << "  " << program_name << " -g 7 \"octocat\"\n";
}

int main(int argc, char* argv[]) {
    std::string output_file = "avatar.png";
    std::string input_string;
    int size = 420;
    int grid_size = 5;
    
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else if (arg == "-o") {
            if (i + 1 >= argc) {
                std::cerr << "Error: -o requires a filename argument\n";
                return 1;
            }
            output_file = argv[++i];
        } else if (arg == "-s") {
            if (i + 1 >= argc) {
                std::cerr << "Error: -s requires a size argument\n";
                return 1;
            }
            size = std::atoi(argv[++i]);
            if (size <= 0) {
                std::cerr << "Error: size must be positive\n";
                return 1;
            }
        } else if (arg == "-g") {
            if (i + 1 >= argc) {
                std::cerr << "Error: -g requires a grid size argument\n";
                return 1;
            }
            grid_size = std::atoi(argv[++i]);
            if (grid_size <= 0) {
                std::cerr << "Error: grid size must be positive\n";
                return 1;
            }
        } else if (arg[0] != '-') {
            input_string = arg;
        } else {
            std::cerr << "Error: Unknown option: " << arg << "\n";
            print_usage(argv[0]);
            return 1;
        }
    }
    
    if (input_string.empty()) {
        std::cerr << "Error: No input string provided\n\n";
        print_usage(argv[0]);
        return 1;
    }
    
    try {
        // Create generator
        hashface::AvatarGenerator generator(size, grid_size);
        
        // Show hash
        auto hash = hashface::MD5::hash(input_string);
        std::cout << "Input: " << input_string << "\n";
        std::cout << "MD5:   " << hashface::MD5::to_hex(hash) << "\n";
        std::cout << "Size:  " << size << "x" << size << " pixels\n";
        std::cout << "Grid:  " << grid_size << "x" << grid_size << "\n";
        
        // Generate avatar
        if (generator.generate_to_file(input_string, output_file)) {
            std::cout << "Saved: " << output_file << "\n";
            return 0;
        } else {
            std::cerr << "Error: Failed to write output file\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
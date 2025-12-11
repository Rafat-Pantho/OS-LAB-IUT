#include<iostream>
#include <cmath>
#include <limits> 
#include <string>
#include <chrono>

#include "printer.h"
#include "vid2arr.h"



class renderer {

private:
    int target_width,target_height;
    std::string input_filepath;

public:

    renderer(){};
    /**
     * Gathers user input for dimensions and file path, then enters an infinite loop.
     */
    void run_render_loop() {
        // 1. Get Input from user
        // std::cout << "--- ASCII Renderer Setup ---" << std::endl;

        // // Input Width with validation
        // std::cout << "Enter target output width (columns): ";
        // while (!(std::cin >> target_width) || target_width <= 0) {
        //     std::cout << "Invalid input. Please enter a positive integer for width: ";
        //     std::cin.clear(); // Clear error flags
        //     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
        // }

        // // Input Height with validation
        // std::cout << "Enter target output height (rows): ";
        // while (!(std::cin >> target_height) || target_height <= 0) {
        //     std::cout << "Invalid input. Please enter a positive integer for height: ";
        //     std::cin.clear();
        //     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        // }
        
        // // Input Filepath
        // std::cout << "Enter image file path to process: ";
        // // Clear the buffer after numeric input to allow getline to work correctly
        // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        // std::getline(std::cin, input_filepath);
        
        target_width=500;
        target_height=150;
        input_filepath="mojo.mp4";
        

        std::cout << "\nStarting render loop with parameters:" << std::endl;
        std::cout << "  Width: " << target_width << std::endl;
        std::cout << "  Height: " << target_height << std::endl;
        std::cout << "  File: " << input_filepath << std::endl;
        std::cout << "Press Ctrl+C to stop the loopy." << std::endl;
        
        // 2. The empty while loop requested by the user
        
        

        vid2arr f =  vid2arr(input_filepath,target_height,target_width);
        console_init();

        while (true) {

            auto start=std::chrono::high_resolution_clock::now();
            std::vector<std::vector<char>> arr = f.get_current_frame();
            printBuffer(arr);

            auto duration = std::chrono::duration_cast<
            std::chrono::microseconds>(std::chrono::high_resolution_clock::now()-start).count();

            usleep(std::max(40000 - duration, (long)0 ));
        }
    }

};
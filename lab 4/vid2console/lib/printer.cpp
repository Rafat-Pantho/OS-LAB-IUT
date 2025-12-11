#include "printer.h"


void printBuffer(const std::vector<std::vector<char>>& arr){

// 1. Estimate buffer size (generous estimate needed)
    // Max 10 digits per int + 1 space/newline per element
    const size_t N = arr.size();
    const size_t M = arr[0].size();

    const size_t BUFFER_SIZE = N * M * 12; 
    std::string buffer(BUFFER_SIZE, '\0');
    size_t current_pos = 0;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            // 2. Use snprintf to format into the buffer
            // snprintf returns the number of characters that would have been written
            int written = std::snprintf(&buffer[current_pos], BUFFER_SIZE - current_pos, "%c", arr[i][j]);
            current_pos += written;
            
            if ( j == M-1 ) {
                written = std::snprintf(&buffer[current_pos], BUFFER_SIZE - current_pos,  "%c", '\n');
                current_pos += written;
            }

            if (current_pos >= BUFFER_SIZE - 20) { /* Handle buffer overflow */ }
        }
    }
    
    // 3. Print the single, pre-formatted buffer
    std::fputs(buffer.c_str(), stdout);
    printf("\033[H");
    return;
}

void console_init(){
    printf("\033[H");
    printf("\033[?25l");
}
#include "vid2arr.h"



vid2arr::vid2arr(std::string& filepath,int height,int width){

    this->H=height;
    this->W=width;
    this->path=filepath;
    this->readVideoFramesGray3D();
}

std::vector<std::vector<char>> vid2arr::get_current_frame(){

    this->cur_frame_idx=this->cur_frame_idx%this->frame_data.size();

    // std::cout<<frame_data[cur_frame_idx-1].size()<<'\n';
    return this->frame2char(this->frame_data[this->cur_frame_idx++]);
}

void vid2arr::readVideoFramesGray3D(){
    
    
    cv::VideoCapture cap(this->path);
    if (!cap.isOpened()) {
        throw std::runtime_error("Cannot open video: " + this->path);
    }
    

    cv::Mat frame, gray_full, gray_resized;

    cv::Size size(W,H);
    

    while (cap.read(frame)) {
        // Convert to grayscale (CV_8UC1)
        cv::cvtColor(frame, gray_full, cv::COLOR_BGR2GRAY);
        cv::resize(gray_full,gray_resized,size);
        int h = gray_resized.rows;
        int w = gray_resized.cols;

        // std::cout<<h<<" "<<w<<"\n";
        // Allocate 2D uint8 array
        std::vector<std::vector<uint8_t>> img2D(h,std::vector<uint8_t>(w,0));

        // Copy grayscale pixels into 2D vector
        for (int r = 0; r < h; r++) {
            const uint8_t* rowPtr = gray_resized.ptr<uint8_t>(r);
            for (int c = 0; c < w; c++) {
                img2D[r][c] = rowPtr[c];
            }
        }

        this->frame_data.push_back(img2D);
    }

}




std::vector<std::vector<char>> vid2arr::frame2char(const std::vector<std::vector<uint8_t>>& arr) {
    
    
    // Return empty if input is empty

    const std::string ASCII_RAMP = " .'`^\\\",:;Il!i><~+_-?][}{1)(|\\\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"; 


    size_t rows = arr.size();
    size_t cols = arr[0].size();
    size_t ramp_len = ASCII_RAMP.length();

    // Initialize the result vector with correct dimensions

    std::vector<std::vector<char>> result(rows, std::vector<char>(cols));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            uint8_t pixel_value = arr[i][j];

            // Map the 0-255 range to the 0-(ramp_len-1) range
            // Formula: index = (pixel_value * (max_index)) / 255
            size_t char_index = (pixel_value * (ramp_len - 1)) / 255;

            result[i][j] = ASCII_RAMP[char_index];
        }
    }

    return result;
}
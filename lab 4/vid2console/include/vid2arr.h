#include <opencv2/opencv.hpp>
#include <array>
#include <vector>
#include <string>
#include "framegenerator.h"



class vid2arr: public framegenerator {
private:
    int cur_frame_idx=0;
    std::vector<std::vector<std::vector<uint8_t >>> frame_data;

    std::string path;

    int H,W;
public:

    vid2arr(std::string &filepath,int height,int width);
    std::vector<std::vector<char>> get_current_frame();
    
private:
    void readVideoFramesGray3D();
    std::vector<std::vector<char>> frame2char(const std::vector<std::vector<uint8_t>>& arr);

};


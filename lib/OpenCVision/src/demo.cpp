#include <opencv2/highgui/highgui.hpp>
#include <free.hpp>
#include "yellow.hpp"

using namespace cv;

int main() {
    while (true) {
        Mat color = imread("../images/ir.png", CV_LOAD_IMAGE_GRAYSCALE);
        std::vector<YellowTote> totes = find_yellow_ir(color);
        //imshow("Input", color);
        //moveWindow("Input", 0, 0);
        int key = cv::waitKey(33) & 0xFF;
        if (key == 27) {
            break;
        }
    }
    cv::destroyAllWindows();
}

#include <vector>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

enum Side {
    UNK_SIDE = 0,
    LONG_SIDE,
    SHORT_SIDE
};

class YellowTote {
public:
    YellowTote(Side side, int x, int y); // constructor
    Side facingSide;
    double distance;
    float area_ratio;      //left contour area / right contour area
    double Calc_X_Rot();    // function
    cv::Point2f point();
    double x, y;         // variable
    bool stacked = false;
    friend bool operator==(const YellowTote& one, const YellowTote& two);
};

std::vector<YellowTote> find_yellow_ir(cv::Mat img);

std::vector<YellowTote> find_yellow_color(cv::Mat img);

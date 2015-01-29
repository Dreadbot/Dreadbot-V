#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>
#include <algorithm>
#include "yellow.hpp"
#include "util.hpp"

using namespace std;
using namespace cv;

enum LSide {
    LEFT_SIDE = 0,
    RIGHT_SIDE
};

struct SingleL {
    Point2f center;
    LSide side;
    Rect bound;
    float area;
    bool paired = false;
};

YellowTote::YellowTote(Side side, int x, int y) : facingSide(side), x(x), y(y)
{
}

double YellowTote::Calc_X_Rot()
{
    return 0;
}

Point2f YellowTote::point()
{
    return Point2f(x, y);
}

bool operator==(const YellowTote& one, const YellowTote& two)
{
    return one.x == two.x && one.y == two.y;
}

double distance(Point2f one, Point2f two) {
    return sqrt(pow(one.x - two.x, 2) + pow(one.y - two.y, 2));
}

static Mat kern = getStructuringElement(MORPH_CROSS, Size(3, 3), Point(-1, -1));


// this function may be bugged
std::vector<YellowTote> pairTotes(std::vector<SingleL> singles)
{
    std::vector<YellowTote> detected_totes;
    for (size_t i = 0; i < singles.size(); i++) {
        SingleL single = singles[i];
        if (single.paired)
            continue;
        for (size_t j = i + 1; j < singles.size(); j++) {
            SingleL other = singles[j];
            if (other.paired)
                continue;
            int dist = distance(single.center, other.center);
            int xdist = abs(single.center.x - other.center.x);
            if (dist < single.bound.height * 2 && xdist > 15) {
                other.paired = true;
                single.paired = true;
                YellowTote tote(LONG_SIDE, (single.center.x + other.center.x) / 2,
                                (single.center.y + other.center.y) / 2);
                if(single.side == LEFT_SIDE)
                {
                    tote.area_ratio = single.area/other.area;
                }
                else
                {
                    tote.area_ratio = other.area/single.area;
                }
                detected_totes.push_back(tote);
                break;
            }
        }
    }
    return detected_totes;
}

std::vector<YellowTote> find_yellow_ir(Mat ir)
{
    //CHANGE THESE TO WHAT YOU ARE USING
    int IMAGE_WIDTH = 1366;
    int IMAGE_HEIGHT = 786;
    double FOV_X = 25.45;


    double X_rot_stacked = -99;
    std::vector<SingleL> singles;
    Mat binary, draw;
    cvtColor(ir, draw, CV_GRAY2BGR);
    threshold(ir, binary, 250, 255, CV_THRESH_BINARY);
    dilate(binary, binary, kern, Point(-1,-1), 3);
    erode(binary, binary, kern, Point(-1,-1), 1);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(binary, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    vector<vector<Point> > polygons(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        vector<Point> contour = contours[i];
        int area = contourArea(contour);
        if (area > 100000 || area < 100) {
            continue;
        }
        vector<Point> polygon;
        approxPolyDP(contour, polygon, 5, true);
        polygons[i] = polygon;
        Rect bound = boundingRect(polygon);
        RotatedRect rotated = minAreaRect(polygon);
        rectangle(draw, bound, Scalar(78, 45, 68));

        Point2f vertices[4];
        rotated.points(vertices);

        Moments moment = moments(contour, false);
        Point2f massCenter(moment.m10/moment.m00, moment.m01/moment.m00);
        double boxCenter = bound.x + (bound.width / 2.0);
        line(draw, Point2f(boxCenter, 0), Point2f(boxCenter, 480), Scalar(0, 255, 0));
        SingleL single;
        single.center = massCenter;
        single.bound = bound;
        single.area= contourArea(contours[i]);
        if (boxCenter - massCenter.x > 0) {
            // closer to left side
            putText(draw, "R", massCenter, CV_FONT_HERSHEY_SIMPLEX, 2, COLOR_RED, 2);
            single.side = RIGHT_SIDE;
        } else {
            putText(draw, "L", massCenter, CV_FONT_HERSHEY_SIMPLEX, 2, COLOR_RED, 2);
            single.side = LEFT_SIDE;
        }
        circle(draw, massCenter, 3, COLOR_RED, 2);
        singles.push_back(single);
    }
    std::vector<YellowTote> detected_totes = pairTotes(singles);
    // operation: split the detected totes into ones that look stacked and ones that aren't
    std::vector<YellowTote> stacked_totes, unstacked_totes;
    for(unsigned int i= 0; i < detected_totes.size(); i++)
    {
        detected_totes[i].stacked = false;
    }
    for (size_t tote_i = 0; tote_i < detected_totes.size(); tote_i++) {
        for (size_t other_i =  tote_i + 1; other_i < detected_totes.size(); other_i++) {
            if (abs(detected_totes[tote_i].x - detected_totes[other_i].x) < 20) {
                if (!detected_totes[tote_i].stacked) {
                    detected_totes[tote_i].stacked = true;
                    stacked_totes.push_back(detected_totes[tote_i]);
                }
                if (!detected_totes[other_i].stacked) {
                    detected_totes[other_i].stacked = true;
                    stacked_totes.push_back(detected_totes[other_i]);
                }
            }
        }
    }
    for (unsigned int i = 0; i < detected_totes.size(); i ++)
    {
        if(!detected_totes[i].stacked)
        {
            unstacked_totes.push_back(detected_totes[i]);
        }

    }

    circle(draw, Point(stacked_totes[0].x, stacked_totes[0].y), 3, COLOR_BLUE);
    line(draw, Point(stacked_totes[0].x, stacked_totes[0].y), Point(stacked_totes[1].x, stacked_totes[1].y), COLOR_BLUE, 2);

    //Calculate X rotation

    if(stacked_totes.size() != 0)
    {
        stacked_totes[0].x = (stacked_totes[0].x - IMAGE_WIDTH/2);
        X_rot_stacked = stacked_totes[0].x/(IMAGE_WIDTH/2)*FOV_X;
    }
    double X_rot_unstacked[unstacked_totes.size()];
    for(unsigned int i = 0; i < unstacked_totes.size(); i ++)
    {
        unstacked_totes[i].x = (unstacked_totes[i].x - IMAGE_WIDTH/2);
        X_rot_unstacked[i] = unstacked_totes[i].x/(IMAGE_WIDTH/2)*FOV_X;
    }

    printf("Area Ratio = %f\n", stacked_totes[0].area_ratio);

    //Calculate real rotation to tote (to figure out how to line up perpendicular with it)
    printf("Area Ratio = %f\n", stacked_totes[0].area_ratio);
    float Stacked_Area_Ratio = stacked_totes[0].area_ratio;
    float Unstacked_Area_Ratio[unstacked_totes.size()];

    for(unsigned int i = 0; i < unstacked_totes.size(); i ++)
    {
        Unstacked_Area_Ratio[i] = unstacked_totes[i].area_ratio;
    }


    imshow("Drawing", draw);

    //variables you want to pass to the roboRIO:
    //X_rot_stacked
    //X_rot_unstacked[array]
    //Stacked_Area_Ratio
    //Unstacked_Area_Ratio

    return detected_totes;
}

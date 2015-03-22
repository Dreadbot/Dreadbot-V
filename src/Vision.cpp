#include "Vision.h"

using namespace dreadbot;

// http://en.wikipedia.org/wiki/Don%27t_repeat_yourself

Range DreadVision::TOTE_HUE_RANGE;
Range DreadVision::TOTE_SAT_RANGE;
Range DreadVision::TOTE_VAL_RANGE;
double DreadVision::AREA_MINIMUM;
double DreadVision::LONG_RATIO;
double DreadVision::SHORT_RATIO;
double DreadVision::SCORE_MIN;
double DreadVision::VIEW_ANGLE;
ParticleFilterOptions2 DreadVision::filterOptions;

Image *DreadVision::binaryFrame;
int DreadVision::imaqError;

ParticleFilterCriteria2 DreadVision::criteria[1];
DreadVision::Scores DreadVision::scores;

double DreadVision::strafeValue;
double DreadVision::distValue;

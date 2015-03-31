// @todo: Convert Vision class to singleton & separate into multiple files

#pragma once

#include "WPILib.h"
#include <vector>
#include <cmath>


// Some of the parameters used for vision processing will be input from the dashboard.
// Next line should be commented out after calibration for performance reasons
#define SD_TEST

namespace dreadbot {
	class DreadVision {

		//A structure to hold measurements of a particle
		struct ParticleReport {
			double PercentAreaToImageArea;
			double Area;
			double ConvexHullArea;
			double BoundingRectLeft;
			double BoundingRectTop;
			double BoundingRectRight;
			double BoundingRectBottom;
		};

		//Structure to represent the scores for the various tests used for target identification
		struct Scores {
			double Trapezoid;
			double LongAspect;
			double ShortAspect;
			double AreaToConvexHullArea;
		};

		//Images
		static Image *binaryFrame; // Last image passed to RegisterFrame with a threshold
		static int imaqError;

		// Constants
		static Range TOTE_HUE_RANGE;
		static Range TOTE_SAT_RANGE;
		static Range TOTE_VAL_RANGE;
		static double AREA_MINIMUM;
		static double LONG_RATIO;
		static double SHORT_RATIO;
		static double SCORE_MIN;
		static double VIEW_ANGLE;
		static ParticleFilterOptions2 filterOptions;
		static ParticleFilterCriteria2 criteria[1];
		static Scores scores;

		static double strafeValue;
		static double distValue;
		static bool seesTote;

		//Comparator function for sorting particles. Returns true if particle 1 is larger
		static bool CompareParticleSizes(ParticleReport particle1, ParticleReport particle2) {
			//we want descending sort order
			return particle1.PercentAreaToImageArea > particle2.PercentAreaToImageArea;
		}

		/**
		 * Converts a ratio with ideal value of 1 to a score. The resulting function is piecewise
		 * linear going from (0,0) to (1,100) to (2,0) and is 0 for all inputs outside the range 0-2
		 */
		static double ratioToScore(double ratio) {
			return (fmax(0, fmin(100*(1-fabs(1-ratio)), 100)));
		}

		/**
		 * Method to score convex hull area. This scores how "complete" the particle is. Particles with large holes will score worse than a filled in shape
		 */
		static double ConvexHullAreaScore(ParticleReport report) {
			return ratioToScore((report.Area/report.ConvexHullArea)*1.18);
		}

		/**
		 * Method to score if the particle appears to be a trapezoid. Compares the convex hull (filled in) area to the area of the bounding box.
		 * The expectation is that the convex hull area is about 95.4% of the bounding box area for an ideal tote.
		 */
		static double TrapezoidScore(ParticleReport report) {
			return ratioToScore(report.ConvexHullArea/((report.BoundingRectRight-report.BoundingRectLeft)*(report.BoundingRectBottom-report.BoundingRectTop)*.954));
		}

		/**
		 * Method to score if the aspect ratio of the particle appears to match the long side of a tote.
		 */
		static double LongSideScore(ParticleReport report) {
			return ratioToScore(((report.BoundingRectRight-report.BoundingRectLeft)/(report.BoundingRectBottom-report.BoundingRectTop))/LONG_RATIO);
		}

		/**
		 * Method to score if the aspect ratio of the particle appears to match the short side of a tote.
		 */
		static double ShortSideScore(ParticleReport report) {
			return ratioToScore(((report.BoundingRectRight-report.BoundingRectLeft)/(report.BoundingRectBottom-report.BoundingRectTop))/SHORT_RATIO);
		}

		static double computeStrafe(ParticleReport report) {
			int xRes, yRes;
			imaqGetImageSize(binaryFrame, &xRes, &yRes);
			double midpoint = (report.BoundingRectRight + report.BoundingRectLeft)/(2*xRes) - 1.0;
			double adj = midpoint * computeDistance(report, false);
			SmartDashboard::PutNumber("Tote bounding rect MP (view)", midpoint);
			SmartDashboard::PutNumber("Tote bounding rect MP (dist adj)", adj);
			return adj;
		}

		/**
		 * Computes the estimated distance to a target using the width of the particle in the image. For more information and graphics
		 * showing the math behind this approach see the Vision Processing section of the ScreenStepsLive documentation.
		 *
		 * @param image The image to use for measuring the particle estimated rectangle
		 * @param report The Particle Analysis Report for the particle
		 * @param isLong Boolean indicating if the target is believed to be the long side of a tote
		 * @return The estimated distance to the target in feet.
		 */
		static double computeDistance(ParticleReport report, bool isLong) {
			double normalizedWidth, targetWidth;
			int xRes, yRes;

			imaqGetImageSize(binaryFrame, &xRes, &yRes);
			normalizedWidth = 2*(report.BoundingRectRight - report.BoundingRectLeft)/xRes;
			SmartDashboard::PutNumber("Tote width", normalizedWidth);
			targetWidth = isLong ? 26.9 : 16.9;

			return  targetWidth/(normalizedWidth*12*tan(VIEW_ANGLE*M_PI/360));
		}


	public:
		static double GetAlignmentError() {
			return strafeValue;
		}
		static double GetDistance() {
			return distValue;
		}

		static void Init() {
			// Initialize constants
			TOTE_HUE_RANGE = {24, 49};	//Default hue range for yellow tote
			TOTE_SAT_RANGE = {67, 255};	//Default saturation range for yellow tote
			TOTE_VAL_RANGE = {49, 255};	//Default value range for yellow tote
			AREA_MINIMUM = 2;  //Default Area minimum for particle as a percentage of total image area
			LONG_RATIO = 2.22; //Tote long side = 26.9 / Tote height = 12.1 = 2.22
			SHORT_RATIO = 1.4; //Tote short side = 16.9 / Tote height = 12.1 = 1.4
			SCORE_MIN = 75;  //Minimum score to be considered a tote
			VIEW_ANGLE = 60; //View angle for camera set to 49.4 for Axis m1011 by default, 64 for m1013, 51.7 for 206, 52 for HD3000 square, 60 for HD3000 640x480
			filterOptions = {0, 0, 1, 1};


			binaryFrame = imaqCreateImage(IMAQ_IMAGE_U8, 0);

			//Put default values to SmartDashboard so fields will appear
		#ifdef SD_TEST
			SmartDashboard::PutNumber("Score min", SCORE_MIN);
			SmartDashboard::PutNumber("Tote hue min", TOTE_HUE_RANGE.minValue);
			SmartDashboard::PutNumber("Tote hue max", TOTE_HUE_RANGE.maxValue);
			SmartDashboard::PutNumber("Tote sat min", TOTE_SAT_RANGE.minValue);
			SmartDashboard::PutNumber("Tote sat max", TOTE_SAT_RANGE.maxValue);
			SmartDashboard::PutNumber("Tote val min", TOTE_VAL_RANGE.minValue);
			SmartDashboard::PutNumber("Tote val max", TOTE_VAL_RANGE.maxValue);
			SmartDashboard::PutNumber("Area min %", AREA_MINIMUM);
		#endif
		}

		static Image* GetFrame() {
			return binaryFrame;
		}

		static void RegisterFrame(Image* frame) {
			//Update threshold values from SmartDashboard. For performance reasons it is recommended to remove this after calibration is finished.
		#ifdef SD_TEST
			SCORE_MIN = SmartDashboard::GetNumber("Score min", SCORE_MIN);
			TOTE_HUE_RANGE.minValue = SmartDashboard::GetNumber("Tote hue min", TOTE_HUE_RANGE.minValue);
			TOTE_HUE_RANGE.maxValue = SmartDashboard::GetNumber("Tote hue max", TOTE_HUE_RANGE.maxValue);
			TOTE_SAT_RANGE.minValue = SmartDashboard::GetNumber("Tote sat min", TOTE_SAT_RANGE.minValue);
			TOTE_SAT_RANGE.maxValue = SmartDashboard::GetNumber("Tote sat max", TOTE_SAT_RANGE.maxValue);
			TOTE_VAL_RANGE.minValue = SmartDashboard::GetNumber("Tote val min", TOTE_VAL_RANGE.minValue);
			TOTE_VAL_RANGE.maxValue = SmartDashboard::GetNumber("Tote val max", TOTE_VAL_RANGE.maxValue);

			float areaMin = SmartDashboard::GetNumber("Area min %", AREA_MINIMUM);
		#endif
			// Threshold the image looking for yellow (tote color)
			// FFR this is where frame is set.
			imaqError = imaqColorThreshold(binaryFrame, frame, 255, IMAQ_HSV, &TOTE_HUE_RANGE, &TOTE_SAT_RANGE, &TOTE_VAL_RANGE);

			//Send particle count to dashboard
			int numParticles = 0;
			imaqError = imaqCountParticles(binaryFrame, 1, &numParticles);
			SmartDashboard::PutNumber("Masked particles", numParticles);

			//filter out small particles
			criteria[0] = {IMAQ_MT_AREA_BY_IMAGE_AREA, areaMin, 100, false, false};
			imaqError = imaqParticleFilter4(binaryFrame, binaryFrame, criteria, 1, &filterOptions, NULL, NULL);

			//Send particle count after filtering to dashboard
			imaqError = imaqCountParticles(binaryFrame, 1, &numParticles);
			SmartDashboard::PutNumber("Filtered particles", numParticles);

			if(numParticles > 0) {
				//Measure particles and sort by particle size
				std::vector<ParticleReport> particles;
				for(uint16_t particleIndex = 0; particleIndex < numParticles; ++particleIndex) {
					ParticleReport par;
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_AREA_BY_IMAGE_AREA, &(par.PercentAreaToImageArea));
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_AREA, &(par.Area));
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_CONVEX_HULL_AREA, &(par.ConvexHullArea));
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_BOUNDING_RECT_TOP, &(par.BoundingRectTop));
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_BOUNDING_RECT_LEFT, &(par.BoundingRectLeft));
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_BOUNDING_RECT_BOTTOM, &(par.BoundingRectBottom));
					imaqMeasureParticle(binaryFrame, particleIndex, 0, IMAQ_MT_BOUNDING_RECT_RIGHT, &(par.BoundingRectRight));
					particles.push_back(par);
				}
				sort(particles.begin(), particles.end(), CompareParticleSizes);

				ParticleReport poi; // Particle of interest
				bool isTote = false;

				for (auto it = particles.begin(); it != particles.end(); ++it) {
					scores.Trapezoid = TrapezoidScore(poi);
					SmartDashboard::PutNumber("Trapezoid", scores.Trapezoid);
					scores.LongAspect = LongSideScore(poi);
					SmartDashboard::PutNumber("Long Aspect", scores.LongAspect);
					scores.ShortAspect = ShortSideScore(poi);
					SmartDashboard::PutNumber("Short Aspect", scores.ShortAspect);
					scores.AreaToConvexHullArea = ConvexHullAreaScore(poi);
					SmartDashboard::PutNumber("Convex Hull Area", scores.AreaToConvexHullArea);
					isTote = scores.Trapezoid > SCORE_MIN && scores.ShortAspect > SCORE_MIN && scores.AreaToConvexHullArea > SCORE_MIN;
					if (scores.LongAspect < scores.ShortAspect) {
						poi = *it;
						isTote = true;
					}
				}
				seesTote = isTote;
				SmartDashboard::PutBoolean("IsTote", isTote);
				if (isTote) {
					strafeValue = computeStrafe(poi);
					distValue = computeDistance(poi, false);
					SmartDashboard::PutNumber("Distance to tote", strafeValue);
					SmartDashboard::PutNumber("Tote offset from center", distValue);
					return;
				}
			} else
				SmartDashboard::PutBoolean("IsTote", false);
			strafeValue = 0.0;
			distValue = 0.0;
		}
	};
}

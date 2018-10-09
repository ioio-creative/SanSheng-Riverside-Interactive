//
//  KinectToScreenMapper.h
//  Calibrates Kinect.CameraSpacePoints to display coordinates
//
//  Created by hugoymh on 5/10/2018.
//
#ifndef KinectToFloorScreenMapper_h
#define KinectToFloorScreenMapper_h
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

class KinectToFloorScreenMapper {
	public:

		KinectToFloorScreenMapper(int width, int height);

		//kinect 3D to 2D calibration

		ofParameter<glm::vec2> kinectFourCorners[4] =
		{
			glm::vec2(0, 0),
			glm::vec2(10, 0),
			glm::vec2(10, 10),
			glm::vec2(0, 10)
		};

		const cv::Point2f canvasFourCorners[4] = {
				cv::Point2f(100, 100),
				cv::Point2f(canvasWidth - 100, 100),
				cv::Point2f(canvasWidth - 100, canvasHeight - 100),
				cv::Point2f(100, canvasHeight - 100)
		};
		
		cv::Mat camToScreenTransform;
		void refreshCamToScreenTransform();
		

		ofParameterGroup cornersGroup;
		ofxButton cornerBtn0;
		ofxButton cornerBtn1;
		ofxButton cornerBtn2;
		ofxButton cornerBtn3;
		void refBodyIdxChanged(int& idx);
		void corner0ButtonPressed();
		void corner1ButtonPressed();
		void corner2ButtonPressed();
		void corner3ButtonPressed();

		void setupCalibrationGui(ofxPanel &GUI);
		void updateGuiInspectorValues();
		
		//Fbo
		ofFbo CanvasCalibrateFbo;
		void setupCavasCalibrateFbo();

	private:
		int canvasWidth, canvasHeight;
		ofImage target;
};

#endif // !KinectToScreenMapper
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

		KinectToFloorScreenMapper(const KinectToFloorScreenMapper& mapper);

		~KinectToFloorScreenMapper();
		//kinect 3D to 2D calibration

		ofParameter<glm::vec2> kinectFourCorners[4] =
		{
			glm::vec2(0, 0),
			glm::vec2(10, 0),
			glm::vec2(10, 10),
			glm::vec2(0, 10)
		};

		cv::Point2f canvasFourCorners[4];
				
		cv::Mat GetTransform() const;
		void refreshCamToScreenTransform();
		
		//ofxGui
		ofParameterGroup cornersGroup;
		ofxButton cornerBtn0;
		ofxButton cornerBtn1;
		ofxButton cornerBtn2;
		ofxButton cornerBtn3;
		void updateRefBodyPosOnFloor(const cv::Point2f &bodyPos);

		//ofxGui listeners
		void corner0ButtonPressed();
		void corner1ButtonPressed();
		void corner2ButtonPressed();
		void corner3ButtonPressed();

		void setupCalibrationParamGroup(ofxPanel &GUI);
		
		//Fbo
		ofFbo CanvasCalibrateFbo;
		void setupCavasCalibrateFbo();

	private:
		int canvasWidth, canvasHeight;
		cv::Mat camToScreenTransform;

		const cv::Point2f *refBodyPosOnFloor;
		ofImage target;
};

#endif // !KinectToScreenMapper
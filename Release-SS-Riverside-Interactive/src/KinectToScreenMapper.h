//
//  KinectToScreenMapper.h
//  Calibrates Kinect.CameraSpacePoints to display coordinates
//
//  Created by hugoymh on 5/10/2018.
//
#define KinectToScreenMapper_h
#ifndef KinectToScreenMapper_h
#define KinectToScreenMapper_h

class KinectToScreenMapper {
	public:

		KinectToScreenMapper() {

		};
		//kinect 3D to 2D calibration
		ofVec2f asdf;

		ofParameter<glm::vec2> kinectFourCorners[4] =
		{
			glm::vec2(0, 0),
			glm::vec2(10, 0),
			glm::vec2(10, 10),
			glm::vec2(0, 10)
		};

		const cv::Point2f canvasFourCorners[4] = {
				cv::Point2f(100, 100),
				cv::Point2f(KINECTAREA_WIDTH - 100, 100),
				cv::Point2f(KINECTAREA_WIDTH - 100, KINECTAREA_HEIGHT - 100),
				cv::Point2f(100, KINECTAREA_HEIGHT - 100)
		};

		cv::Point2f[] getCanvasCorners() {
			return canvasFourCorners;
		};
		
		cv::Mat camToScreenTransform;
		void refreshCamToScreenTransform();

		void setupCalibrationGui();
		void updateGuiInspectorValues();
		
		//Fbo
		ofFbo CanvasCalibrateFbo;
		void setupCavasCalibrateFbo();
};

#endif // !KinectToScreenMapper
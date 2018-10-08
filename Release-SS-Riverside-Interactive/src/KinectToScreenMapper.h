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
		void refreshCamToScreenTransform() {
			cv::Point2f k4C[] = {
				cv::Point2f(kinectFourCorners[0]->x, kinectFourCorners[0]->y),
				cv::Point2f(kinectFourCorners[1]->x, kinectFourCorners[1]->y),
				cv::Point2f(kinectFourCorners[2]->x, kinectFourCorners[2]->y),
				cv::Point2f(kinectFourCorners[3]->x, kinectFourCorners[3]->y)
			};
			camToScreenTransform = cv::getPerspectiveTransform(k4C, canvasFourCorners); 
		};

		void setupCalibrationGui();
		void updateGuiInspectorValues();
		
};

#endif // !KinectToScreenMapper
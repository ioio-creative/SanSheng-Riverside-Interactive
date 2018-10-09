#include "KinectToScreenMapper.h"

void KinectToScreenMapper::refreshCamToScreenTransform() {
	cv::Point2f k4C[] = {
				cv::Point2f(kinectFourCorners[0]->x, kinectFourCorners[0]->y),
				cv::Point2f(kinectFourCorners[1]->x, kinectFourCorners[1]->y),
				cv::Point2f(kinectFourCorners[2]->x, kinectFourCorners[2]->y),
				cv::Point2f(kinectFourCorners[3]->x, kinectFourCorners[3]->y)
	};
	camToScreenTransform = cv::getPerspectiveTransform(k4C, canvasFourCorners);
}
#include "KinectToFloorScreenMapper.h"

KinectToFloorScreenMapper::KinectToFloorScreenMapper(int width, int height) {
	canvasWidth = width;
	canvasHeight = height;
	CanvasCalibrateFbo.allocate(canvasWidth, canvasHeight, GL_RGBA);
}

void KinectToFloorScreenMapper::setupCavasCalibrateFbo() {
	target.loadImage("images/target.png");
	target.resize(128, 128);
	target.setAnchorPercent(0.5, 0.5);
	CanvasCalibrateFbo.begin();
	ofClear(ofColor::slateGrey);
	for (auto canvasCorner : canvasFourCorners)
	{
		target.draw(canvasCorner.x, canvasCorner.y);
	}
	CanvasCalibrateFbo.end();
}

void KinectToFloorScreenMapper::refreshCamToScreenTransform() {
	cv::Point2f k4C[] = {
				cv::Point2f(kinectFourCorners[0]->x, kinectFourCorners[0]->y),
				cv::Point2f(kinectFourCorners[1]->x, kinectFourCorners[1]->y),
				cv::Point2f(kinectFourCorners[2]->x, kinectFourCorners[2]->y),
				cv::Point2f(kinectFourCorners[3]->x, kinectFourCorners[3]->y)
	};
	camToScreenTransform = cv::getPerspectiveTransform(k4C, canvasFourCorners);
}
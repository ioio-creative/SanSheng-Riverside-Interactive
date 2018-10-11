#include "KinectToFloorScreenMapper.h"

KinectToFloorScreenMapper::KinectToFloorScreenMapper(int width, int height) {
	canvasWidth = width;
	canvasHeight = height;
	canvasFourCorners[0] = cv::Point2f(100, 100);
	canvasFourCorners[1] = cv::Point2f(canvasWidth - 100, 100);
	canvasFourCorners[2] = cv::Point2f(canvasWidth - 100, canvasHeight - 100);
	canvasFourCorners[3] = cv::Point2f(100, canvasHeight - 100);

}

cv::Mat KinectToFloorScreenMapper::GetTransform() const {
	return camToScreenTransform;
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

void KinectToFloorScreenMapper::setupCavasCalibrateFbo() {
	target.loadImage("images/target.png");
	target.resize(128, 128);
	target.setAnchorPercent(0.5, 0.5);

	CanvasCalibrateFbo.allocate(canvasWidth, canvasHeight, GL_RGBA);
	CanvasCalibrateFbo.begin();
	ofClear(ofColor::slateGrey);
	for (auto canvasCorner : canvasFourCorners)
	{
		target.draw(canvasCorner.x, canvasCorner.y);
	}
	CanvasCalibrateFbo.end();
}

void KinectToFloorScreenMapper::setupCalibrationParamGroup(ofxPanel &GUI) {
	cornersGroup.setName("Corners");
	for (int i = 0; i < 4; i++)
	{
		cornersGroup.add(kinectFourCorners[i].set("Corner#" + ofToString(i), kinectFourCorners[i]));
	}
	GUI.add(cornersGroup);
	GUI.loadFromFile("settings.xml");

	cornerBtn0.addListener(this, &KinectToFloorScreenMapper::corner0ButtonPressed);
	cornerBtn1.addListener(this, &KinectToFloorScreenMapper::corner1ButtonPressed);
	cornerBtn2.addListener(this, &KinectToFloorScreenMapper::corner2ButtonPressed);
	cornerBtn3.addListener(this, &KinectToFloorScreenMapper::corner3ButtonPressed);
	GUI.add(cornerBtn0.setup("Top Left: " + ofToString(kinectFourCorners[0])));
	GUI.add(cornerBtn1.setup("Top Right: " + ofToString(kinectFourCorners[1])));
	GUI.add(cornerBtn2.setup("Bottom Right: " + ofToString(kinectFourCorners[2])));
	GUI.add(cornerBtn3.setup("Bottom Left: " + ofToString(kinectFourCorners[3])));
}


void KinectToFloorScreenMapper::updateRefBodyPosOnFloor(const cv::Point2f &bodyPos) {
	refBodyPosOnFloor = &bodyPos;
}

void KinectToFloorScreenMapper::corner0ButtonPressed() {
	kinectFourCorners[0] = glm::vec2(refBodyPosOnFloor->x, refBodyPosOnFloor->y);
	cornerBtn0.setName("Top Left: " + ofToString(kinectFourCorners[0]));
	refreshCamToScreenTransform();
}

void KinectToFloorScreenMapper::corner1ButtonPressed() {
	kinectFourCorners[1] = glm::vec2(refBodyPosOnFloor->x, refBodyPosOnFloor->y);
	cornerBtn1.setName("Top Right: " + ofToString(kinectFourCorners[1]));
	refreshCamToScreenTransform();
}

void KinectToFloorScreenMapper::corner2ButtonPressed() {
	kinectFourCorners[2] = glm::vec2(refBodyPosOnFloor->x, refBodyPosOnFloor->y);
	cornerBtn2.setName("Bottom Right: " + ofToString(kinectFourCorners[2]));
	refreshCamToScreenTransform();
}

void KinectToFloorScreenMapper::corner3ButtonPressed() {
	kinectFourCorners[3] = glm::vec2(refBodyPosOnFloor->x, refBodyPosOnFloor->y);
	cornerBtn3.setName("Bottom Left: " + ofToString(kinectFourCorners[3]));
	refreshCamToScreenTransform();
}

KinectToFloorScreenMapper::~KinectToFloorScreenMapper() {
	CanvasCalibrateFbo.destroy();
	target.clear();
}


#include "KinectManager.h"

KinectManager::KinectManager(int width, int height, int maxPlayers) {
	monitorWidth = width;
	monitorHeight = height;
	players = maxPlayers;
	bHaveAllStreams = false;

	bodyIdxTracked.resize(players);
	bodyPositions.resize(players);
	bodyPosOnFloor.resize(players);
	bodyPosOnScreen.resize(players);
}

void KinectManager::setup() {
	kinect.open();
	kinect.initDepthSource();
	kinect.initColorSource();
	kinect.initInfraredSource();
	kinect.initBodySource();
	kinect.initBodyIndexSource();

	if (kinect.getSensor()->get_CoordinateMapper(&coordinateMapper) < 0) {
		ofLogError() << "Could not acquire CoordinateMapper!";
	}
	
	Kinect3dCamFbo.allocate(monitorWidth, monitorHeight, GL_RGBA);
	Kinect3dCamFbo.begin();
	ofClear(ofColor::black);
	Kinect3dCamFbo.end();
	kinect3dCam.setControlArea(ofRectangle(0, KINECT3DVIEW_VERTICALDRAWOFFSET - monitorHeight / 2, monitorWidth, monitorHeight));
}

void KinectManager::update(KinectToFloorScreenMapper& floorMapper, const int refBodyIdx) {

	kinect.update();
	floorPlane = kinect.getBodySource()->getFloorClipPlane();
	string floorMsg = "[" + to_string(floorPlane.x) + "][" + to_string(floorPlane.y) + "][" + to_string(floorPlane.z) + "][" + to_string(floorPlane.w) + "]";
	KINECTNOTICELOG << "Floor Plane Vector: " << floorMsg << endl;
	floorTransform = glm::mat4(kinect.getBodySource()->getFloorTransform());
	tiltAngle = atan(floorPlane.z / floorPlane.y);
	rollAngle = atan(floorPlane.x / floorPlane.y);

	// Make sure there's some data here, otherwise the cam probably isn't ready yet
	auto& bodySrc = kinect.getBodySource();
	if (bodySrc->isFrameNew() && !bodySrc->getBodies().size()) {
		bHaveAllStreams = false;
		return;
	}
	else {
		bHaveAllStreams = true;
	}

	// Count number of tracked bodies, and clear bool flags
	numBodiesTracked = 0;
	for (int i = 0; i < bodyIdxTracked.size(); i++)
	{
		bodyIdxTracked[i] = false;
	}

	auto& bodies = bodySrc->getBodies();
	KINECTNOTICELOG << "tracked body ID: ";
	for (auto& body : bodies) {
		if (body.tracked) {
			numBodiesTracked++;
			int bodyIdx = body.bodyId;
			bodyIdxTracked[bodyIdx] = true;
			ofVec3f refJoint = body.joints.at(REFJOINTTYPE).getPosition();
			bodyPositions[bodyIdx] = glm::vec3(refJoint.x, refJoint.y, refJoint.z);
			ofLogNotice() << "[" << bodyIdx << "]";

			glm::vec3& torsoJoint = bodyPositions[bodyIdx];
			glm::vec3 bodyOnFloor = projectedPointOntoPlane(bodyPositions[bodyIdx], floorPlane);
			glm::vec4 bodyPosVec4 = glm::vec4(bodyOnFloor.x, bodyOnFloor.y, bodyOnFloor.z, 1);
			glm::vec4 bodyPosOnHorizonOffset = floorTransform * bodyPosVec4;
			bodyPosOnFloor[bodyIdx] = cv::Point2f(bodyPosOnHorizonOffset.x, bodyPosOnHorizonOffset.z);
		}
	}
	floorMapper.updateRefBodyPosOnFloor(bodyPosOnFloor[refBodyIdx]);

	cv::perspectiveTransform(bodyPosOnFloor, bodyPosOnScreen, floorMapper.GetTransform());
	colorTex = kinect.getColorSource()->getTexture();
}

void KinectManager::draw() {
	Kinect3dCamFbo.begin();
	ofClear(ofColor::white);
	ofBackground(0);
	kinect3dCam.begin();

	ofPushMatrix();
	ofDrawGrid(20, 20, true, true, true, true);
	ofScale(50, 50, 50);

	ofPushMatrix();
	glm::vec3 originOnFloor = projectedPointOntoPlane(glm::vec3(), floorPlane);
	ofTranslate(originOnFloor);
	ofRotateXDeg(90);
	ofRotateXRad(tiltAngle);
	ofRotateZRad(rollAngle);
	ofSetColor(ofColor::violet);
	ofDrawPlane(0, 0, 0, 5, 5);
	ofPopMatrix();

	//draw origin projected on floor
	//ofSetColor(0);
	//ofFill();
	//ofDrawSphere(originOnFloor, 0.1);

	for (auto& body : kinect.getBodySource()->getBodies()) {
		int bodyIdx = int(body.bodyId);
		if (body.tracked)
		{
			ofSetColor(ofColor::blue);
			ofFill();
			for (auto& joint : body.joints)
			{
				//ofVec2f jointDepthPos = joint.second.getPositionInDepthMap();
				ofVec3f jointPos = joint.second.getPositionInWorld();
				ofDrawSphere(jointPos.x, jointPos.y, jointPos.z, 0.02);
			}

			ofSetColor(255, 50, 50);
			ofFill();

			glm::vec3& torsoJoint = bodyPositions[bodyIdx];
			glm::vec3 bodyOnFloor = projectedPointOntoPlane(torsoJoint, floorPlane);
			glm::vec4 bodyPosVec4 = glm::vec4(bodyOnFloor.x, bodyOnFloor.y, bodyOnFloor.z, 1);
			glm::vec4 bodyPosOnHorizonOffset = floorTransform * bodyPosVec4;
			bodyPosOnFloor[bodyIdx] = cv::Point2f(bodyPosOnHorizonOffset.x, bodyPosOnHorizonOffset.z);
			//bodyPosOnScreen[bodyIdx] = glm::vec2(bodyPosOnHorizonOffset.x, bodyPosOnHorizonOffset.z);


			//Red Box drawn along 3d floor plane
			ofDrawBox(bodyOnFloor, 0.2);
			//Blue Box drawn along horizontal plane, projected from RedBox
			ofSetColor(ofColor::aqua);
			//ofDrawBox(bodyPosOnScreen[bodyIdx].x, 0, bodyPosOnScreen[bodyIdx].y, 0.15);
			//Gold Box drawn from mat4 multiply Position on Floor
			ofSetColor(ofColor::gold);
			ofNoFill();
			ofDrawBox(glm::vec3(bodyPosOnHorizonOffset.x, bodyPosOnHorizonOffset.y, bodyPosOnHorizonOffset.z), 0.25);
		}
	}

	ofPopMatrix();
	kinect3dCam.end();
	Kinect3dCamFbo.end();


	ofPushMatrix();
	ofTranslate(0, KINECT3DVIEW_VERTICALDRAWOFFSET - Kinect3dCamFbo.getHeight() / 2);
	Kinect3dCamFbo.draw(0, 0);
	ofPopMatrix();
}

glm::vec3 KinectManager::projectedPointOntoPlane(glm::vec3& point, const Vector4& plane) const {
	glm::vec3 n = glm::vec3(plane.x, plane.y, plane.z);
	return (point - (glm::dot(n, point) + plane.w) * n);
}

KinectManager::~KinectManager()
{
	colorTex.clear();
	Kinect3dCamFbo.destroy();
}

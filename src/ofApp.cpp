#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	//ofSetLogLevel(OF_LOG_VERBOSE);

	ofLog() << "GL Version" << glGetString(GL_VERSION);
	ofLog() << "Maximum number of output vertices support is: " << shader.getGeometryMaxOutputCount();

	ofSoundStreamSetup(0, 1, this, 44100, beat.getBufferSize(), 4);

	ofLogNotice(__FUNCTION__) << "Found " << ofxAzureKinect::Device::getInstalledCount() << " installed devices.";

	// Open Kinect.
	auto kinectSettings = ofxAzureKinect::DeviceSettings();
	kinectSettings.updateIr = false;
	kinectSettings.updateColor = true;
	kinectSettings.colorResolution = K4A_COLOR_RESOLUTION_1080P;
	kinectSettings.updateVbo = false;
	if (this->kinectDevice.open(kinectSettings))
	{
		this->kinectDevice.startCameras();
	}

	// Load shader.
	auto shaderSettings = ofShaderSettings();
	shaderSettings.shaderFiles[GL_VERTEX_SHADER] = "shaders/render.vert";
	shaderSettings.shaderFiles[GL_GEOMETRY_SHADER] = "shaders/render.geom";
	shaderSettings.shaderFiles[GL_FRAGMENT_SHADER] = "shaders/render.frag";
	shaderSettings.bindDefaults = true;
	if (this->shader.setup(shaderSettings))
	{
		ofLogNotice(__FUNCTION__) << "Success loading shader!";
	}

	// Setup vbo.
	std::vector<glm::vec3> verts(1);
	this->vbo.setVertexData(verts.data(), verts.size(), GL_STATIC_DRAW);

	this->pointSize = 3.0f;
	this->useColorSpace = false;

	panel.setup("", "settings.xml", 10, 100);
	panel.add(pointCloudSteps.setup("pointCloudSteps", 25, 1, 100));
	panel.add(thickness.setup("thickness", 1, 1, 50));
	panel.add(jitter.setup("jitter", 1, -100, 100));
	//panel.add(pointSize.setup("pointSize", 1, 1, 10));
	panel.add(connectionDistance.setup("connectionDistance", 250, 1, 500));
	panel.add(pointDepth.setup("pointDepth", 2000, 1, 5000));
	panel.add(doShader.setup("shader?", true));
	panel.add(showKinectDebug.setup("showKinectDebug?", false));
	panel.add(orbitInc.setup("orbit speed", 0.1, -2, 2));
	panel.add(distance.setup("cam distance", 1000, 0, 10000));
	panel.add(blurAmnt.setup("blurAmnt", 100, -500, 500));

	panel.add(xDistortRadius.setup("xDistortRadius", 1, -2.50, 2.50));
	panel.add(yDistortRadius.setup("yDistortRadius", 1, -2.50, 2.50));
	panel.add(zDistortRadius.setup("zDistortRadius", 1, -2.50, 2.50));

	panel.loadFromFile("settings.xml");

	cam.setFov(60);
	cam.setNearClip(1);
	cam.setFarClip(20000);

	orbitAngle = rollAngle = 0.0f;
}

//--------------------------------------------------------------
void ofApp::exit()
{
	this->kinectDevice.close();
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofBackground(0);

	if (this->kinectDevice.isStreaming())
	{
		this->cam.begin();
		{
			ofEnableDepthTest();

			ofDrawAxis(100.0f);

			ofPushMatrix();
			{
				ofRotateXDeg(180);

				this->shader.begin();
				{
					this->shader.setUniform1f("uSpriteSize", this->pointSize);

					int numPoints;

					if (this->useColorSpace)
					{
						this->shader.setUniformTexture("uDepthTex", this->kinectDevice.getDepthInColorTex(), 1);
						this->shader.setUniformTexture("uWorldTex", this->kinectDevice.getColorToWorldTex(), 2);
						this->shader.setUniformTexture("uColorTex", this->kinectDevice.getColorTex(), 3);
						this->shader.setUniform2i("uFrameSize", this->kinectDevice.getColorTex().getWidth(), this->kinectDevice.getColorTex().getHeight());

						numPoints = this->kinectDevice.getColorTex().getWidth() * this->kinectDevice.getColorTex().getHeight();
					}
					else
					{
						this->shader.setUniformTexture("uDepthTex", this->kinectDevice.getDepthTex(), 1);
						this->shader.setUniformTexture("uWorldTex", this->kinectDevice.getDepthToWorldTex(), 2);
						this->shader.setUniformTexture("uColorTex", this->kinectDevice.getColorInDepthTex(), 3);
						this->shader.setUniform2i("uFrameSize", this->kinectDevice.getDepthTex().getWidth(), this->kinectDevice.getDepthTex().getHeight());

						numPoints = this->kinectDevice.getDepthTex().getWidth() * this->kinectDevice.getDepthTex().getHeight();
					}

					this->vbo.drawInstanced(GL_POINTS, 0, 1, numPoints);
				}
				this->shader.end();
			}
			ofPopMatrix();
		}
		this->cam.end();
	}

	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate(), 2) + " FPS", 10, 20);

	if (!bHide) {

		//ofDisableDepthTest();
		panel.draw();
		//ofEnableDepthTest();
		
	}
	else {
		//ofEnableDepthTest();
	}
}


vector<ofPoint> ofApp::reducePointCloud(vector<ofPoint> pc, int steps) {

	vector<ofPoint> reducedPc;

	for (int i = 0; i < pc.size(); i += steps) {

		if (pc[i].x != 0 && pc[i].y != 0 && pc[i].z != 0 && pc[i].z < pointDepth * 1.0) {

			reducedPc.push_back(pc[i]);

		}

	}

	return reducedPc;


}

void ofApp::audioReceived(float* input, int bufferSize, int nChannels) {
	beat.audioReceived(input, bufferSize, nChannels);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'h') {
		bHide = !bHide;
	}
	if (key == 's') {

		string ts = ofGetTimestampString();

		string fname = "dreamstate_" + ts + ".ply";
		mesh.save(fname);

		img.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
		img.save("dreamstate_" + ts + ".png");
	}
	if (key == OF_KEY_UP)
	{
		this->pointSize *= 2;
	}
	else if (key == OF_KEY_DOWN)
	{
		this->pointSize /= 2;
	}
	else if (key == ' ')
	{
		this->useColorSpace ^= 1;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

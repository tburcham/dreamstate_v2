#pragma once

#include "ofMain.h"
#include "ofxAzureKinect.h"
#include "ofxGui.h"
#include "ofxBeat.h"

class ofApp
	: public ofBaseApp
{
public:

	void setup();
	void exit();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

private:
	ofxAzureKinect::Device kinectDevice;
	ofEasyCam cam;
	ofVbo vbo;
	ofShader shader;

	bool useColorSpace;

	ofxPanel panel;
	ofxIntSlider pointCloudSteps;
	float pointSize;
	ofxIntSlider pointDepth;

	ofxFloatSlider connectionDistance;
	ofxToggle doShader;
	ofxToggle showKinectDebug;
	ofxFloatSlider thickness;
	ofxFloatSlider jitter;
	ofxFloatSlider orbitInc;
	ofxFloatSlider rollInc;
	ofxFloatSlider distance;
	ofxFloatSlider xDistortRadius;
	ofxFloatSlider yDistortRadius;
	ofxFloatSlider zDistortRadius;
	ofxFloatSlider blurAmnt;

	vector<ofPoint> pointCloud;
	vector<ofPoint> reducePointCloud(vector<ofPoint> pc, int steps);
	vector<ofPoint> reducedPointCloud;

	vector <ofMesh> meshes;
	vector <ofVbo> vbos;

	ofMesh mesh;
	ofImage     img;

	bool bHide;
	float orbitAngle, rollAngle;

	ofxBeat beat;
	void audioReceived(float*, int, int);
};

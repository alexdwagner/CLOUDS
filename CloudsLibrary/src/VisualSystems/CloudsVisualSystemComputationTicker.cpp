
#include "CloudsVisualSystemComputationTicker.h"

void CloudsVisualSystemComputationTicker::setup(){

	
	int buttonWidth = 180;
	int buttonHeight = 30;
	gui = new ofxUICanvas(0,0,buttonWidth,ofGetHeight());
	
	gui->addLabel("animation params");
	gui->addSlider("speed", 0., 2., &speed);
	gui->addSlider("scale", 1., 10., &scale);
	gui->addSlider("point size", 1., 25., &pointSize);
	gui->addSlider("path deviation", 0, 1000, &deviation);
	gui->addLabel("generator params");
	gui->addSlider("path color flip chance", 0, 1.0, &colorflip);
	gui->addSlider("path deviation chance", 0, .2, &pathDeviation);
	
	gui->addLabelButton("regenerate particles", &regenerate);
	
	gui->loadSettings("GUI/guiSettings.xml");
	regenerateParticles();
	
	//ALLOCATE BUFFERS
	fbo.allocate(width, height, GL_RGB);
	speedTexture.allocate(width, 1, OF_IMAGE_COLOR);
	for(int i = 0; i < width; i++){
		speedTexture.setColor(i, 0, ofRandom(10,255) );
	}
	speedTexture.reloadTexture();
	
	sourceOffset.allocate(width, 1, GL_RGB32F);
	targetOffset.allocate(width, 1, GL_RGB32F);
	
	offsetMesh.addVertex(ofVec3f(0,0,0));
	offsetMesh.addVertex(ofVec3f(0,1,0));
	offsetMesh.addVertex(ofVec3f(width,0,0));
	offsetMesh.addVertex(ofVec3f(width,1,0));
	offsetMesh.addTexCoord(ofVec3f(0,0));
	offsetMesh.addTexCoord(ofVec3f(0,1));
	offsetMesh.addTexCoord(ofVec2f(width,0));
	offsetMesh.addTexCoord(ofVec2f(width,1));
	
	offsetMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	
	reloadShaders();

}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::begin(){
	sourceOffset.begin();
	ofClear(0);
	sourceOffset.end();
	
	targetOffset.begin();
	ofClear(0);
	targetOffset.end();
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::end(){
	
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::reloadShaders(){
	
	sourceOffset.begin();
	ofClear(0);
	sourceOffset.end();
	
	targetOffset.begin();
	ofClear(0);
	targetOffset.end();
	
	drawShader.load("display");
	updateShader.load("update");
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::regenerateParticles(){
	
	float currentColorFlip = powf(colorflip,2.0);
	bool one = false;
	debug = false;
	width  = 1280;
	height = 720;
	mesh.clear();
	for(int x = 0; x < width; x++){
		for(int y = 0; y < height; y++){
			mesh.addColor( ofFloatColor( one ? 1 : 0) );
			mesh.addVertex(ofVec3f(x,y,0));
			if(ofRandomuf() > 1.0 - currentColorFlip ) one = !one;
			if(ofRandomuf() > .05) currentColorFlip = ofRandomf() * powf(colorflip, 2.0);
		}
	}
	mesh.setMode(OF_PRIMITIVE_POINTS);
	
	shiftTexture.allocate(width, height, OF_IMAGE_COLOR);
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			if(y == 0){
				shiftTexture.setColor(x, y, ofColor(128));
			}
			else{
				ofColor current = shiftTexture.getColor(x, y-1);
				float random = ofRandomf();
				if(current.r >= 128 && random > 1.0 - pathDeviation){
					shiftTexture.setColor(x, y, current+1);
				}
				else if(current.r <= 128 && random < -(1.0 - pathDeviation)){
					shiftTexture.setColor(x, y, current-1);
				}
				else{
					shiftTexture.setColor(x, y, current);
				}
			}
		}
	}
	
	shiftTexture.update();
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::update(ofEventArgs & args){
	if(regenerate){
		regenerateParticles();
	}
	
	ofPushStyle();
	
	ofDisableAlphaBlending();
	sourceOffset.begin();
	updateShader.begin();
	updateShader.setUniformTexture("image", targetOffset.getTextureReference(), 0);
	updateShader.setUniformTexture("speed", speedTexture.getTextureReference(), 1);
	updateShader.setUniform1f("speedMultiplier", powf(speed, 2.0));
	
	offsetMesh.draw();
	
	updateShader.end();
	sourceOffset.end();
	
	ofPopStyle();
	
	swap(sourceOffset,targetOffset);
}

//--------------------------------------------------------------
void CloudsVisualSystemComputationTicker::draw(ofEventArgs & args){
	
	
	ofRectangle screenRect(200,0,ofGetWidth(), ofGetHeight());
	ofRectangle videoRect(0,0,fbo.getWidth(), fbo.getHeight());
	videoRect.scaleTo(screenRect);
	
	if(debug){
		//speedTexture.getTextureReference().draw(videoRect);
		targetOffset.getTextureReference().draw(videoRect);
		//shiftTexture.draw(videoRect);
	}
	else {
		fbo.begin();
		ofClear(128);

		drawShader.begin();
		drawShader.setUniformTexture("image", targetOffset.getTextureReference(), 0);
		drawShader.setUniformTexture("shift", shiftTexture.getTextureReference(), 1);
		drawShader.setUniform1f("height", height);
		drawShader.setUniform1f("deviation", deviation);
		
		ofPushMatrix();
		ofSetLineWidth(10);
		glPointSize(pointSize);
		float scaleexp = powf(scale,2);
		ofTranslate(ofGetWidth()/2,ofGetHeight()/2 );
		ofScale(scaleexp, scaleexp);
		ofTranslate(-ofGetWidth()/2,-ofGetHeight()/2 );
		
		mesh.draw();
		
		ofPopMatrix();
		
		drawShader.end();
		
		fbo.end();

		
		fbo.getTextureReference().draw(videoRect);
	}
	
	ofPushStyle();
	ofDrawBitmapString(ofToString( ofGetFrameRate(),2 ), ofGetWidth() - 100, 20);
	ofPopStyle();
	
}

void CloudsVisualSystemComputationTicker::mouseDragged(ofMouseEventArgs & args){
	
}
void CloudsVisualSystemComputationTicker::mouseMoved(ofMouseEventArgs & args){
	
}
void CloudsVisualSystemComputationTicker::mousePressed(ofMouseEventArgs & args){
	
}
void CloudsVisualSystemComputationTicker::mouseReleased(ofMouseEventArgs & args){
	
}

void CloudsVisualSystemComputationTicker::keyPressed(ofKeyEventArgs & args){
	if(args.key == 'r'){
		reloadShaders();
	}

	if(args.key == 'D'){
		debug = !debug;
	}

	if(args.key == 'f'){
		ofToggleFullscreen();
	}
}

void CloudsVisualSystemComputationTicker::keyReleased(ofKeyEventArgs & args){
	
}

bool CloudsVisualSystemComputationTicker::isReleventToKeyword(string keyword){
	
}

void CloudsVisualSystemComputationTicker::exit(ofEventArgs & args){
	gui->saveSettings("GUI/guiSettings.xml");
	delete gui;
}
#include "testApp.h"
#include "CloudsGlobal.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(0);
	ofToggleFullscreen();
	
	
    ofEnableAlphaBlending();
	
    parser.loadFromFiles();

	
	if(!ofFile::doesFileExist(getDataPath() + "CloudsMovieDirectory.txt")){
		ofSystemAlertDialog("Could not find movie file path. Create a file called CloudsMovieDirectory.txt that contains one line, the path to your movies folder");
	}
	

	parser.setCombinedVideoDirectory(ofBufferFromFile(getDataPath() + "CloudsMovieDirectory.txt").getText());
	player.setup();
	
	visualSystems.populateVisualSystems();
	vector<CloudsVisualSystemPreset>& presets = visualSystems.getPresetsForSystem("ComputerTicker");
	
	CloudsAct* act = new CloudsAct();
	CloudsClip& clip = parser.getRandomClip(true, false);
	
	
	act->addClip(clip, "topic", 0);
	act->addVisualSystem( presets[ ofRandom(presets.size()) ], 5, 12 );
	act->populateTime();
	
	player.playAct(act);
}

//--------------------------------------------------------------
void testApp::update(){

	ofShowCursor();
	
	//get cam a
	
	//get cam b
	
	//
}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
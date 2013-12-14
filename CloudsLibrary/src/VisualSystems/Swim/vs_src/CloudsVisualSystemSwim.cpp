//
//  CloudsVisualSystemSwim.cpp
//

#include "CloudsVisualSystemSwim.h"
#include "CloudsRGBDVideoPlayer.h"

//#include "CloudsRGBDVideoPlayer.h"
//#ifdef AVF_PLAYER
//#include "ofxAVFVideoPlayer.h"
//#endif

const float CloudsVisualSystemSwim::CAM_DAMPING = .08f;

CloudsVisualSystemSwim::CloudsVisualSystemSwim() : yRot(0), zSpeed(0)
{
}

// selfSetup is called when the visual system is first instantiated
// This will be called during a "loading" screen, so any big images or
// geometry should be loaded here
void CloudsVisualSystemSwim::selfSetup()
{
    ofAddListener(ofEvents().windowResized, this, &CloudsVisualSystemSwim::onWindowResized);
    
    snow.init(getVisualSystemDataPath(), 100000);
    bubbles.init(getVisualSystemDataPath());
    creatures.init(getVisualSystemDataPath());
    
    post.init(ofGetWidth(), ofGetHeight(), true);
    post.createPass<FxaaPass>();
    post.createPass<BloomPass>();
}

// selfBegin is called when the system is ready to be shown
// this is a good time to prepare for transitions
// but try to keep it light weight as to not cause stuttering
void CloudsVisualSystemSwim::selfBegin()
{
    // adding this here as custom gui data is loaded after setup
	creatures.generate();
    snow.generate();
}

//normal update call
void CloudsVisualSystemSwim::selfUpdate()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
    
    // cam
    yRot += CAM_DAMPING * (ofMap(GetCloudsInputX(), 0.f, ofGetWidth(), 20, -20) - yRot);
    zSpeed += CAM_DAMPING * (ofMap(GetCloudsInputY(), 0, ofGetHeight(), -600.f, 600.f) - zSpeed);
    getCameraRef().move(0, 0, zSpeed * ofGetLastFrameTime());
    getCameraRef().setOrientation(ofVec3f(0, yRot, 0.f));
    getCameraRef().setFarClip(Creature::fogEnd);

    //bubbles.update();
    creatures.update();//getCameraRef().getPosition() + 1000.f * getCameraRef().getLookAtDir().normalized());
}

// selfDraw draws in 3D using the default ofEasyCamera
// you can change the camera by returning getCameraRef()
void CloudsVisualSystemSwim::selfDraw()
{
    creatures.draw(getCameraRef());
    //bubbles.draw();
    snow.draw(getCameraRef());
}

void CloudsVisualSystemSwim::selfPostDraw()
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    post.process(CloudsVisualSystem::getSharedRenderTarget(), false);
    post.getProcessedTextureReference().draw(0, ofGetHeight(), ofGetWidth(), -ofGetHeight());
    glPopAttrib();
}

//use render gui for display settings, like changing colors
void CloudsVisualSystemSwim::selfSetupRenderGui()
{    
    rdrGui->addToggle("regenerate", false);
    rdrGui->addMinimalSlider("creatureFogStart", 0.f, 10000.f, &Creature::fogStart);
    rdrGui->addMinimalSlider("creatureFogEnd", 0.f, 10000.f, &Creature::fogEnd);
    rdrGui->addLabel("Flocking");
    rdrGui->addSpacer();
    rdrGui->addMinimalSlider("zoneRadius", 50.f, 2000.f, &creatures.zoneRadius);
    rdrGui->addMinimalSlider("alignmentLower", 0.f, 1.f, &creatures.alignmentLower);
    rdrGui->addMinimalSlider("alignmentUpper", 0.f, 1.f, &creatures.alignmentUpper);
    rdrGui->addMinimalSlider("repelStrength", 0.f, 1.f, &creatures.repelStrength);
    rdrGui->addMinimalSlider("alignStrength", 0.f, 1.f, &creatures.alignStrength);
    rdrGui->addMinimalSlider("attractStrength", 0.f, 1.f, &creatures.attractStrength);
    rdrGui->addMinimalSlider("maxDistFromCentre", 500.f, 4000.f, &creatures.maxDistFromCentre);
    
    rdrGui->addLabel("Points");
    rdrGui->addSpacer();
    rdrGui->addIntSlider("numPointOne", 0, 1000, &creatures.numPointOne);
    rdrGui->addMinimalSlider("huePointOne", 0.f, 1.f, &creatures.huePointOne);
    rdrGui->addIntSlider("numPointTwo", 0, 1000, &creatures.numPointTwo);
    rdrGui->addMinimalSlider("huePointTwo", 0.f, 1.f, &creatures.huePointTwo);
    rdrGui->addIntSlider("numPointThree", 0, 1000, &creatures.numPointThree);
    rdrGui->addMinimalSlider("huePointThree", 0.f, 1.f, &creatures.huePointThree);
    
    rdrGui->addLabel("Jellies (see other menus)");
    rdrGui->addSpacer();
    rdrGui->addIntSlider("numJellyOne", 0, 300, &creatures.numJellyOne);
    rdrGui->addIntSlider("numJellyTwo", 0, 300, &creatures.numJellyTwo);
    
    rdrGui->addLabel("Fish One");
    rdrGui->addSpacer();
    rdrGui->addIntSlider("numGreyFish", 0, 300, &creatures.numGreyFish);
    rdrGui->addMinimalSlider("greySizeAverage", .1f, 3.f, &creatures.fishOneParams.sizeAverage);
    rdrGui->addMinimalSlider("greySizeStdDeviation", 0.f, 1.f, &creatures.fishOneParams.sizeStdDeviation);
    
    rdrGui->addLabel("Fish Two");
    rdrGui->addSpacer();
    rdrGui->addIntSlider("numYellowFish", 0, 300, &creatures.numYellowFish);
    rdrGui->addMinimalSlider("yellowSizeAverage", .1f, 3.f, &creatures.fishTwoParams.sizeAverage);
    rdrGui->addMinimalSlider("yellowSizeStdDeviation", 0.f, 1.f, &creatures.fishTwoParams.sizeStdDeviation);
    
    //rdrGui->addMinimalSlider("fishTexAmt", 0.f, 1.f, &ModelCreature::texAmount);
}

//These methods let us add custom GUI parameters and respond to their events
void CloudsVisualSystemSwim::selfSetupGui()
{
	jellyOneGui = createCustomGui("Jellyus Oneus");
    addSliders(jellyOneGui, creatures.jellyOneParams);
    
    jellyTwoGui = createCustomGui("Jellyus Twous");
    addSliders(jellyTwoGui, creatures.jellyTwoParams);
}

void CloudsVisualSystemSwim::addSliders(ofxUISuperCanvas* gui, JellyParams& params)
{
    gui->addSpacer();
    
    gui->addLabel("Colour");
    gui->addMinimalSlider("body h", 0.f, 1.f, &params.bodyHsb.x);
    gui->addMinimalSlider("body s", 0.f, 1.f, &params.bodyHsb.y);
    gui->addMinimalSlider("body b", 0.f, 1.f, &params.bodyHsb.z);
    
    gui->addMinimalSlider("body alpha", 0.f, 1.f, &params.bodyAlpha);
    
    gui->addMinimalSlider("tentacles h", 0.f, 1.f, &params.tentacleHsb.x);
    gui->addMinimalSlider("tentacles s", 0.f, 1.f, &params.tentacleHsb.y);
    gui->addMinimalSlider("tentacles b", 0.f, 1.f, &params.tentacleHsb.z);
    
    gui->addRangeSlider("pulse amt (range)", 0.f, 0.4f, &params.pulseAmtMin, &params.pulseAmtMax);
    
    gui->addLabel("Size");
    gui->addMinimalSlider("width average", 10, 200, &params.widthAverage);
    gui->addMinimalSlider("width std dev", 0, 200, &params.widthStdDeviation);
    
    gui->addMinimalSlider("length average", 10, 200, &params.lengthAverage);
    gui->addMinimalSlider("length std dev", 0, 200, &params.lengthStdDeviation);
    
    gui->addLabel("Shape");
    gui->addRangeSlider("spherical segment (range)", .5f * HALF_PI, PI, &params.segmentMin, &params.segmentMax);

    gui->addRangeSlider("superformula m1 (range)", 2, 20, &params.m1Min, &params.m1Max);
    gui->addRangeSlider("superformula m2 (range)", 2, 20, &params.m2Min, &params.m2Max);
}

// selfPresetLoaded is called whenever a new preset is triggered
// it'll be called right before selfBegin() and you may wish to
// refresh anything that a preset may offset, such as stored colors or particles
void CloudsVisualSystemSwim::selfPresetLoaded(string presetPath)
{
    creatures.generate();
    snow.generate();
}


void CloudsVisualSystemSwim::onWindowResized(ofResizeEventArgs& args)
{
    post.init(args.width, args.height, true);
}

ofxUISuperCanvas* CloudsVisualSystemSwim::createCustomGui(const string& name)
{
    ofxUISuperCanvas* newGui = new ofxUISuperCanvas(name, gui);
	newGui->copyCanvasStyle(gui);
	newGui->copyCanvasProperties(gui);
	newGui->setName(name);
	newGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    
    guis.push_back(newGui);
	guimap[newGui->getName()] = newGui;
    
    return newGui;
}

void CloudsVisualSystemSwim::guiRenderEvent(ofxUIEventArgs &e)
{
	if (e.widget->getName() == "regenerate")
    {
        ofxUIToggle* toggle = static_cast<ofxUIToggle*>(e.widget);
        if (toggle->getValue())
        {
            snow.generate();
            creatures.generate();
            toggle->setValue(false);
        }
    }
}

void CloudsVisualSystemSwim::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "Custom Button"){
		cout << "Button pressed!" << endl;
	}
}

//Use system gui for global or logical settings, for exmpl
void CloudsVisualSystemSwim::selfSetupSystemGui(){
	
}

void CloudsVisualSystemSwim::guiSystemEvent(ofxUIEventArgs &e){
	
}

//do things like ofRotate/ofTranslate here
//any type of transformation that doesn't have to do with the camera
void CloudsVisualSystemSwim::selfSceneTransformation(){
	
}





// draw any debug stuff here
void CloudsVisualSystemSwim::selfDrawDebug(){
	
}
// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
void CloudsVisualSystemSwim::selfDrawBackground(){

	//turn the background refresh off
	//bClearBackground = false;
	
}
// this is called when your system is no longer drawing.
// Right after this selfUpdate() and selfDraw() won't be called any more
void CloudsVisualSystemSwim::selfEnd(){
	
	simplePointcloud.clear();
	
}
// this is called when you should clear all the memory and delet anything you made in setup
void CloudsVisualSystemSwim::selfExit(){
	
}

//events are called when the system is active
//Feel free to make things interactive for you, and for the user!
void CloudsVisualSystemSwim::selfKeyPressed(ofKeyEventArgs & args){
	
}
void CloudsVisualSystemSwim::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsVisualSystemSwim::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsVisualSystemSwim::selfMouseReleased(ofMouseEventArgs& data){
	
}
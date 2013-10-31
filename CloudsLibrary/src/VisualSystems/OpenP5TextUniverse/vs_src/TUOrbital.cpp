//
//  TUOrbital.cpp
//  OpenP5TextUniverse
//
//  Created by Elie Zananiri on 2013-10-29.
//
//

#include "TUOrbital.h"

float TUOrbital::focusX;
float TUOrbital::focusY;
float TUOrbital::focusZ;

ofxFTGLSimpleLayout TUOrbital::font;
ofxFTGLSimpleLayout TUOrbital::initium;
ofColor TUOrbital::textColor(255);

ofColor TUOrbital::lineColor(255);
float TUOrbital::lineWidth = 1.0f;

ofColor TUOrbital::nodeColor(255);
float TUOrbital::nodeScalar = 1.0f;

//--------------------------------------------------------------
TUOrbital::TUOrbital(float size, float radius)
{
    text = "";
    
    this->size = size;
    this->radius = radius;
    
    bSelected = bClicked = bRenderText = true;
}

//--------------------------------------------------------------
TUOrbital::TUOrbital(TUOrbital& parent, string text)
{
    
    this->text = text;
    
    this->size = parent.size / 3 * 2;
    this->radius = parent.radius / 2;
    
    bSelected = bClicked = bRenderText = true;
}

//--------------------------------------------------------------
void TUOrbital::update(float x, float y, float z, bool bSelected)
{
    pos.x = x;
    pos.y = y;
    pos.z = z;
    
    if (bSelected) {
        this->bSelected = true;
    }
    
    // calculating sphere distribution
    
    // constants
    float dphi = PI * (3 - sqrtf(5));
    float phi = 0;
    float dz = 2.0 / float(children.size());
    float zz = 1 - dz / 2.0;
    
    // child downscaling
    float cRadius = radius / 3;
    
    // determining positions children
    for (int i = 0; i < children.size(); i++) {
        float r = cRadius * sqrtf(1 - zz * zz);
        
        float childX = r * cosf(phi);
        float childY = r * sinf(phi);
        float childZ = cRadius * zz;
        
        zz = zz - dz;
        phi = phi + dphi;
        
        // recursive update
        children[i].update(childX, childY, childZ, bSelected);
    }
}

//--------------------------------------------------------------
void TUOrbital::draw(ofCamera& cam, bool bMouseDragged)
{
    ofPushMatrix();
    {
        ofTranslate(pos);
        
        for (int i = 0; i < children.size(); i++) {
            if (lineWidth > 0) {
                ofSetColor(lineColor);
                ofLine(ofVec3f::zero(), children[i].pos);
            }
            
            // recursive draw
            children[i].draw(cam, bMouseDragged);
        }
        
        billboard();
        ofScale(1, -1, 1);
        
        if (bRenderText && bClicked) {
            ofSetColor(textColor);
            font.drawString(text.substr(1), initium.stringWidth(text.substr(0, 1)), 0);
        }
        
        ofSetColor(nodeColor);
//        ofRect(-(size * nodeScalar) / 2.0f, -(size * nodeScalar) / 2.0f, (size * nodeScalar), (size * nodeScalar));
        initium.drawString(text.substr(0, 1), 0, 0);
        
        if (!bMouseDragged) {
            if (isMouseover(cam)) {
                if (ofGetMousePressed()) {
                    bClicked = true;
                    bSelected = true;
                    
                    GLfloat modelview[16];
                    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
                    focusX = modelview[12];
                    focusY = modelview[13];
                    focusZ = modelview[14];
                }
            }
            else {
                //bSelected = false;
            }
            
            if (ofGetKeyPressed(OF_KEY_RETURN)) {
                bClicked = false;
                bSelected = false;
            }
        }
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void TUOrbital::billboard()
{
	// Get the current modelview matrix.
	float modelview[16];
	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);
    
	// Undo all rotations.
	// Beware all scaling is lost as well.
	for (int i = 0; i < 3; i++) {
		for (int j=0; j < 3; j++) {
			if (i == j) {
				modelview[i * 4 + j] = 1.0;
            }
			else {
				modelview[i * 4 + j] = 0.0;
            }
		}
    }
    
	// Set the modelview with no rotations.
	glLoadMatrixf(modelview);
}

//--------------------------------------------------------------
bool TUOrbital::isMouseover(ofCamera& cam)
{
    ofVec2f topLeft = cam.worldToScreen(ofVec3f(-size / 2.0f, -size / 2.0f, 0));
    ofVec2f bottomRight = cam.worldToScreen(ofVec3f(size / 2.0f, size / 2.0f, 0));
    
    if (ofGetMouseX() > topLeft.x && ofGetMouseX() < bottomRight.x) {
        if (ofGetMouseY() > topLeft.y && ofGetMouseY() < bottomRight.y) {
            return true;
        }
    }
    return false;
}
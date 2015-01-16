//
//  dpScoreSceneBodyPatterns.cpp
//  dpScore
//
//  Created by YoshitoONISHI on 1/10/15.
//
//

#include "dpScoreSceneBodyPatterns.h"

DP_SCORE_NAMESPACE_BEGIN


void SceneBodyPatterns::initialize()
{
    dpDebugFunc();
    
    mUICanvas = new ofxUICanvas();
    mUICanvas->setName(getName());
    mUICanvas->addLabel(getName());
    mUICanvas->addSpacer();
    
    mCam.setDistance(200);
    mCam.disableMouseInput();
}

void SceneBodyPatterns::shutDown()
{
    dpDebugFunc();
    
    if (mUICanvas) {
        delete mUICanvas;
        mUICanvas = nullptr;
    }
}

void SceneBodyPatterns::enter()
{
    dpDebugFunc();
    
    ofAddListener(ofxMotioner::updateSkeletonEvent,
                  this,
                  &SceneBodyPatterns::onUpdateSkeleton);
    mCam.enableMouseInput();
}

void SceneBodyPatterns::exit()
{
    dpDebugFunc();
    
    ofRemoveListener(ofxMotioner::updateSkeletonEvent,
                     this,
                     &SceneBodyPatterns::onUpdateSkeleton);
    mCam.disableMouseInput();
    
    mSkeletons.clear();
}

void SceneBodyPatterns::update(ofxEventMessage& m)
{
    if (m.getAddress() == kOscAddrMotioner) {
        mFrame++;
        if (mFrame%60 == 0)  {
            (++mFocusNode) %= ofxMot::NUM_JOINTS;
        }
    }
}

void SceneBodyPatterns::drawSkeleton(ofxMot::SkeletonPtr skl, int idx)
{
    ofPushMatrix();
    ofPushStyle();
    
    auto& joints = skl->getJoints();
    
    ofNoFill();
    
    for (size_t i=0; i<joints.size(); i++) {
        ofSetLineWidth(1.0f);
        auto& n = skl->getJoint(i);
        n.transformGL();
        const float s = 5.f;
        (n.id == mFocusNode || idx == 0) ? ofSetColor(color::kMain) : ofSetColor(ofColor::white);;
        ofDrawBox(ofVec3f::zero(), s);
        n.restoreTransformGL();
        
        if (!n.getParent()) continue;
    
        ofSetColor(ofColor::white);
        ofLine(n.getGlobalPosition(), n.getParent()->getGlobalPosition());
    }
    
    ofPopStyle();
    ofPopMatrix();
    
}

void SceneBodyPatterns::draw()
{
    const float stepX = aligned(kW/nX);
    const float stepY = aligned((kH-20.f)/nY);
    for (int j=0; j<nY; j++) {
        for (int i=0; i<nX; i++) {
            ofRectangle viewport = alignedRectangle(i*stepX, j*stepY+20.f, stepX, stepY);
            mCam.begin(viewport);
            const int idx = mSkeletons.size() - (i + j * nX) - 1;
            if (idx<mSkeletons.size()) {
                ofPushMatrix();
                ofTranslate(0.f, -70.f);
                drawSkeleton(mSkeletons.at(idx), i + j * nX);
                ofPopMatrix();
            }
            mCam.end();
            ofNoFill();
            ofSetLineWidth(1.f);
            ofSetColor(ofColor::white);
            ofRect(viewport);
        }
    }
    alignedLine(kW-1, 20.f, kW-1, kH);
    alignedLine(0.f, kH-1.f, kW, kH-1.f);
}

#pragma mark ___________________________________________________________________
void SceneBodyPatterns::onUpdateSkeleton(ofxMotioner::EventArgs &e)
{
    auto skl = e.skeleton;
    
    if (mSkeletonName=="") mSkeletonName = skl->getName();
    
    if (mSkeletonName == skl->getName()) {
        if (mFrame % kSkip == 0) {
            auto copy = ofxMot::Skeleton::copy(skl);
            mSkeletons.push_back(copy);
            
            while (mSkeletons.size() > kNumSkeletons) {
                mSkeletons.pop_front();
            }
        }
        else if (mSkeletons.empty() == false) {
            *(mSkeletons.end()-1) = ofxMot::Skeleton::copy(skl);
        }
    }
}

DP_SCORE_NAMESPACE_END
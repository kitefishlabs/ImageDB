//
//  ImageRegion.h
//  ImageCorpus
//
//  Created by Thomas Stoll on 1/15/12.
//  Copyright (c) 2012 Kitefish Labs. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

#define COLOR_BINS 16


class ImageHistogram {
public:
    void setup();
    void reset();
    

    IplImage                *histIPL;
    CvHistogram             *hist;
    int                     histArray[64];
    
    ofxCvColorImage         histViz;
    
    bool                    bHistogramCalculated;
    int                     histMax;
    
    
    
    void calcHistogram(IplImage *source);
    void injectHistogram(int* data);
    void displayHistogram(int offsetX, int offsetY, int iplHeight);
    string writeOutHistogram();
};

class ImageRegion {
public:
    bool                    prefab;
    int                     regionID;
    CvSize                  size;
    CvPoint                 anchor, centroid;
    float                   area, ratio;
    ofColor                 distribution[3*16];
    ofxCvColorImageAlpha    regionImage;
    
    ofxCvBlob               contour;
    ofFbo                   fbo;
    bool                    bFBOAllocated;
    
    ImageHistogram          regionHistogram;

};

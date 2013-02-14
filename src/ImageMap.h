//
//  ImageMap.h
//  ImageCorpus
//
//  Created by Thomas Stoll on 1/15/12.
//  Copyright (c) 2012 Kitefish Labs. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ImageRegion.h"

#define N_REGIONS 16


class ImageMap {

public:
    
    bool                    prefab;
    int                     size, width, height, analysisThreshold;
    string                  imagePath, imageFileBase, imageExt;
    int                     imageID, numberOfRegions, currentRegion;

    map<int, ImageRegion>   mappedRegions;
    
    
//    ofImage                 ofSrcImage;               // temp var!
    ofxCvColorImage         cvSrcImage; //, cvDS2Image;
//    IplImage                *iplSrcImage;
    
//    ofxCvGrayscaleImage     cvGrayImage;
    
    ImageHistogram          ds2ImageHistogram;
//    ofxCvContourFinder      contourFinder;
//    
//    ofxCvColorImage         cvCurrentROI;
//    ofxCvColorImageAlpha    cvCurrentROIWithAlpha;
//    ofxCvGrayscaleImage     cvCurrentContourMask;    
//    IplImage                *currentROIIPL;
//    IplImage                *currentROIWithAlphaIPL;
//    IplImage                *currentContourMaskIPL;

    
    void setup(int threshold);
    
    void setupDownsampleThresholdAnalyze(int id, string imageFile);
    void findContourMask(int rnum, bool bSaveFlag);
    
    void downSample2(IplImage* srcIPL, IplImage* destIPL);
    
    void writeAnalysis(int regionIndex);
    
    void constructPrefabRegion(string name, int currentIndex, CvPoint anchor, CvSize size, float area, float ratio, CvPoint centroid, int histmax);
};

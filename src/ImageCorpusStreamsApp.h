//
//  ImageCorpusStreamsApp.h
//  ImageCorpus
//
//  Created by Thomas Stoll on 1/23/12.
//  Copyright (c) 2012 Kitefish Labs. All rights reserved.
//

#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"

#include "ImageMap.h"
#include "ImageRegion.h"
#include "IRKDTree.h"

class ImageCorpusStreamsApp : public ofBaseApp {
    
public:
    
    map<int, string>            mappedFileNames, toBeAnalyzedFileNames;
    map<int, int>               mappedThresholds, mappedRegionCounts;
    int                         totalNumImages, totalNumBlobs, gStandardThreshold, currentImage;
    
    map<int, ImageMap>          mappedImages;
    
    map<int, pair<int, int> >   mappedIndexes;
    
    
    IRKDTree                    tree;
    int                         tCounter;
    bool                        bTreeSearchable;
    vector<int>                 result;
    
    int                         *mapped;
    
    void setup();
    void update();
    void draw();
    
    void importCorpusFromDirectory(string dirName);
    
    void constructCorpusTree();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    
    
    
};

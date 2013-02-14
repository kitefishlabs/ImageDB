#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"

#include "ImageMap.h"
#include "ImageRegion.h"
#include "IRKDTree.h"

class imageCorpusAnalysisApp : public ofBaseApp {

public:

    map<int, string>            mappedFileNames, toBeAnalyzedFileNames;
    map<int, int>               mappedThresholds;
    map<int, vector<int> >      mappedBlobIDs;
    int                         totalNumImages, totalNumBlobs, gStandardThreshold, currentImageNum;
    int                         totalNumImagesToBeAnalyzed;
    
    map<int, ImageMap>          mappedImages;
    
    
    IRKDTree        tree;
    int             tCounter;
    
    
    void setup();
    void update();
    void draw();
    
    void analyzeDirectory(string dirName);
    
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


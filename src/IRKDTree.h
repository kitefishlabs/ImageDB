//
//  IRKDTree.h
//  ImageCorpus
//
//  Created by Thomas Stoll on 1/15/12.
//  Copyright (c) 2012 Kitefish Labs. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

class IRKDTree {
    
public:
    cv::Mat                         searchPoints;
    int                             knnValue;
    
    cv::flann::Index_<float>        *index;
//    IMageHistogram                  searchHist;
    
    int                             currentIndex;
    int                             width;
    
    
    void setup(int k);
    
    void initAndUploadData(float *data, int rows);
    void searchForPoint(int searchIndex, std::vector<int> *res);
    
    void clearIndex();
    
//    void draw();
    
};

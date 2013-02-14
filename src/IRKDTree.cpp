//
//  IRKDTree.cpp
//  ImageCorpus
//
//  Created by Thomas Stoll on 1/15/12.
//  Copyright (c) 2012 Kitefish Labs. All rights reserved.
//

#include "IRKDTree.h"


void IRKDTree::setup(int k) {
    // sensible defaults
    knnValue = k;
    width = 64;
    currentIndex = -1;
//    cout << searchPoints.size[0] << "|" << searchPoints.size[1] << endl;
    
}

void IRKDTree::initAndUploadData(float *data, int rows) {
    
    clearIndex();
    

    searchPoints = cv::Mat(rows, width, CV_32F, data); // cols = 64
//    cout << "SIZE: " << searchPoints.size[0] << "|" << searchPoints.size[1] << endl;
        
    // searchPoints <<== data
    
    index = new cv::flann::Index_<float>(searchPoints, cv::flann::KDTreeIndexParams(8));
    
}

void IRKDTree::searchForPoint(int searchIndex, std::vector<int> *res) {

//    cout << searchPoints.size[0] << "|" << searchPoints.size[1] << endl;
    
    if (searchPoints.size[0] > 0) {
        currentIndex = searchIndex;
    
//        cout << "currentIndex: " << currentIndex << "\n";
        
        cv::Mat m_indices(1, knnValue, CV_32S);
        cv::Mat m_dists(1, knnValue, CV_32F);
        cv::Mat m_object(1, 64, CV_32F);
        for (int i=0; i<64; i++) {
            m_object.at<float>(i) = searchPoints.ptr<float>(currentIndex)[i];
//            cout << (float)searchPoints.ptr<float>(currentIndex)[i] << "|";
  
        
            
            //        imageHist[i] = searchPoints.ptr<float>(currentIndex)[i]; // ????
        
        
        }
//        cout << endl;
        
        index->knnSearch(m_object, m_indices, m_dists, knnValue, cv::flann::SearchParams(width));
        
//        cout << "\n  |" << currentIndex << "|\n";
        for (int i=0; i<knnValue; i++) {
//            cout << "  " << i << ": " << m_indices.at<int>(0,i) << endl;
            res->push_back(m_indices.at<int>(0,i));
        }
    }
}

void IRKDTree::clearIndex() {
    if (searchPoints.size[0] > 0) {
//        cout << "clearing search points...\n";
        searchPoints.release();
//        cout << searchPoints.size[0] << "|" << searchPoints.size[1] << endl;
    }
}
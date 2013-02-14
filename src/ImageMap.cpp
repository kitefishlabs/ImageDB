//
//  ImageMap.cpp
//  ImageCorpus
//
//  Created by Thomas Stoll on 1/15/12.
//  Copyright (c) 2012 Kitefish Labs. All rights reserved.
//

#include "ImageMap.h"

void ImageMap::setup(int threshold) {
    
    prefab = false;
    analysisThreshold = threshold;
    
}

void ImageMap::setupDownsampleThresholdAnalyze(int id, string imageFilePath) {
    
    ofImage                 ofSrcImage;               // temp var!
    ofxCvColorImage         cvDS2Image;
    ofxCvGrayscaleImage     cvGrayImage;
    
    IplImage                *iplSrcImage;
    
    ofxCvContourFinder      contourFinder;
    
    imageID = id;
    imageFileBase = ofFilePath::getBaseName(imageFilePath);
    
    ofSrcImage.loadImage(imageFilePath);
    imagePath = imageFilePath;
    width = ofSrcImage.width;
    height = ofSrcImage.height;
        
    cvSrcImage.allocate(width, height);
    cvDS2Image.allocate(width/2, height/2);
    cvGrayImage.allocate(width/2, height/2);
    
    cvSrcImage.setFromPixels(ofSrcImage.getPixels(), width, height);
    
    ds2ImageHistogram.setup();
    
    iplSrcImage = cvSrcImage.getCvImage();  // src -> ipl
    
    IplImage *ds2;
    ds2 = cvDS2Image.getCvImage();
    downSample2(iplSrcImage, ds2);
    
//    ds2ImageHistogram.iplHistImage = cvDS2Image.getCvImage(); // copy into histogram
//    downSample2(iplSrcImage, ds2ImageHistogram.iplHistImage); // downsample
//    cvDS2Image.flagImageChanged();
    
    ds2ImageHistogram.calcHistogram(ds2);    
    cout << ds2ImageHistogram.writeOutHistogram() << endl;
    
    
    cvGrayImage = cvDS2Image;
    cvGrayImage.threshold(analysisThreshold);
    cvGrayImage.flagImageChanged();
    
//    cout << "gray's width: " << cvGrayImage.width << endl << endl;
//    cout << cvGrayImage.getPixels()[640] << endl;
    numberOfRegions = 0;
    contourFinder.findContours(cvGrayImage, 200, (320*180), N_REGIONS, true);
    int i=0;
    vector<ofxCvBlob> blobs = contourFinder.blobs;
    
//    cout << contourFinder.nBlobs << "\n";
    
    vector<ofxCvBlob>::iterator it;
    for (it = blobs.begin(); it != blobs.end(); it++) {         // record metadata using ImageRegion objects
        ofxCvBlob temp = (ofxCvBlob)(*it);
        ImageRegion ir;
        ir.prefab = false;
        ir.contour = temp;
        ir.regionID = i;
        ir.size = cvSize(temp.boundingRect.width, temp.boundingRect.height);
        ir.anchor = cvPoint(temp.boundingRect.getCenter().x, temp.boundingRect.getCenter().y);
        ir.area = temp.area;
        ir.ratio = temp.area / (temp.boundingRect.width * temp.boundingRect.height);
        ir.centroid = cvPoint(temp.centroid.x, temp.centroid.y);
        ir.bFBOAllocated = false;
        ir.regionHistogram.setup();             // now this particular region's histogram is ready for calculation!
        
        mappedRegions.insert(pair<int,ImageRegion>(i,ir));
        i++;
    }
    numberOfRegions = i;
//    cout << "titnb: " << numberOfRegions << "\n";
    
}

void ImageMap::findContourMask(int rnum, bool bSaveFlag) {
    
//    cout << "findContourMask!\n";
    
    ofxCvColorImage         cvCurrentROI;
    ofxCvColorImageAlpha    cvCurrentROIWithAlpha;
    ofxCvGrayscaleImage     cvCurrentContourMask;    

    IplImage                *currentROIIPL;
    IplImage                *currentROIWithAlphaIPL;
    IplImage                *currentContourMaskIPL;
    
    
//    cout << rnum << ": " << mappedRegions[rnum].area  << "\n";
    
    int irw = mappedRegions[rnum].size.width;
    int irh = mappedRegions[rnum].size.height;
    
//    cout << irw << "\n";
    
    cvCurrentROI.clear();
    cvCurrentROIWithAlpha.clear();
    
    cvCurrentROI.allocate(irw*2, irh*2);                       // reallocate the ROI for the current blob
    cvCurrentROIWithAlpha.allocate(irw*2, irh*2);
//    cout << "::  " << cvCurrentROIWithAlpha.bAllocated << endl << endl;
    
    // cvSrcImage in NOT downsampled, mappedRegions ARE
    cvSrcImage.setROI(((mappedRegions[rnum].anchor.x*2)-irw), // (* 2 / 2)
                      ((mappedRegions[rnum].anchor.y*2)-irh),
                      irw*2,
                      irh*2);
    
    cvCurrentROI.setFromPixels(cvSrcImage.getRoiPixelsRef());
//    currentROIIPL = cvCurrentROI.getCvImage();
//    currentROIWithAlphaIPL = cvCurrentROIWithAlpha.getCvImage();
    
    // convert contour points into a 1-channel image using fbo
    int bbx = mappedRegions[rnum].anchor.x;
    int bby = mappedRegions[rnum].anchor.y;
//    cout << "allocating fbo for blob #" << mappedRegions[rnum].regionID << " / " << numberOfRegions << ")\n";
    //    cout << ir.area << endl;
    mappedRegions[rnum].fbo.allocate(irw*2, irh*2);
    mappedRegions[rnum].bFBOAllocated = true;
    mappedRegions[rnum].fbo.begin();
    
    ofClear(255, 255, 255);
    ofSetHexColor(0x000000);
    ofFill();
    ofBeginShape();
    vector<ofPoint>::iterator it;
    for (it = mappedRegions[rnum].contour.pts.begin(); it < mappedRegions[rnum].contour.pts.end(); it++) {
//        cout << (*it).x << "|" << bbx << " " << (*it).y << "|" << bby << endl;
        ofVertex(((*it).x - (bbx-(irw/2)))*2, ((*it).y - (bby-(irh/2)))*2);        
        //        cout << (*it).x << "|" << (*it).y << endl;
    }
    ofEndShape(true);
    
    mappedRegions[rnum].fbo.end();
    
    uchar* pxls = cvCurrentROI.getPixels(); // cvSrcImage.getRoiPixels()
    uchar* apxls = cvCurrentROIWithAlpha.getPixels();
    
    ofPixels mpxls;
    mpxls.allocate(irw*2, irh*2, OF_PIXELS_MONO);
    mappedRegions[rnum].fbo.readToPixels(mpxls);
    int fw = mappedRegions[rnum].fbo.getWidth();
    
    for (int y=0; y<mappedRegions[rnum].fbo.getHeight(); y++) {
        for (int x=0; x<fw; x++) {
            int z = (((y*fw)+x)*3);
            //            cout << "||" << pxls[z] << ", " << pxls[z+1] << ", " << pxls[z+2] << " <<== ";
            int filter = (int)((255. - mpxls[(((y*fw)+x)*4)+0]) / 255.);
            //            int filter = (int)(mpxls[(((y*fw)+x)*4)+0]);
            //            cout << filter << ", ";
            if (filter != 0) {
                apxls[(((y*fw)+x)*4)+2] = pxls[z+2] * filter;
                apxls[(((y*fw)+x)*4)+1] = pxls[z+1] * filter;
                apxls[(((y*fw)+x)*4)+0] = pxls[z+0] * filter;
                apxls[(((y*fw)+x)*4)+3] = 255;
            } else {
                apxls[(((y*fw)+x)*4)+2] = 255;
                apxls[(((y*fw)+x)*4)+1] = 255;
                apxls[(((y*fw)+x)*4)+0] = 255;
                apxls[(((y*fw)+x)*4)+3] = 0;
            }
        }
    }
    
    cvCurrentROIWithAlpha.setFromPixels(apxls, irw*2, irh*2);
    cvCurrentROIWithAlpha.flagImageChanged();
    mappedRegions[rnum].regionImage.allocate(cvCurrentROIWithAlpha.width, cvCurrentROIWithAlpha.height);
    mappedRegions[rnum].regionImage = cvCurrentROIWithAlpha.getCvImage();
    mappedRegions[rnum].regionImage.setAnchorPercent(0, 0);
    mappedRegions[rnum].regionImage.flagImageChanged();
    
    // save the alpha masked region to a PNG
    if (bSaveFlag) {
    
//        cout << "saving!!!\n";
        ofImage outImg;
        outImg.setFromPixels(apxls, irw*2, irh*2,OF_IMAGE_COLOR_ALPHA);
        stringstream oss;
        string stripped = ofFilePath::removeExt(imagePath);
        oss << stripped << "_" << rnum << ".png";
        outImg.saveImage(oss.str());
    
    }    
    
//    mappedRegions[rnum].regionHistogram.iplHistImage = cvCurrentROIWithAlpha.getCvImage();
//    mappedRegions[rnum].regionHistogram.calcHistogram();

    mappedRegions[rnum].regionHistogram.calcHistogram(cvCurrentROIWithAlpha.getCvImage());
//    cout << "###  " << mappedRegions[rnum].regionHistogram.bHistogramCalculated << "|" << mappedRegions[rnum].regionHistogram.rgbMax << "\n";
    cvSrcImage.resetROI();                 // clean up!!!
    
    
}

void ImageMap::downSample2(IplImage* srcIPL, IplImage* destIPL) {
    cvPyrDown(srcIPL, destIPL);
}

void ImageMap::writeAnalysis(int regionIndex) {
//    cout << "     --- write analysis ....\n";

    ImageRegion ir = mappedRegions.at(regionIndex);

    ofFilePath savePath;
    string stripped = savePath.removeExt(imagePath);
//    cout << "stripped for write: " << stripped << endl;

    stringstream ss;
    ss << "_" << ir.regionID << ".txt";
    stripped += ss.str();   
    ofFile file(stripped, ofFile::WriteOnly);

    stringstream ss1;
    ss1 << stripped << "\n" << ir.regionID << "\n" << ir.size.width << " " << ir.size.height << "\n" << ir.anchor.x << " " << ir.anchor.y << "\n" << ir.area << "\n" << ir.ratio << "\n" << ir.centroid.x << " " << ir.centroid.y << "\n";
    file << ss1.str();
//    cout << "\n\n" << ss1.str() << "\n";

    file << ir.regionHistogram.writeOutHistogram();

    file.close();
}

void ImageMap::constructPrefabRegion(string name, int currentIndex, CvPoint anchor, CvSize size, float area, float ratio, CvPoint centroid, int histmax) {

    ImageRegion ir;
    ir.prefab = true;
    ir.regionID = currentIndex;
    ir.size = size;
    ir.anchor = anchor;
    ir.area = area;
    ir.ratio = ratio;
    ir.centroid = centroid;
    
//    ir.bFBOAllocated = false;
    ir.regionHistogram.setup();             // now this particular region's histogram is ready for INJECTION
    ir.regionHistogram.histMax = histmax;
    
    mappedRegions.insert(pair<int,ImageRegion>(currentIndex,ir));
    
    ImageRegion outIR = mappedRegions[currentIndex];
    
    stringstream ss1;
    ss1 << "PREFAB PARAMS...\n" << outIR.regionID << "\n" << outIR.size.width << " " << outIR.size.height << "\n" << outIR.anchor.x << " " << outIR.anchor.y << "\n" << outIR.area << "\n" << outIR.ratio << "\n" << outIR.centroid.x << " " << outIR.centroid.y << "\n" << outIR.regionHistogram.histMax << "\n";
//    cout << ss1.str() << "\n\n";
    
}
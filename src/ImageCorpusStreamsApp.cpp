//
//  ImageCorpusStreamsApp.cpp
//  ImageCorpus
//
//  Created by Thomas Stoll on 1/23/12.
//  Copyright (c) 2012 Kitefish Labs. All rights reserved.
//

#include "ImageCorpusStreamsApp.h"

void ImageCorpusStreamsApp::setup() {
    
    ofSetFrameRate(8);
    ofEnableAlphaBlending();
    
    bTreeSearchable = false;
    totalNumBlobs, totalNumImages = 0;
    tCounter = -1;
    importCorpusFromDirectory("random/1/");
    
    constructCorpusTree(); // bTreeSearchable should flip from false to true
    

    if (totalNumBlobs > 0) {
        
        mapped = new (nothrow) int[totalNumBlobs*13];
        
        for (int i=0; i<totalNumBlobs; i++) { 
            for (int j=0; j<13; j++) {
                mapped[(i*13)+j] = -1;
            }
        }
    }
}

void ImageCorpusStreamsApp::update() {
    tCounter = (tCounter+1)%totalNumBlobs;
//    cout << bTreeSearchable << " | " << mapped[tCounter*13] << "\n";
    if (bTreeSearchable && (mapped[tCounter*13] < 0)) {
        result.clear();
//        cout << (int)result.size() << "\n";
            
        tree.searchForPoint(tCounter, &result);
        
        if ((int)result.size() > 2) {
//            cout << "\n\nresult: (" << result.size() << ")";
            for (int i=0; i<MIN(result.size(),12); i++) {
                
//                cout << result[i] << "|" << mappedIndexes[result[i]].first << "|" << mappedIndexes[result[i]].second << " || ";
                
                mapped[(result[i]*13)+12] += (12 - i); // increment last slot of result ID, weighting for closeness
                mapped[(tCounter*13)+i] = result[i];
                
            } 
        }
//        cout << "\n";
    }
    for (int i=0; i<totalNumBlobs; i++) {
//        cout << (mapped[(i*13)+12] - 12) << " . ";
    }
//    cout << "\n";
}

void ImageCorpusStreamsApp::draw() {
    ofSetColor(255, 255, 255);
    
//    cout << bTreeSearchable << " |::: " << mapped[tCounter*13] << "\n";

    if (bTreeSearchable && (mapped[tCounter*13] > -1)) {
        int targ, tIID, tRID = 0;
        targ = tCounter;
        tIID = mappedIndexes[targ].first;
        tRID = mappedIndexes[targ].second;
        
        mappedImages[tIID].mappedRegions[tRID].regionImage.draw(0, 0);

        int spacerX = mappedImages[tIID].mappedRegions[tRID].regionImage.width+20;
        int spacerY = mappedImages[tIID].mappedRegions[tRID].regionImage.height+20;
        
        for (int i=0; i<12; i++) {
            
            if (mapped[(tCounter*13)+i] > -1) {
                int zero = mapped[(tCounter*13)+i];
                int zIID = mappedIndexes[zero].first;       // image ID
                int zRID = mappedIndexes[zero].second;      // image s

                mappedImages[zIID].mappedRegions[zRID].regionImage.draw(i*spacerX, spacerY);
                mappedImages[zIID].mappedRegions[zRID].regionHistogram.histViz.draw (i*spacerX, spacerY*2);
            }
        }
    }
}


void ImageCorpusStreamsApp::importCorpusFromDirectory(string dirName) {
    
    ofDirectory DIR;
    string inpath;
    
    DIR.allowExt("fdb");
    
    int numFiles = DIR.listDir(dirName);
//    cout << numFiles << "\n";
    
    if (numFiles == 1) {
        inpath = DIR.getPath(0);
        ifstream ifile;
        string readin;
//        cout << inpath << "\n";
        ifile.open(ofToDataPath(inpath).c_str(), ios::in);        
        
        if (ifile.is_open()) {
//            cout << "open!\n";
            getline(ifile, readin);
            string fldr = readin.c_str();
            getline(ifile, readin);
            totalNumImages = atoi(readin.c_str());
            getline(ifile, readin);
            totalNumBlobs = atoi(readin.c_str());
            getline(ifile, readin);
            gStandardThreshold = atoi(readin.c_str());
            
//            cout << totalNumImages << "|" << totalNumBlobs << "||" << gStandardThreshold << "\n";
            
            int bnum = 0;
            
            for (int i=0; i<totalNumImages; i++) {
                
                ImageMap im;
                im.setup(gStandardThreshold);
                im.prefab = true;
                
                getline(ifile, readin, ' ');
                int imnum = atoi(readin.c_str());
//                cout << "     i|imnum: " << i << "|" << imnum << "\n";
                im.imageID = imnum;
                getline(ifile, readin, ' ');
                int numRegions = atoi(readin.c_str());
                getline(ifile, readin);
                string basename = readin;
                
//                cout << imnum << "|" << numRegions << "||" << basename << "\n";
                
                for (int reg=0; reg<numRegions; reg++) {
                    
                    // get the .txt files' + the .png files' names
                    stringstream sst, ssp, ssmd;
                    sst << dirName << basename << "_" << reg << ".txt";
                    ssp << dirName << basename << "_" << reg << ".png";
                    
                    ofFile mdfile;
                    mdfile.open(sst.str());
                    
                    if ((numRegions > 0) && (mdfile.is_open())) {
                        string name;
                        int regionIndex;
                        CvPoint anchor;
                        CvSize size;
                        float area, ratio;
                        CvPoint centroid;
                        int hsvmax;
                        getline(mdfile, readin);
                        name = readin; // DUMMY!
                        getline(mdfile, readin);
                        regionIndex = atoi(readin.c_str());
//                        cout << "ri: " << regionIndex << "\n";
                        
                        getline(mdfile, readin, ' ');
                        size.width = atoi(readin.c_str());
                        getline(mdfile, readin);
                        size.height = atoi(readin.c_str());
                        getline(mdfile, readin, ' ');
                        anchor.x = atoi(readin.c_str());
                        getline(mdfile, readin);
                        anchor.y = atoi(readin.c_str());

                        
                        getline(mdfile, readin);
                        area = atof(readin.c_str());
                        getline(mdfile, readin);
                        ratio = atof(readin.c_str());
                        
                        getline(mdfile, readin, ' ');
                        centroid.x = atof(readin.c_str());
                        getline(mdfile, readin);
                        centroid.y = atof(readin.c_str());
                        
                        getline(mdfile, readin);
                        hsvmax = atoi(readin.c_str());
                    
                        im.constructPrefabRegion(name, regionIndex, anchor, size, area, ratio, centroid, hsvmax);
                        
                        mappedIndexes.insert(pair<int, pair<int, int> >(bnum, pair<int, int>(im.imageID, regionIndex)));
                        
                        im.mappedRegions[regionIndex].regionHistogram.setup();
                        
//                        cout << "injecting 64 values!\n";
                        int vals[64];
                        int* vptr;
                        vptr = vals;
                        
                        for (int h=0; h<64; h++) {                   // N_REGIONS!
                            
//                            getline(mdfile, readin, ' ');
//                            int rd = atoi(readin.c_str());
//                            getline(mdfile, readin, ' ');
//                            int gn = atoi(readin.c_str());
//                            getline(mdfile, readin);
//                            int bl = atoi(readin.c_str());
                            
                            getline(mdfile, readin, ' ');
                            vals[h] = atoi(readin.c_str());
                        }
                        
                        im.mappedRegions[regionIndex].regionHistogram.injectHistogram(vptr);
                        
//                        cout << "\n" << (int)im.mappedRegions[regionIndex].regionHistogram.histArray[2] << "\n";
                        
                        ofImage image;
//                        cout << "Adding PNG image: " << ssp.str() << "\n";
                        image.loadImage(ssp.str());
                        
//                        cout << image.width << "\n\n";
                        
                        im.mappedRegions[regionIndex].regionImage.allocate(image.width, image.height);
                        im.mappedRegions[regionIndex].regionImage.setFromPixels(image.getPixels(), image.width, image.height);
                        
//                        cout << "and now the histogram:\n" << im.mappedRegions[regionIndex].regionHistogram.writeOutHistogram();
                        
                        bnum++;
                        
                    } else {
                        cout << "no regions for this file!!!\nor " << sst.str() << " is missing...\n";
                    }
                }                                
//                cout << "inserting imagemap at: " << i << "\n"; 
                mappedImages.insert(pair<int, ImageMap>(i, im));
            }
        }
    }
}

void ImageCorpusStreamsApp::constructCorpusTree() {
    
    tree.setup(12);
    map<int, ImageMap>::iterator miIter;
    map<int, ImageRegion>::iterator mrIter;
    
    float corpusArray[64*totalNumBlobs];
    float * dataPtr;
    dataPtr = corpusArray;
    
//    cout << "$$$ " << dataPtr[64*9] << " | " << dataPtr[64*18] << "\n\n";
    for (int i=0; i<(64*totalNumBlobs); i++) { corpusArray[i] = 0.0; }
    int caIndex = 0;
    
    
    int asd = 0;
    for (miIter = mappedImages.begin(); miIter != mappedImages.end(); miIter++) {

        asd++;
        
        ImageMap im = (*miIter).second;
        int findex = im.imageID;
        
        for (mrIter = im.mappedRegions.begin(); mrIter != im.mappedRegions.end(); mrIter++) {
         
            ImageRegion ir = (*mrIter).second;
            int relindex = ir.regionID;
            float rgbmax = (float)ir.regionHistogram.histMax;

            for (int i=0; i<64; i++) {
                
                corpusArray[caIndex+i] = (float)(ir.regionHistogram.histArray[i] / 255.0);
//                cout << corpusArray[i] << " ";

            }
            
//            cout << "\n" << caIndex << "|" << rgbmax << "||" << findex << "|" << relindex << "\n";
            caIndex += 64;
            
            
        }
    }
//    cout << "\n";
    
    tree.initAndUploadData(dataPtr, totalNumBlobs);
    bTreeSearchable = true;
    
}

void ImageCorpusStreamsApp::keyPressed(int key) {

}

void ImageCorpusStreamsApp::keyReleased(int key) {
    
}

void ImageCorpusStreamsApp::mouseMoved(int x, int y ) {
    
}

void ImageCorpusStreamsApp::mouseDragged(int x, int y, int button) {
    
}

void ImageCorpusStreamsApp::mousePressed(int x, int y, int button) {
    
}

void ImageCorpusStreamsApp::mouseReleased(int x, int y, int button) {
    
}

void ImageCorpusStreamsApp::windowResized(int w, int h) {
    
}

void ImageCorpusStreamsApp::dragEvent(ofDragInfo dragInfo) {
    
}

void ImageCorpusStreamsApp::gotMessage(ofMessage msg) {
    
}



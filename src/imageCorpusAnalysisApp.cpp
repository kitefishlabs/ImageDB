#include "imageCorpusAnalysisApp.h"

//--------------------------------------------------------------
void imageCorpusAnalysisApp::setup(){

    ofSetFrameRate(4);
    ofEnableAlphaBlending();
    ofSetRectMode(OF_RECTMODE_CORNER);
    gStandardThreshold = 120;
    tCounter = 0;
    
    analyzeDirectory("random/1/");
    
    currentImageNum = 0;
    mappedImages[currentImageNum].currentRegion = -1;
    
}

//--------------------------------------------------------------
void imageCorpusAnalysisApp::update(){
    
    ofBackground(0, 0, 0);
    
    if (mappedImages[currentImageNum].numberOfRegions > 0) {
        mappedImages[currentImageNum].currentRegion = (mappedImages[currentImageNum].currentRegion + 1) % mappedImages[currentImageNum].numberOfRegions;
    }
    if (mappedImages[currentImageNum].currentRegion == 0) {
        currentImageNum = (currentImageNum + 1) % totalNumImages;
        mappedImages[currentImageNum].currentRegion = 0;
    }
}

//--------------------------------------------------------------
void imageCorpusAnalysisApp::draw(){
    if (mappedImages[currentImageNum].numberOfRegions > 0) {
        cout << "--> " << mappedImages[currentImageNum].currentRegion <<  "\n";
//        mappedImages[currentImageNum].mappedRegions[ mappedImages[currentImageNum].currentRegion ].contour.draw();
        
        ofSetColor(255, 255, 255);
        
        ImageRegion ir = mappedImages[currentImageNum].mappedRegions[ mappedImages[currentImageNum].currentRegion ];
        
        int x = ir.anchor.x - (ir.size.width/2);
        int y = ir.anchor.y - (ir.size.height/2);
        
        ir.regionImage.draw(x,y);
        if (ir.regionHistogram.bHistogramCalculated) { ir.regionHistogram.histViz.draw(25, 600); }
    }
}

void imageCorpusAnalysisApp::analyzeDirectory(string dirName) {
    
    ofDirectory DIR;
    string fdpPath, fp;
    
    totalNumImages = 0;
    totalNumBlobs = 0;
    totalNumImagesToBeAnalyzed = 0;
    currentImageNum = 0;
    
    // scan for fdb file
    DIR.allowExt("fdb");
    int numFDBFiles = DIR.listDir(dirName);
    cout << "# of .fdb files found (should be 1): " << numFDBFiles << endl;
    
    if (numFDBFiles == 1) {
        
        fdpPath = DIR.getPath(0);
        cout << ".fdb file path: " << fdpPath << "\n";
        ifstream ifile;
        ifile.open(ofToDataPath(fdpPath).c_str(), ios::in); // ofToDataPath ???
        string readin;
        
        if (ifile.is_open()) {
            
            getline(ifile, readin);
            string basename = readin;                           // don't use yet
            getline(ifile, readin);
            int numImages = (int)atoi(readin.c_str());          // used only for scanning array
            getline(ifile, readin);
            int numBlobs = (int)atoi(readin.c_str());           // don't use
            getline(ifile, readin);
            int threshold = (int)atoi(readin.c_str());          // this had better be right!
            
            cout << "Initial conditions (from .fdb file): " << numImages << "|" << numBlobs << "|" << threshold << "\n";
            
            for (int i=0; i<numImages; i++) {
                
                getline(ifile, readin, ' ');
                int bindex = (int)atoi(readin.c_str());         // blob index DUMMY!!!!!
//                getline(ifile, readin, ' ');
//                int bthresh = (int)atoi(readin.c_str());        // threshold (*for now, not used)
                getline(ifile, readin, ' ');
                int blobs = (int)atoi(readin.c_str());          // # just for this file

                getline(ifile, readin);
                stringstream ss;
                string blobPng, blobTxt;
                string bstring = readin.c_str();
//                cout << "|" << bstring << "|\n";
//                vector<int> blobvec;
                
                for (int b=0; b<blobs; b++) {

                    ss << bstring << "_" << b << ".png";
//                    cout << "SS: " << ss.str() << "\n";
                    blobPng = ss.str();
                    ss.clear(); ss.str(std::string());

                    ss << bstring << "_" << b << ".txt";
                    blobTxt = ss.str();
                    ss.clear(); ss.str(std::string());
                    
//                    cout << "blobPng/blobTxt:  " << blobPng << " | " << blobTxt << "\n";

                }
//                cout << "inserting: " << totalNumImages << "|" << bstring << "\n";
                mappedFileNames.insert(pair<int, string>(totalNumImages, bstring));
                mappedThresholds.insert(pair<int, int>(totalNumImages, threshold));
//                mappedBlobIDs.insert(pair<int, vector<int> >(totalNumImages, blobvec));
                
                totalNumImages++;
                totalNumBlobs = numBlobs;
                
            }
            
            cout << "After scanning .fdb file...\n";
            cout << "  *total # of images: " << totalNumImages << "\n";
            cout << "  *total # of blobs: " << totalNumBlobs << "\n";
            cout << "  *total # of images TO BE ANALYZED: " << totalNumImagesToBeAnalyzed << "\n";
        }
        ifile.close();
        
    }
    
    string jpegFileName;
    ofDirectory DIR2;
    DIR2.allowExt("jpg");
    int numJPGFiles = DIR2.listDir(dirName);
    cout << "# of JPEG files to import: " << numJPGFiles << "\n";
    
    for (int j=0; j<numJPGFiles; j++) {
        jpegFileName = ofFilePath::removeExt(DIR2.getName(j));
//        cout << ":::  " << jpegFileName << endl;
        
        bool matchFlag = false;
        
        map<int, string>::iterator itz;
        for (itz = mappedFileNames.begin(); itz != mappedFileNames.end(); itz++) {

//            cout << "compare: " << (*itz).second << "|" << jpegFileName << "\n";
            if ((*itz).second == jpegFileName) { // we have a match : do not add to be analyzed
            
                matchFlag = true;
                break;
            
            }
        }
        
        cout << "broke! ... " << (int)matchFlag << "\n";
        if (matchFlag == 0) {
            
            cout << "***total num images: " << totalNumImages << " + j: " << j << "\n";
            cout << "add to analyze: " << (totalNumImages + j) << ", " << jpegFileName << "\n";
            toBeAnalyzedFileNames.insert(pair<int, string>((totalNumImages + totalNumImagesToBeAnalyzed), jpegFileName));
            totalNumImagesToBeAnalyzed++;
        
        }
    }

    cout << "After scanning directory for .jpg files...\n";
    cout << "  * total # of images: " << totalNumImages << "\n"; // TO BE MAPPED (NOT ANALYZED)
    cout << "  * total # of blobs: " << totalNumBlobs << "\n";
    cout << "  * # of images YET TO BE ANALYZED: " << totalNumImagesToBeAnalyzed << "\n";
    cout << "  ** mapped file names map contains:\n";
    
    map<int, string>::iterator it;
    for (it = mappedFileNames.begin(); it != mappedFileNames.end(); it++) {
//        cout << (*it).first << "|" << (*it).second << "\n";
        currentImageNum++;
    }
    
//    cout << "current image #: " << currentImageNum << "\n";
    
    // setup out.fdb
    ofFile ofile;
    ofile.open("random/1/out.fdb", ofFile::WriteOnly);
    
    
    cout << "\n\n\nREPORT: \n\n";    
    // iterate over mapped and pull in metadata/pngs
    cout << "mapped:\n";
    if (mappedFileNames.begin() == mappedFileNames.end()) {
        cout << "\n no mapped files to import.\n";
    } else {
//        for (it = mappedFileNames.begin(); it != mappedFileNames.end(); it++) {
//            cout << "map w/o analyzing: " << (*it).first << "|" << (*it).second << "\n";            
            
        ifstream ifile;
        string fullPath, readin;
//        stringstream ss;
//        ss << "random/" << mappedFileNames[i] << ".jpg";
//        cout << "**************\n\n" << ss.str() << "\n\n";
        ifile.open(ofToDataPath(fdpPath).c_str(), ios::in);
        
        
        if ((ifile.is_open()) && (ofile.is_open())) {
                    
            for (int i=0; i<totalNumImages; i++) {
                
                ImageMap im;
                im.setup(gStandardThreshold);
                im.prefab = true;
                
                stringstream oss;
                
                getline(ifile, readin, ' ');
                int imnum = atoi(readin.c_str());
                cout << "     i|imnum: " << i << "|" << imnum << "   ...   ";
                im.imageID = imnum;
                getline(ifile, readin, ' ');
                im.numberOfRegions = atoi(readin.c_str());
                getline(ifile, readin);
                im.imageFileBase = readin;
                
                //                cout << imnum << "|" << im.numberOfRegions << "||" << basename << "\n";
                    
                for (int reg=0; reg<im.numberOfRegions; reg++) {
                    
                    
                    // get the .txt files' + the .png files' names
                    stringstream sst, ssp, ssmd;
                    sst << dirName << im.imageFileBase << "_" << reg << ".txt";
                    ssp << dirName << im.imageFileBase << "_" << reg << ".png";
                    
                    ofFile mdfile;
                    mdfile.open(sst.str());
                    
                    if ((im.numberOfRegions > 0) && (mdfile.is_open())) {
                        string name;
                        int regionIndex;
                        CvPoint anchor;
                        CvSize size;
                        float area, ratio;
                        CvPoint centroid;
                        int rgbmax;
                        getline(mdfile, readin);
                        name = readin; // DUMMY!
                        getline(mdfile, readin);
                        regionIndex = atoi(readin.c_str());
                        cout << "ri: " << regionIndex << "\n";
                        
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
                        rgbmax = atoi(readin.c_str());
                        
                        im.constructPrefabRegion(name, regionIndex, anchor, size, area, ratio, centroid, rgbmax);
                        
                        im.mappedRegions[regionIndex].regionHistogram.setup();
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
                            
//                            im.mappedRegions[regionIndex].regionHistogram.injectHistogram(h, rd, gn, bl);
                        }
                        im.mappedRegions[regionIndex].regionHistogram.injectHistogram(vptr);
                        
//                        int w = im.mappedRegions[regionIndex].size.width*2;
//                        int h = im.mappedRegions[regionIndex].size.height*2;
                        //                        cout << w << "|" << h << "\n";
                        //                        cout << ssp.str() << "\n";
                        //                        cout << ssp.str() << "\n";
//                        ofImage image;
//                        image.loadImage(ssp.str());
//                        
//                        //                        cout << image.width << "\n\n";
//                        
//                        im.mappedRegions[regionIndex].regionImage.allocate(image.width, image.height);
//                        im.mappedRegions[regionIndex].regionImage.setFromPixels(image.getPixels(), image.width, image.height);
                        
                        //                        cout << "and now the histogram:\n" << im.mappedRegions[regionIndex].regionHistogram.writeOutHistogram();
                        
                    } else {
                        cout << "no regions for this file!!!\nor " << sst.str() << " is missing...\n";
                    }
                }                                
//                cout << "inserting imagemap at: " << i << "\n"; 
                mappedImages.insert(pair<int, ImageMap>(i, im));
                // map one line of out.fdb
                oss << im.imageID << " " << im.numberOfRegions << " " << im.imageFileBase << "\n";

                ofile << oss.str();
                oss.flush();

            }
        } else {
            cout << "ERROR: COULD NOT LOAD FILE!\n";
        }
        
    }
    currentImageNum = totalNumImages;
    
    // iterate over toBeAnalyzed and perform analysis
    //    - write txt and png files!
//    cout << "\nto be Analyzed:\n";

//    cout << "curr. image: " << currentImageNum << "\n";
    
    for (it = toBeAnalyzedFileNames.begin(); it != toBeAnalyzedFileNames.end(); it++) {
        stringstream ss, oss;
        ss << "random/1/" << (*it).second << ".jpg";
//        cout << "analyzing: " << (*it).first << "  | " << ss.str() << "\n";
        
        ImageMap newIM;
//        mappedImages.insert(pair<int, ImageMap>(currentImageNum, newIM));
        
        newIM.setup(gStandardThreshold);
        newIM.setupDownsampleThresholdAnalyze( (int)((*it).first), (string)(ss.str()) );

//        cout << "#: " << newIM.numberOfRegions << "\n";
        
        for (int r=0; r<newIM.numberOfRegions; r++) {
//            cout << "r: " << r << endl;
            newIM.findContourMask(r, true);
            newIM.writeAnalysis(r);
            totalNumBlobs++;
        }
        // map one line of out.fdb
        oss.flush();
        oss << newIM.imageID << " " << newIM.numberOfRegions << " " << newIM.imageFileBase << "\n";
        
        mappedImages.insert(pair<int, ImageMap>(currentImageNum, newIM));
        
        totalNumImages++;
        currentImageNum++;
        
        ofile << oss.str();
        oss.flush();
    }
    
    stringstream oss;
    oss << "random/1/" << "\n";
    cout << "TOTALS:\n" << totalNumImages << "\n" << totalNumBlobs << "\n" << gStandardThreshold << "\n";
    oss << totalNumImages << "\n" << totalNumBlobs << "\n" << gStandardThreshold << "\n";
    ofile << oss.str();
    ofile.close();
    
}



//--------------------------------------------------------------
void imageCorpusAnalysisApp::keyPressed(int key){

}

//--------------------------------------------------------------
void imageCorpusAnalysisApp::keyReleased(int key){

}

//--------------------------------------------------------------
void imageCorpusAnalysisApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void imageCorpusAnalysisApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void imageCorpusAnalysisApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void imageCorpusAnalysisApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void imageCorpusAnalysisApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void imageCorpusAnalysisApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void imageCorpusAnalysisApp::dragEvent(ofDragInfo dragInfo){ 

}

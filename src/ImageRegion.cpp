//
//  ImageRegion.cpp
//  ImageCorpus
//
//  Created by Thomas Stoll on 1/15/12.
//  Copyright (c) 2012 Kitefish Labs. All rights reserved.
//

#include "ImageRegion.h"

void ImageHistogram::setup() {

    bHistogramCalculated = false;
    histMax = 0;
    
    reset();
}

void ImageHistogram::reset() {
    /* Reset histograms */
}

void ImageHistogram::calcHistogram(IplImage *source) {
    
    IplImage* img = cvCloneImage(source);
	IplImage* back_img = cvCreateImage( cvGetSize( img ), IPL_DEPTH_8U, 1 );
    
    histViz.allocate(80, 80);
    histIPL = histViz.getCvImage();
    
	// Compute HSV image and separate into colors
	IplImage* hsv = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
	cvCvtColor( img, hsv, CV_BGR2HSV );
    
	IplImage* h_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* s_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* v_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* planes[] = { h_plane, s_plane };
	cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
    
	// Build and fill the histogram
	int h_bins = 8, s_bins = 8;
    //	CvHistogram* hist;
	{
		int hist_size[] = { h_bins, s_bins };
		float h_ranges[] = { 0, 180 };
		float s_ranges[] = { 0, 255 };
		float* ranges[] = { h_ranges, s_ranges };
		hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	}
	cvCalcHist( planes, hist, 0, 0 ); // Compute histogram
	cvNormalizeHist( hist, 20*255 ); // Normalize it
    
	cvCalcBackProject( planes, back_img, hist );// Calculate back projection
	cvNormalizeHist( hist, 1.0 ); // Normalize it
    
	
	
	// Create an image to visualize the histogram
	int scale = 10;
    //	histIPL = cvCreateImage( cvSize( h_bins * scale, s_bins * scale ), 8, 3 );
	cvZero ( histIPL );
    
	// populate the visualization
	float max_value = 0;
	cvGetMinMaxHistValue( hist, 0, &max_value, 0, 0 );
//    histMax = (int)(max_value * 255);

//    cout << max_value << "\n";
    
    int a = 0;
	for( int h = 0; h < h_bins; h++ ){
		for( int s = 0; s < s_bins; s++ ){
			float bin_val = cvQueryHistValue_2D( hist, h, s );
            //            cout << bin_val << " ";
            //            int intensity = cvRound( (bin_val / max_value) * 255 );
            float ival = sqrt((float)(bin_val / max_value));
//            float ival = pow((double)(bin_val / max_value), 2.0);
            int intensity = cvRound( ival * 255 );
//            cout << intensity << " ";
			cvRectangle( histIPL, cvPoint( h*scale, s*scale ),
						cvPoint( (h+1)*scale - 1, (s+1)*scale - 1 ),
						CV_RGB( intensity, intensity, intensity ),
						CV_FILLED );
            histArray[a] = intensity;
            a++;
		}
	}
//    cout << "\n";
    
    histViz.flagImageChanged();

    cvReleaseImage( &img );
    cvReleaseImage( &back_img );
    
    bHistogramCalculated = true;
//    cout << "RGBMAX: " << histMax << " " << bHistogramCalculated << "\n";
    
}

void ImageHistogram::injectHistogram(int* data) {
    
    histViz.allocate(80, 80);
    histIPL = histViz.getCvImage();

    int a = 0;
	for( int h=0; h<8; h++ ){
		for( int s=0; s<8; s++ ){
			float bin_val = data[(h*8)+s];//cvQueryHistValue_2D( hist, h, s );
            float ival = sqrt((float)(bin_val / 255.0));
            int intensity = cvRound( ival * 255 );
			cvRectangle( histIPL, cvPoint( h*10, s*10 ),
						cvPoint( (h+1)*10 - 1, (s+1)*10 - 1 ),
						CV_RGB( intensity, intensity, intensity ),
						CV_FILLED );
            histArray[a] = intensity;
            a++;
		}
	}    
    histViz.flagImageChanged();
    
}

void ImageHistogram::displayHistogram(int offsetX, int offsetY, int iplHeight) {

    histViz.draw(offsetX, offsetY);
}

string ImageHistogram::writeOutHistogram() {
    
    stringstream ss;
    ss << histMax << "\n";    
    
    for (int i=0; i<64; i++) { 
        ss << histArray[i] << " ";
    }
    ss << "\n";
    string out = ss.str();
    
    return out;
    
}


#include "stdafx.h"
#include "cv.h"    
#include "highgui.h"     
#include <fstream>    
#include <stdio.h>    
#include <stdlib.h>    
#include <conio.h>
#include <iostream>
#include <string.h>    
#include <assert.h>    
#include <math.h>    
#include <float.h>    
#include <limits.h>    
#include <time.h>    
#include <ctype.h>    
   
#ifdef _EiC    
#define WIN32    
#endif    
   
void Eyes_Location( IplImage *image,int img_no, std::string dir1, std::string dir2  );   
void detectMouth( IplImage *image,int img_no, std::string dir1, std::string dir2);  

IplImage* rotateImage(const IplImage* src, int angleDegrees)
{
    //take the dimention of original image
    int w = src->width;
    int h = src->height; 

    // Make a new image for the result
    CvSize newSize;
    newSize.width = cvRound(w);
    newSize.height = cvRound(h);
    IplImage *imageRotated = cvCreateImage( newSize, src->depth, src->nChannels );
    
    
    // Create a map_matrix, where the left 2x2 matrix is the transform and the right 2x1 is the dimensions.
    float m[6];
    CvMat M = cvMat(2, 3, CV_32F, m);
    
    float angleRadians = angleDegrees * ((float)CV_PI / 180.0f);
    m[0] = (float)( cos(angleRadians) );
    m[1] = (float)( sin(angleRadians) );
    m[3] = -m[1];
    m[4] = m[0];
    m[2] = w*0.5f;  
    m[5] = h*0.5f;  


    // Transform the image
    cvGetQuadrangleSubPix( src, imageRotated, &M);

    return imageRotated;
}


  

static CvMemStorage* storage_l = 0;   
static CvMemStorage* storage_r = 0;   
using namespace std;   
void main()   
{   
    //const char* filename = "2_face.jpg";   
    IplImage* image = 0;   
	IplImage* rotated_img;
    //image = cvLoadImage(filename, 1);   
   
    storage_l = cvCreateMemStorage(0);   
    storage_r = cvCreateMemStorage(0);  


	ifstream File("speaker_list_UFlinders.txt");
    string line;
	string folder[108];
	short loop,loop2,row,col;
	char imageName[120],temp_imageName[120];
	int i = 0;
	if(File)
    {
	    while(getline(File,line))
        {
			for (loop =0;loop<7;loop++)
			{
				folder[i] =line;
			}
            //printf(line);
			//cout << folder[i];
			//cvWaitKey(0);
			//getchar();
			i++;
        }
		File.close();
    }
	string folder2[12] = {"0123","0429","1230","2094","2301","3012","4902","5678","6785","7856","8567","9420"}; 
    string fold;
	string fold2;
	int totalImages;
	

	for(int j=0;j<i;j++)
    {
		for (int k =0;k<12;k++)
		{
			for (int l =1; l<=200;l++)
			{
				sprintf(imageName,"Z:\\MyPhD\\AV_database\\AusTalk\\AusTalk_Video\\TrUFlinders\\%s\\%s\\%d.jpg",folder[j].c_str(),folder2[k].c_str(),l);
				//cout<<imageName;
				//getchar();
				image=cvLoadImage(imageName,1);
				if(!image)
				{
					printf("Could not load image file and trying once again: %s\n",image);
					//getchar();
					//cout<<l;
				}
				if ( image )   
				{   
					//rotated_img=rotateImage(image,28);

					//cvNamedWindow( "result" );   
					//cvShowImage( "result", rotated_img ); 
					//cvWaitKey(0); 

					//cvNamedWindow( "result" );   
					//cvShowImage( "result", image ); 
					
					//cout<< imageName;
					Eyes_Location( image,l,folder[j].c_str(),folder2[k].c_str());	  
					
					//cvWaitKey(0);   
					//cvDestroyWindow( "result" );   
				}   
				if ( image )   
				{   
					
					
					//cout<< imageName;
					//detectMouth( image,l,folder[j].c_str(),folder2[k].c_str());	  
					
					//cvWaitKey(0);   
					//cvDestroyWindow( "result" );   
				}   
       
				cvReleaseImage( &image );   

			}
		}
	}


	
   
}   
   
void detectMouth( IplImage *image,int img_no, std::string dir1, std::string dir2)
 {
    int i;  
	cvSetImageROI(image,/* the source image */ 
                 cvRect(0,            /* x = start from leftmost */
                         0+(image->height *2/3), /* y = a few pixels from the top */
                         image->width,        /* width = same width with the face */
                         image->height/3    /* height = 1/3 of face height */
                        )
                 );
	
	IplImage* tmp = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
	cvCopy(image, tmp, NULL);
	cvResetImageROI(image);
	image = cvCloneImage(tmp);
	//cvShowImage( "result", image ); 
	//cvWaitKey(0); 
	CvSeq *mouth;
	const char* mouth_cascade = "haarcascade_mcs_mouth.xml";
	static CvHaarClassifierCascade* cascade_mouth = ( CvHaarClassifierCascade* )cvLoad( mouth_cascade, 0, 0, 0 );
	
	cvClearMemStorage( storage_l );    
	if( !cascade_mouth )    
	{    
		fprintf( stderr, "ERROR: Could not load classifier cascade\n" );    
		fprintf( stderr,    
		"Usage: facedetect --cascade=\"<cascade_path>\" [filename|camera_index]\n" );    
		return;    
	}    
		
	mouth = cvHaarDetectObjects(image,/* the source image, with the estimated location defined */ 
                                cascade_mouth,      /* the eye classifier */ 
                                storage_l,        /* memory buffer */
                                 1.15, 4, 0,     /* tune for your app */ 
                                cvSize(25, 15)  /* minimum detection scale */
                                ); 
	if(mouth->total > 0)
	{
		//cout<<mouth->total;
		CvRect* mouth_cord = (CvRect*)cvGetSeqElem( mouth, mouth->total ); 
		//cvRectangle( image, cvPoint(r->x, r->y), cvPoint((r->x+r->width), (img_copy->height-2)), CV_RGB(255,0,0), 1, 8, 0 );    
		//cvSetImageROI(image, cvRect(r->x, r->y,r->width,img_copy->height-2));
		cvSetImageROI(image, cvRect(mouth_cord->x, mouth_cord->y,mouth_cord->width,mouth_cord->height));
		
		IplImage* tmp = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
		cvCopy(image, tmp, NULL);
		cvResetImageROI(image);
		image = cvCloneImage(tmp);
		//cvShowImage( "result", image );  
		//cvWaitKey(0); 
		char imageName[120];
		sprintf(imageName,"Z:\\MyPhD\\AV_database\\AusTalk\\AusTalk_Video\\UFlinders\\%s\\%s\\%d.jpg",dir1.c_str(),dir2.c_str(),img_no);
		cvSaveImage(imageName,image);
		
		cvReleaseImage( &tmp ); 
		//cout<< imageName;
		
	}
	
}

void Eyes_Location( IplImage *image,int img_no, std::string dir1, std::string dir2  )    
{    
  int i;    
    
  IplImage* img_copy = cvCreateImage( cvSize( image->width, image->height ), 8, 1 );    
  cvCvtColor( image, img_copy, CV_BGR2GRAY );    
  cvEqualizeHist( img_copy, img_copy );    
  
  const char* cascade_name_pair = "parojos.xml";
  //const char* cascade_name_pair = "haarcascade_mcs_mouth.xml";

  static CvHaarClassifierCascade* cascade_pair = (CvHaarClassifierCascade*)cvLoad( cascade_name_pair, 0, 0, 0 );    
  
  cvClearMemStorage( storage_l );    
    
  if( !cascade_pair )    
  {    
      fprintf( stderr, "ERROR: Could not load classifier cascade\n" );    
      fprintf( stderr,    
      "Usage: facedetect --cascade=\"<cascade_path>\" [filename|camera_index]\n" );    
      return;    
  }    
    
  CvSeq* eyes_pair = cvHaarDetectObjects( img_copy, cascade_pair, storage_l, 1.1, 3, 0, cvSize(22, 5));    
    
  //for( i = 0; i < (eyes_pair ? eyes_pair->total : 0); i++ )    
  //  {    
      //CvRect* r = (CvRect*)cvGetSeqElem( eyes_pair, i );    
      
  if(eyes_pair->total > 0)
  {
	CvRect* r = (CvRect*)cvGetSeqElem( eyes_pair, 1 ); 
	//cvRectangle( image, cvPoint(r->x, r->y), cvPoint((r->x+r->width), (img_copy->height-2)), CV_RGB(255,0,0), 1, 8, 0 );    
	
	//cvSetImageROI(image, cvRect(r->x, r->y,r->width,img_copy->height-2));
	cvSetImageROI(image, cvRect(r->x, r->y,r->width,r->width));
	IplImage* tmp = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
	cvCopy(image, tmp, NULL);
	cvResetImageROI(image);
	image = cvCloneImage(tmp);
	//printf("Orig dimensions after crop: %dx%d\n", tmp->width, tmp->height);
	//cvShowImage( "result", image );   
	char imageName[120];
	sprintf(imageName,"Z:\\MyPhD\\AV_database\\AusTalk\\AusTalk_Video\\UFlinders\\%s\\%s\\%d.jpg",dir1.c_str(),dir2.c_str(),img_no);
	//cout<< imageName;
	//cvShowImage( "result", image ); 
	//cvWaitKey(0); 
	cvSaveImage(imageName,image);
	cvReleaseImage( &img_copy );  
	cvReleaseImage( &tmp ); 

  }
	  //  }    
  

  
}   

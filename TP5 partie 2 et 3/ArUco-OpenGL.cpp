//
//  ArUco-OpenGL.cpp
//
//  Created by Jean-Marie Normand on 28/02/13.
//  Copyright (c) 2013 Centrale Nantes. All rights reserved.
//



#include <iostream>
#include "ArUco-OpenGL.h"
#include <opencv2/imgproc/imgproc.hpp>

// Constructor
ArUco::ArUco(string intrinFileName, float markerSize) {
   // Initializing attributes
   m_IntrinsicFile= intrinFileName;
   m_MarkerSize   = markerSize;
   // read camera parameters if passed
   m_CameraParams.readFromXMLFile(intrinFileName);

   marker2_in_last_frame = false;
   marker3_in_last_frame = false;
   marker2_z = m_MarkerSize/4;
}

// Destructor
ArUco::~ArUco() {}

// Detect marker and draw things
void ArUco::doWork(Mat inputImg) {
   m_InputImage   = inputImg;
   m_GlWindowSize = m_InputImage.size();
   resize(m_GlWindowSize.width, m_GlWindowSize.height);
}

// Draw axis function
void ArUco::drawAxis(float axisSize) {
   // X
   glColor3f(1,0,0);
   glBegin(GL_LINES);
   glVertex3f(0.0f, 0.0f, 0.0f); // origin of the line
   glVertex3f(axisSize,0.0f, 0.0f); // ending point of the line
   glEnd( );
   
   // Y
   glColor3f (0,1,0);
   glBegin(GL_LINES);
   glVertex3f(0.0f, 0.0f, 0.0f); // origin of the line
   glVertex3f(0.0f, axisSize, 0.0f); // ending point of the line
   glEnd( );
   
   // Z
   glColor3f (0,0,1);
   glBegin(GL_LINES);
   glVertex3f(0.0f, 0.0f, 0.0f); // origin of the line
   glVertex3f(0.0f, 0.0f, axisSize); // ending point of the line
   glEnd( );
}

// GLUT functionnalities

// Drawing function
void ArUco::drawScene() {
   // If we do not have an image we don't do anyhting
   if (m_ResizedImage.rows==0)
      return;
   
   // Setting up  OpenGL matrices
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, m_GlWindowSize.width, 0, m_GlWindowSize.height, -1.0, 1.0);
   glViewport(0, 0, m_GlWindowSize.width , m_GlWindowSize.height);
   glDisable(GL_TEXTURE_2D);
   glPixelZoom( 1, -1);
   
   //////glRasterPos3f( 0, m_GlWindowSize.height  - 0.5f, -1.0f );
   glRasterPos3f(0, m_GlWindowSize.height, -1.0f);
   
   glDrawPixels (m_GlWindowSize.width, m_GlWindowSize.height, GL_RGB, GL_UNSIGNED_BYTE, m_ResizedImage.ptr(0));
   
   // Enabling depth test
   glEnable(GL_DEPTH_TEST);
   
   // Set the appropriate projection matrix so that rendering is done 
   // in an environment like the real camera (without distorsion)
   glMatrixMode(GL_PROJECTION);
   double proj_matrix[16];
   //m_CameraParams.glGetProjectionMatrix(m_InputImage.size(),m_GlWindowSize,proj_matrix,0.05,10);
   m_CameraParams.glGetProjectionMatrix(m_ResizedImage.size(),m_GlWindowSize,proj_matrix,0.05,10);
   glLoadIdentity();
   glLoadMatrixd(proj_matrix);
   
   // Debug : outputting projection matrix
   /*
    std::cout<<"Projection Matrix"<<std::endl;
   for(int i=0;i<4;i++) {
      std::cout<<"| ";
      for(int j=0;j<4;j++) {
         std::cout<<proj_matrix[j*4+i]<<"\t\t\t";
      }
      std::cout<<" |"<<std::endl;
   }
    */
   
   //now, for each marker,
   double modelview_matrix[16];
   std::cout << "Number of markers: " << m_Markers.size() << std::endl;
   
   // For each detected marker
   
   bool marker2 = false;
   bool marker3 = false;
   for (unsigned int m=0;m<m_Markers.size();m++) {
	   if (m_Markers[m].id == 2) {
		   marker2=true;
	   }
	   if (m_Markers[m].id == 3) {
		   marker3=true;
	   }
   }

   bool marker2_up = marker3_in_last_frame && (!marker3) && marker2 && marker2_in_last_frame;
   bool marker2_down = !marker2 && marker2_in_last_frame;
   if (marker2_up) {
	   marker2_z+= m_MarkerSize/4;
   }
   if (marker2_down) {
	   marker2_z = m_MarkerSize/4;
   }
   marker2_in_last_frame = marker2;
   marker3_in_last_frame = marker3;

   for (unsigned int m=0;m<m_Markers.size();m++)
   {
      m_Markers[m].glGetModelViewMatrix(modelview_matrix);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glLoadMatrixd(modelview_matrix);
      
      // Disabling light if it is on
      GLboolean lightOn = false;
      glGetBooleanv(GL_LIGHTING, &lightOn);
      if(lightOn) {
         glDisable(GL_LIGHTING);
      }
      
      // Drawing axis
      drawAxis(m_MarkerSize);
      
      // Drawing a cube
      glColor3f(1,0.4f,0.4f);


	  ///translater le carré pour qu'il soit flottant au dessus de marqueur 

	  float new_z = m_MarkerSize/4;
	  if (m_Markers[m].id ==2) {
		  new_z = marker2_z;
	  }
      glTranslatef(-m_MarkerSize/2, -m_MarkerSize/2, new_z);
	  
      
      glPushMatrix();
	  /// changer la couleur de carré en fonction de la distance de la caméra par rapport au marqueur
	  float red_part = m_Markers[m].getPerimeter()/1300;
	  //////création d'un carré de côté m_MerkerSize
	  glColor3f(red_part,0.7,0);
      glBegin(GL_POLYGON);
      glVertex3f(0.0f, 0.0f, 0.0f); 
      glVertex3f(m_MarkerSize,0.0f, 0.0f); 
	  glVertex3f(m_MarkerSize,m_MarkerSize, 0.0f);
	  glVertex3f(0.0f,m_MarkerSize, 0.0f);
      glEnd( );
      //glutWireCube( m_MarkerSize );

      
      // Re-enabling light if it is on
      if(lightOn) {
         glEnable(GL_LIGHTING);
      }
      
      glPopMatrix();
   }
   
   // Disabling depth test
   glDisable(GL_DEPTH_TEST);
   
   m_pixels.create(m_GlWindowSize.height , m_GlWindowSize.width, CV_8UC3);
   //use fast 4-byte alignment (default anyway) if possible
   glPixelStorei(GL_PACK_ALIGNMENT, (m_pixels.step & 3) ? 1 : 4);
   //set length of one complete row in destination data (doesn't need to equal img.cols)
   glPixelStorei(GL_PACK_ROW_LENGTH, m_pixels.step/m_pixels.elemSize());
   // Reading back the pixels
   // TODO: Changer le RGB en BGR
   glReadPixels(0, 0, m_GlWindowSize.width , m_GlWindowSize.height, GL_RGB, GL_UNSIGNED_BYTE, m_pixels.data);
   // Flip the pixels since OpenCV stores top to bottom and OpenGL from bottom to top
   cv::flip(m_pixels, m_pixels, 0);
}

// Idle function
void ArUco::idle(Mat newImage) {
   // Getting new image
   m_InputImage = newImage.clone();
   
   // Do that here ?
   resize(m_InputImage.size().width, m_InputImage.size().height);
   
   // Undistort image based on distorsion parameters
   m_UndInputImage.create(m_InputImage.size(),CV_8UC3);
   
   //transform color that by default is BGR to RGB because windows systems do not allow reading BGR images with opengl properly
   cv::cvtColor(m_InputImage,m_InputImage,CV_BGR2RGB);
   
   //remove distorion in image
   // Jim commented next line and added the clone line
   //cv::undistort(m_InputImage,m_UndInputImage, m_CameraParams.CameraMatrix, m_CameraParams.Distorsion);
   m_UndInputImage = m_InputImage.clone();
   
   //detect markers
   m_PPDetector.detect(m_UndInputImage, m_Markers, m_CameraParams.CameraMatrix, Mat(), m_MarkerSize);
   //m_PPDetector.detect(m_UndInputImage, m_Markers, m_CameraParams, m_MarkerSize);
   
   //resize the image to the size of the GL window
   cv::resize(m_UndInputImage,m_ResizedImage,m_GlWindowSize);
}

// Resize function
void ArUco::resize(GLsizei iWidth, GLsizei iHeight) {
   m_GlWindowSize=Size(iWidth,iHeight);
   
   //not all sizes are allowed. OpenCv images have padding at the end of each line in these that are not aligned to 4 bytes
   if (iWidth*3%4!=0) {
      iWidth+=iWidth*3%4;//resize to avoid padding
      resize(iWidth, m_GlWindowSize.height);
   }
   else {
      //resize the image to the size of the GL window
      //if (m_UndInputImage.rows!=0)
         //cv::resize(m_UndInputImage, m_ResizedImage, m_GlWindowSize);
   }
}


// Jim
cv::Mat ArUco::getPixels() {
   return m_pixels.clone();
}

// Test using ArUco to display a 3D cube in OpenCV
void ArUco::draw3DCube(cv::Mat img, int markerInd) {
   if(m_Markers.size() > markerInd) {
      aruco::CvDrawingUtils::draw3dCube(img, m_Markers[markerInd], m_CameraParams); 
   }
}

void ArUco::draw3DAxis(cv::Mat img, int markerInd) {
   if(m_Markers.size() > markerInd) {
      aruco::CvDrawingUtils::draw3dAxis(img, m_Markers[markerInd], m_CameraParams); 
   }
   
}

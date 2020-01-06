

// Main 
// OpenGL/GLUT
#include <iostream>
#include <Windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <opencv/cv.hpp>

// ArUco
#include "ArUco-OpenGL.h"

using namespace std;
using namespace cv;







//creation detectors & markers 




int main(int argc, char * argv[])
{
	VideoCapture cap;
	String windowNameCapture;
   // Creating the OpenCV capture
   cout << "Entrez l'identifiant de la camera" << endl;
   int cameraID;
   cin >> cameraID;
   cap.open(cameraID);
   if(!cap.isOpened()) {
      cerr << "Erreur lors de l'initialisation de la capture de la camera !"<< endl;
      cerr << "Fermeture..." << endl;
      exit(EXIT_FAILURE);

	  
   }
   // Getting the marker chosen by the user
   Mat marker_img;
   string ImgName;
   	if(argc > 1) {
		ImgName = argv[1];
		cout <<"image name: "<<ImgName<<endl;
	}
	else { 
		ImgName = "test0.png";
	}
	ImgName =  "marqueurs/" + ImgName;

	marker_img= imread(ImgName);
	cv::namedWindow("image choisie", CV_WINDOW_AUTOSIZE);

	// Markers data
	MarkerDetector myDetector1;
	MarkerDetector myDetector2;
	vector<Marker> markers1;
	vector<Marker> markers2;

	//detection of the marker on the marker_img
	myDetector1.detect (marker_img, markers1);
	for (unsigned int i =0; i<markers1.size() ; i ++) {
		cout << markers1[i] ;
		markers1[i].draw(marker_img, Scalar (0 ,0 ,255) ,2);
	}

	cv::imshow("image choisie",marker_img);
	 

   	// Getting the new image from the camera
   Mat myImage;
   while(1){
	cap.read(myImage);
	cv::namedWindow("image camera", CV_WINDOW_AUTOSIZE); 

	myDetector2.detect (myImage, markers2);
	for (unsigned int i =0; i<markers2.size() ; i ++) {
		cout << markers2[i] ;
		markers2[i].draw(myImage, Scalar (0 ,0 ,255) ,2);
	}

	cv::imshow("image camera",myImage);


	// Attente 10ms
	int key = waitKey(10);

   }
   /////////
   /*
   


   
   // detection markers
   

   // OpenCV window
   windowNameCapture = "Scene";
   cv::namedWindow(windowNameCapture, CV_WINDOW_AUTOSIZE); */
        
   return 0;
}


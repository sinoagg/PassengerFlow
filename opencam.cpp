  
  // #include "stdafx.h"  
    #include <opencv2/opencv.hpp>  
    #include <opencv2/highgui/highgui.hpp>  
    #include <opencv2/core/core.hpp>
	#include <opencv2/imgproc/imgproc.hpp>
	#include <iostream>  
    #include <stdio.h>  
    using namespace std;  
    using namespace cv;  
     /* 
     
    int main(int argc, char** argv)  
    {  
        VideoCapture cap(1); //open the default camera device  
        if(!cap.isOpened())  //check open result 
          return false;  
      
        cout<<"Original size:"<<cap.get(CV_CAP_PROP_FRAME_WIDTH)<<" "<<cap.get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;  
        cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);  
        cap.set(CV_CAP_PROP_FRAME_HEIGHT,240);  
        cap.set(CV_CAP_PROP_FPS, 10);  
        cout<<"Modified size:"<<cap.get(CV_CAP_PROP_FRAME_WIDTH)<<" "<<cap.get(CV_CAP_PROP_FRAME_HEIGHT)<<endl;  
      
            cvNamedWindow("camera show", CV_WINDOW_AUTOSIZE);  
      
        Mat frame;  
      
        while(true)  
        {  
            cap >> frame;  
            imshow("camera show", frame);  
      
            if(waitKey(10) == 27) //press key “ESC” to quit from loop  
              break;  
        }  
      
      
        return 0;  
    }  
	



int main(int argc, char** argv)
{	
	Mat frame1, frame2;
	VideoCapture cap1,cap2;
	int cont = 0;
	while (frame1.rows < 2){
		cap1.open(700);
		cap1.set(CAP_PROP_FOURCC, 'GPJM');
		cap1.set(CAP_PROP_FRAME_WIDTH, 320);
		cap1.set(CAP_PROP_FRAME_HEIGHT, 240);
		cont = 0;
		while (frame1.rows < 2 && cont<5){
			cap1 >> frame1;
			cont++;
		}
	}
	while (frame2.rows < 2){
		cap2.open(701);
		cap2.set(CAP_PROP_FOURCC, 'GPJM');
		cap2.set(CAP_PROP_FRAME_WIDTH, 320);
		cap2.set(CAP_PROP_FRAME_HEIGHT, 240);
		cont = 0;
		while (frame2.rows < 2 && cont<5){
			cap2 >> frame2;
			cont++;
		}
	}
	while (true)
	{
		cap1 >> frame1;
		cap2 >> frame2;
         
		imshow("cap1", frame1);		
		imshow("cap2", frame2);
		waitKey(60);
	}
	return 0;
}
*/
int main()
{ 
    char filenameL[1024],filenameR[1024];
    Mat input_image;
	VideoCapture cam(0);
	Rect left_rect(0, 0, 320, 240);   //创建一个Rect框，属于cv中的类，  
	Rect right_rect(320, 0, 320, 240);
	Mat image_left, image_right;
//	String left_road ;
//	String right_road ;
	int num = 0; 
 /*
 const char* dir_name="/home/zlx/peoplecounting/stereo_pc/";
       dir = opendir(dir_name);
	   cout<<"Successfully opened the dir"<<endl;*/
//initialize and allocate memory to load the video stream from camera     
  
  
  cv::VideoCapture camera1(1);  
  //camera1.set(CAP_PROP_FOURCC, 'GPJM');
  camera1.set(CV_CAP_PROP_FRAME_WIDTH,640);  
  camera1.set(CV_CAP_PROP_FRAME_HEIGHT,240);  
  camera1.set(CV_CAP_PROP_FPS, 25);

 /*
  cv::VideoCapture camera1(0);  
  camera1.set(CV_CAP_PROP_FRAME_WIDTH,640);
   camera1.set(CV_CAP_PROP_FRAME_HEIGHT,240);   

  cv::VideoCapture camera2(2);  
  camera2.set(CV_CAP_PROP_FRAME_WIDTH,640);
   camera2.set(CV_CAP_PROP_FRAME_HEIGHT,240) ;*/   
if( !camera1.isOpened() )
return 1;  
/*
if( !camera1.isOpened() )
return 1;

  if(!camera2.isOpened())
  return 1;
   while(true)
   {   
    //grab and retrieve each frames of the video sequentially      
     cv::Mat frame1;     
     camera1 >> frame1;   
  
  cv::Mat frame1;      
      camera1 >> frame1;
	  
	  cv::Mat frame2;
	  camera2 >>frame2;*/
    // cv::imshow("left", frame0);      
      
	 //cv::imshow("right", frame1);
	 //cv::imshow("mid",frame2);
     //  std::cout << frame1.rows() << std::endl;      
     //wait for 40 milliseconds      
    //int c = cvWaitKey(40);   
     //exit the loop if user press "Esc" key  (ASCII value of "Esc" is 27)      
    //if(27 == char(c))
    //break;
      
   namedWindow("摄像头");
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_JPEG_QUALITY);
	compression_params.push_back(100);
	while (true) {
		camera1 >> input_image;
		/*
		image_left = Mat(input_image, left_rect).clone();
		image_right = Mat(input_image, right_rect).clone();
		*/
	    image_left =input_image(cv::Rect(0,0,320,240));
		image_right =input_image(cv::Rect(320,0,320,240));
	    imshow("input image1", image_left);
		imshow("input image2", image_right);
	//	imwrite("alpha.png", mat, compression_params);
		if ('q' == waitKey(30))
			break;
		
		else if ( waitKey(30)=='s')
		{
			num++;
			cout << num;
          
   snprintf(filenameL,sizeof(filenameL), "image/left%02d.jpg", num );
   snprintf(filenameR,sizeof(filenameR), "image/right%02d.jpg", num);

   cv::imwrite(filenameL, image_left,compression_params);
   cv::imwrite(filenameR, image_right,compression_params);
/*
           left_road = "" + to_string(num) + ".bmp";
			right_road = "" + to_string(num) + ".bmp";
			imwrite(left_road, image_left, compression_params);
			imwrite(right_road, image_right, compression_params);
*/
}


  //  int c = cvWaitKey(40);   
     //exit the loop if user press "Esc" key  (ASCII value of "Esc" is 27)      
   // if(27 == char(c))
   // break;
    }  
   camera1.release();	
  return 0;
}

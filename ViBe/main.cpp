/*=================================================================
 * Extract Background & Foreground Model by ViBe Algorithm using OpenCV Library.
 *
 * Copyright (C) 2017 Chandler Geng. All rights reserved.
 *
 *     This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *     You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place, Suite 330, Boston, MA 02111-1307 USA
===================================================================
*/
#include "Vibe.h"
static void saveXYZ(const char* filename, const Mat& mat)
 {
     const double max_z = 1.0e4;
     FILE* fp = fopen(filename, "wt");
     for(int y = 0; y < mat.rows; y++)
     {
         for(int x = 0; x < mat.cols; x++)
         {
             Vec3f point = mat.at<Vec3f>(y, x);
             if(fabs(point[2] - max_z) < FLT_EPSILON || fabs(point[2]) > max_z) continue;
             fprintf(fp, "%f %f %f\n", point[0], point[1], point[2]);
         }
     }
     fclose(fp);
 }
//void disp2Depth(cv::Mat dispMap, cv::Mat &depthMap, cv::Mat K)
/*
void disp2Depth(cv::Mat dispMap, cv::Mat &depthMap)
{
     int type = dispMap.type();
     //intrinsic
	
     float fx = K.at<float>(0, 0);
     float fy = K.at<float>(1, 1);
     float cx = K.at<float>(0, 2);
     float cy = K.at<float>(1, 2);
    
	 float fx =618.8;
	 float fy =619.9;
	 float cx =160.0;
     float cy =120.0;
	 float baseline = 12; //基线距离65mm
 
     if (type == CV_8U)
     {
         const float PI = 3.14159265358;
         int height = dispMap.rows;
         int width = dispMap.cols;
 
         uchar* dispData = (uchar*)dispMap.data;
         ushort* depthData = (ushort*)depthMap.data;
         for (int i = 0; i < height; i++)                                    
         {
             for (int j = 0; j < width; j++)
             {
                 int id = i*width + j;
   if (!dispData[id])  continue;  //防止0除
                 depthData[id] = ushort( (float)fx *baseline / ((float)dispData[id]) );
             }
         }
     }
     else
     {
         cout << "please confirm dispImg's type!" << endl;
         cv::waitKey(0);
     }
 }                        
Mat getHistograph(const Mat grayImage)  
{  
    int channels[]={0};  
    const int histSize[]={256};  
    float range[]={0,256};  
  
    const float* ranges[]={range};  
  
    Mat hist;  
    calcHist(&grayImage,1,channels,Mat(),hist,1,histSize,ranges,true,false);  
  
    double maxValue=0;  
    minMaxLoc(hist,0,&maxValue,0,0);  
    int rows=cvRound(maxValue);  
    Mat histImage=Mat::zeros(rows,256,CV_8UC1);  
  
    for(int i=0;i<256;i++)  
    {  
        int temp=(int)(hist.at<float>(i,0));  
        if(temp)  
        {  
            histImage.col(i).rowRange(Range(rows-temp,rows))=255;   
        }  
    }  
    Mat resizeImage;  
    resize(histImage,resizeImage,Size(256,256));  
    return resizeImage;  
}  
*/
int main(int argc, char* argv[])
{
 
   Mat frame, gray,grayL,grayR, FGModelL,FGModelR,FGModel,depthMap;
   Mat depth_src,depth_dst;
   std::string  intrinsic_filename = "/home/zlx/peoplecounting/PassengerFlow/intrinsics.yml";
   std::string  extrinsic_filename = "/home/zlx/peoplecounting/PassengerFlow/extrinsics.yml";
   std::string disparity_filename = "/home/zlx/peoplecounting/PassengerFlow/disparity_filename/";
   std::string point_cloud_filename = "/home/zlx/peoplecounting/PassengerFlow/point_cloud.txt";
   float scale=1.0;
   int blocksize = 7;
   int max_disparity = 96;
   int SADWindowSize,numberOfDisparities;
   bool no_display;

   Rect roi1,roi2;
   Mat Q;
   Ptr<StereoSGBM> sgbm = StereoSGBM::create(0,16,3);
   numberOfDisparities = max_disparity;
   SADWindowSize = blocksize;
   
  //capture devices
   cv::VideoCapture capture(1);  
   capture.set(CV_CAP_PROP_FRAME_WIDTH,640);  
   capture.set(CV_CAP_PROP_FRAME_HEIGHT,240);  
   capture.set(CV_CAP_PROP_FPS, 25);
  
   if( !capture.isOpened())
    return 1;
	
   FileStorage fs(intrinsic_filename, FileStorage::READ);
       if(!fs.isOpened()){
	    printf("Failed to open file %s\n", intrinsic_filename.c_str());
             return -1;
         }
 
         Mat M1, D1, M2, D2;
         fs["M1"] >> M1;
         fs["D1"] >> D1;
         fs["M2"] >> M2;
         fs["D2"] >> D2;
 
         M1 *= scale;
         M2 *= scale;
 
         fs.open(extrinsic_filename, FileStorage::READ);
         if(!fs.isOpened())
{
             printf("Failed to open file %s\n", extrinsic_filename.c_str());
             return -1;
         }

         Mat R, T, R1, P1, R2, P2;
         fs["R"] >> R;
         fs["T"] >> T;

   
 

    // 程序运行时间统计变量
    // the Time Statistical Variable of Program Running Time
    double time;
    double start;

    ViBe vibe;
    bool count = true;

    while (1)
    {
        capture >> frame;
        if (frame.empty())
            continue;

        cvtColor(frame, gray, CV_RGB2GRAY);
        if (count)
        {
            vibe.init(gray);
            vibe.ProcessFirstFrame(gray);
            cout<<"Training ViBe Success."<<endl;
            count=false;
        }
        else
        {
 
            start = static_cast<double>(getTickCount());
            vibe.Run(gray);
            time =((double)getTickCount()-start) /getTickFrequency() * 1000;
		    cout << "Time of Update ViBe Background:" << time << "ms" <<endl<<endl;
		    FGModel = vibe.getFGModel();
		    imshow("FGModel",FGModel);
	        FGModelL = FGModel(cv::Rect(0, 0, 320, 240));
			FGModelR = FGModel(cv::Rect(320, 0, 320, 240));
		    imshow("FGModelL",FGModelL);
		    imshow("FGModelR",FGModelR);
			Size img_size = FGModelL.size();
		   //stereo_match
 stereoRectify( M1, D1, M2, D2, img_size, R, T, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, -1, img_size, &roi1, &roi2 );
 
         Mat map11, map12, map21, map22;
         initUndistortRectifyMap(M1, D1, R1, P1, img_size, CV_16SC2, map11, map12);
         initUndistortRectifyMap(M2, D2, R2, P2, img_size, CV_16SC2, map21, map22);
 
         Mat img1r, img2r;
         remap(FGModelL, img1r, map11, map12, INTER_LINEAR);
         remap(FGModelR, img2r, map21, map22, INTER_LINEAR);
 
         FGModelL = img1r;
         FGModelR = img2r;

        
 numberOfDisparities = numberOfDisparities > 0 ? numberOfDisparities : (( img_size.width/8) + 15) & -16;
 

     sgbm->setPreFilterCap(63);
     int sgbmWinSize = SADWindowSize > 0 ? SADWindowSize : 3;
     sgbm->setBlockSize(sgbmWinSize);
 
     int cn = FGModelL.channels();
 
     sgbm->setP1(8*cn*sgbmWinSize*sgbmWinSize);
     sgbm->setP2(32*cn*sgbmWinSize*sgbmWinSize);
     sgbm->setMinDisparity(0);
     sgbm->setNumDisparities(numberOfDisparities);
     sgbm->setUniquenessRatio(10);
     sgbm->setSpeckleWindowSize(100);
     sgbm->setSpeckleRange(32);
     sgbm->setDisp12MaxDiff(1);
     sgbm->setMode(StereoSGBM::MODE_SGBM);
    Mat disp, disp8;
    int64 t = getTickCount();
    sgbm->compute(FGModelL, FGModelR, disp);
    t = getTickCount() - t;
    printf("Time elapsed: %fms\n", t*1000/getTickFrequency());
    disp.convertTo(disp8, CV_8U, 255/(numberOfDisparities*16.));
     if( !no_display )
      {
         namedWindow("left", 1);
         imshow("left", FGModelL);
         namedWindow("right", 1);
         imshow("right", FGModelR);
         namedWindow("disparity", 0);
         imshow("disparity", disp8);
        // printf("press any key to continue...");
         fflush(stdout);
         waitKey(10);
         printf("\n");
     }
 if(!disparity_filename.empty())
         imwrite(disparity_filename+"%02d.jpg", disp8);

   if(!point_cloud_filename.empty())
   {
       printf("storing the point cloud...");
       fflush(stdout);
       Mat xyz;
       reprojectImageTo3D(disp, xyz, Q, true);
     //according to the height of man or woman ,segment the depth map
	  //k 是内参矩阵
      
	//   Mat K=[618.5,0.,160.8;0.,619.4,120.9; 0.,0.,1.] ;
      disp2Depth(disp8,depthMap);
     
    //median filter

	//dilate
      // dilate(depthMap,depthd,Mat());
	//eroding
      // erode(depthd,depthe,Mat());
	   //open calculate first erode,then dilate,
     morphologyEx(depthMap,depth_dst,MORPH_OPEN,Mat());
	
	//histogram
      

      Mat hist=getHistograph(depth_dst);  
      imshow("histimage",hist);   





       saveXYZ(point_cloud_filename.c_str(), xyz);
       printf("\n");
   }



        }

        imshow("input", frame);
        int c=cvWaitKey(40);
		//exit the loop if user press "Esc"key
		if(27 == char(c))
		break;
        //if ( waitKey(25) == 27 )
          //  break;
    }

    return 0;
}



#include "BGDifference.h"
int main( int argc, char** argv )
{
    //raw image
    Mat pFrame;
    // output image
    Mat pFroundImg;
    // backgroud image
    Mat pBackgroundImg;
    // advanced output image
   Mat pFroundImg_c;
    Mat pBackgroundImg_c;
     //left right image 
 //   Mat src1,src2,gray,disp8;


   BGDiff BGDif;
         VideoCapture capture("/home/zlx/PassengerFlow/my_video.avi");
         capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
         capture.set(CV_CAP_PROP_FRAME_HEIGHT,240);
   capture.set(CV_CAP_PROP_FPS,25);
 
         if(!capture.isOpened())
                 return 1;


    int nFrmNum = 0;

  int frame_num = capture.get(cv::CAP_PROP_FRAME_COUNT);
   for(int i = 0;i < frame_num-1;++i)
  {
          capture >> pFrame;
		  if(pFrame.empty())
		    continue;
            nFrmNum++;
       
        // control video
        if('q' == waitKey(10))
            break;

        // 阈值筛选后的OTSU
        BGDif.BackgroundDiff(pFrame, pFroundImg_c, pBackgroundImg_c, nFrmNum, CV_THRESH_BINARY);

        imshow("Source Video", pFrame);
        imshow("Background", pBackgroundImg_c);
        imshow("Advanced OTSU ForeGround", pFroundImg_c);
    }
 capture.release();
 waitKey(0);
 return 0;
}


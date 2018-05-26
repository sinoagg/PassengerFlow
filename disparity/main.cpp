#include <iostream>
#include "opencv2/opencv.hpp"
#include <stdio.h>


using namespace std;
using namespace cv;


int main(int argc, char* argv[])
{

		Mat frame, gray,grayL,grayR, FGModelL,FGModelR,FGModel,depthMap;
		Mat depth_src,depth_dst,src1,src2,pFrame,xyz;
		float scale=1.0;
		int blocksize = 5;
		int max_disparity = 96;
		int SADWindowSize,numberOfDisparities;
		bool no_display;

		Rect roi1,roi2;
		// Mat Q;
		Ptr<StereoSGBM> sgbm = StereoSGBM::create(0,16,3);
		numberOfDisparities = max_disparity;
		SADWindowSize = blocksize;

		//capture devices
		cv::VideoCapture capture("/home/zlx/PassengerFlow/my_video.avi");
		capture.set(CV_CAP_PROP_FRAME_WIDTH,640);  
		capture.set(CV_CAP_PROP_FRAME_HEIGHT,240);  
		capture.set(CV_CAP_PROP_FPS, 25);

		if( !capture.isOpened())
				return 1;

		Mat R1, P1, R2,P2,Q;
/*

		Mat M1  = (Mat_<double>(3,3) <<2.993453038794249e+02,0,1.730484774064881e+02,-1.431424856205310,2.995182122144020e+02,1.480356738547862e+02,0,0,1 );

		Mat D1 = (Mat_<double>(5,1) << -0.563007346069242,0.514115180115291,-0.332601703507217,-0.011642193699151,-0.008949098907040);
		Mat M2 = (Mat_<double>(3,3) << 3.001273644103927e+02,0,1.542986528692887e+02,0.424960622267953,2.997687323224824e+02,1.314150173730895e+02,0,0,1);
		Mat D2 = (Mat_<double>(5,1) << -0.416175928036569,-0.105627512213193,0.432156111676388,-0.013769706204844,-0.009456200068230);
		Mat R = (Mat_<double>(3,3) << 0.999986777425550,0.005141502112497,9.964984574866432e-05,-0.005140034177557,0.999921960954937,-0.011386485352675,-1.581857076645501e-04,0.011385822590412,0.999935166908946);

		Mat T  = (Mat_<double>(3,1) <<1.137367260586431e+02,-4.064684973366249,2.672458670038730 );


*/
Mat M1 = (Mat_<double>(3,3) << 2.879822126326698e+02,0.,1.490431105310866e+02,-2.895011775957035e-04,2.880949123425465e+02,1.241572082275463e+02,0.,0.,1. );
 
 Mat D1 = (Mat_<double>(5,1) << -0.479832263834464,0.300422476466651,-0.115130904917736,-4.937907849755521e-04,-6.257772657892366e-04);
 
  Mat M2 = (Mat_<double>(3,3) << 2.862632541687390e+02,0.,1.666971878522014e+02,0.017470776722635,2.863849999104483e+02,1.408214410552361e+02,0.,0.,1.)    ;
 
 Mat D2 = (Mat_<double>(5,1) << -0.491506870191996,0.338811016837191,-0.161962445105546,-3.066266820583723e-04,-3.334311448508388e-04 );
 
 Mat R  =(Mat_<double>(3,3) <<0.999989226969027,-7.502487901587103e-05,-0.004641154722232,1.263577328885453e-04,0.999938816639971,0.011061058284742,0.004640040906232,-0.011061525569407,0.999928053748101 );
 Mat T  = (Mat_<double>(3,1) << -1.159383285341608e+02,0.027637247722756,0.240729919556705 );

		// 程序运行时间统计变量
		// the Time Statistical Variable of Program Running Time
		double time;
		double start;

		int frame_num = capture.get(cv::CAP_PROP_FRAME_COUNT);
		for(int i = 0;i < frame_num-1;++i)
		{
				capture >> pFrame;
				if(pFrame.empty())
						continue;
				//         nFrmNum++;
				cvtColor(pFrame,gray,CV_RGB2GRAY);
				src1 = gray(cv::Rect(0,0,320,240));
				src2 = gray(cv::Rect(320,0,320,240));


				Size img_size = src1.size();



				//stereo_match
				stereoRectify( M1, D1, M2, D2, img_size, R, T, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, -1, img_size, &roi1, &roi2 );

				Mat map11, map12, map21, map22;
				initUndistortRectifyMap(M1, D1, R1, P1, img_size, CV_16SC2, map11, map12);
				initUndistortRectifyMap(M2, D2, R2, P2, img_size, CV_16SC2, map21, map22);

				Mat img1r, img2r;
				remap(src1, img1r, map11, map12, INTER_LINEAR);
				remap(src2, img2r, map21, map22, INTER_LINEAR);

				src1 = img1r;
				src2 = img2r;


				numberOfDisparities = numberOfDisparities > 0 ? numberOfDisparities : (( img_size.width/8) + 15) & -16;


				sgbm->setPreFilterCap(63);

				int sgbmWinSize = 10;
				sgbm->setBlockSize(sgbmWinSize);

				int cn = src1.channels();
				numberOfDisparities = 64;
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
				sgbm->compute(src1,src2, disp);
				t = getTickCount() - t;
				printf("Time elapsed: %fms\n", t*1000/getTickFrequency());
				disp.convertTo(disp8, CV_8U, 255/(numberOfDisparities*16.));
				reprojectImageTo3D(disp,xyz,Q,true);

				if( !no_display )
				{
						namedWindow("left", 1);
						imshow("left", src1);
						namedWindow("right", 1);
						imshow("right", src2);
						namedWindow("disparity", 0);
						imshow("disparity", disp8);
						namedWindow("xyz",0);
						imshow("xyz",xyz);
						printf("press any key to continue...");
						fflush(stdout);
						waitKey(10);
						printf("\n");
				}
				imshow("input", pFrame);
				int c=cvWaitKey(40);
				//exit the loop if user press "Esc"key
				if(27 == char(c))
						break;
		}

		capture.release();
		waitKey();
		return 0;
}


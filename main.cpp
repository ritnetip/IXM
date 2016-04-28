/*
 * main.cpp
 *
 *  Created on: Apr 24, 2016
 *      Author: Wenbo Wang
 *
 *  Description: a test project for video stitching with openCV.
 */

#include "cv.hpp"
#include "highgui/highgui.hpp"
#include "videoio.hpp"
#include "stitching.hpp"

#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace cv;
int main(int argc, char** argv) {
	cout<<"video stitching test";
	if( argc != 4)	{
	     cout <<" Usage: two video files are needed in the arguments" << endl;
	     return -1;
	}


	Mat frame_1st;
	Mat frame_2nd;

	Mat stitching_res;

	Stitcher image_stitcher = Stitcher::createDefault();

	Size res_sz;

	if (string(argv[1]) == "-i"){
		frame_1st = imread(argv[2], CV_LOAD_IMAGE_COLOR);
		frame_2nd = imread(argv[3], CV_LOAD_IMAGE_COLOR);

		if (frame_1st.empty() || frame_2nd.empty()){
			return -1;
		}

		int scale = 1;
		Mat tmp_frame;
		resize(frame_1st, tmp_frame, Size(int(frame_1st.cols / scale), int(frame_1st.rows / scale)));
		frame_1st = tmp_frame;

		tmp_frame.release();
		resize(frame_2nd, tmp_frame, Size(int(frame_2nd.cols / scale), int(frame_2nd.rows / scale)));
		frame_2nd = tmp_frame;

		namedWindow("Video 1", CV_WINDOW_AUTOSIZE);
		imshow("Video 1", frame_1st);

		namedWindow("Video 2", CV_WINDOW_AUTOSIZE);
		imshow("Video 2", frame_2nd);

		vector<Mat> input_fr_array;
		input_fr_array.push_back(frame_1st);
		input_fr_array.push_back(frame_2nd);

		if (res_sz.height != 0 && res_sz.width != 0){
			namedWindow("stitching", CV_WINDOW_AUTOSIZE);
			imshow("stitching", stitching_res);
		}

		Stitcher::Status ret_status = image_stitcher.stitch(input_fr_array, stitching_res);
		if (ret_status == Stitcher::OK){
			cout<<"succeed in stitching"<<endl;
			namedWindow("Video stitching", CV_WINDOW_AUTOSIZE);
			imshow("Video stitching", stitching_res);
			cv::moveWindow("Video stitching", 10, 50);

			imwrite("Stitching.JPG", stitching_res);
		}
		else if (ret_status == Stitcher::ERR_NEED_MORE_IMGS){
			//see what happens in the stitching process
			Stitcher::Status ret_trans = image_stitcher.estimateTransform(input_fr_array);
			if (ret_trans == Stitcher::OK){
				Stitcher::Status ret_comp = image_stitcher.composePanorama(input_fr_array, stitching_res);

				if (ret_comp == Stitcher::ERR_NEED_MORE_IMGS){
					cout<<"image composition has failed"<<endl;
				}
			}
			else{
				cout<<"Transform matrix estimation has failed"<<endl;
			}

		}
		else if (ret_status == Stitcher::ERR_HOMOGRAPHY_EST_FAIL){
			cout<<"Transform matrix estimation has failed";
		}
		else if (ret_status == Stitcher::ERR_CAMERA_PARAMS_ADJUST_FAIL){
			cout<<"Camera estimation has failed";
		}
	}
	else if (string(argv[1]) == "-v"){
		VideoCapture first_video(argv[2]);
		VideoCapture second_video(argv[3]);

		double fr_width_1st = first_video.get(CV_CAP_PROP_FRAME_WIDTH);
		double fr_height_1st = first_video.get(CV_CAP_PROP_FRAME_HEIGHT);

		double fr_width_2nd = second_video.get(CV_CAP_PROP_FRAME_WIDTH);
		double fr_height_2nd = second_video.get(CV_CAP_PROP_FRAME_HEIGHT);

		int flag_resize = 0;
		if (fr_width_1st > fr_width_2nd || fr_height_1st > fr_height_2nd){
			flag_resize = 1;
		}
		else if (fr_width_1st < fr_width_2nd || fr_height_1st < fr_height_2nd){
			flag_resize = 2;
		}

		Size frame_sz;
		if (flag_resize == 1){
			frame_sz = Size(int(fr_width_2nd), int(fr_height_2nd));
		}
		else{
			frame_sz = Size(int(fr_width_1st), int(fr_height_1st));
		}

		VideoWriter video_output;
		video_output.open("test.mp4", VideoWriter::fourcc('m','p','4','v'), 24, frame_sz);
		//reading frame sequences
		bool flag_exit = false;
		Mat tmp_frame;
		while (!flag_exit){
			first_video >> frame_1st;
			second_video >> frame_2nd;

			if (frame_1st.empty() || frame_2nd.empty()){
				flag_exit = true;
				break;
			}

			switch(flag_resize){
				case 1:{
					tmp_frame.release();
					resize(frame_1st, tmp_frame, Size(fr_width_2nd, fr_height_2nd));

					frame_1st = tmp_frame;
				}
				case 2:{
					tmp_frame.release();
					resize(frame_2nd, tmp_frame, Size(fr_width_2nd, fr_height_2nd));

					frame_2nd = tmp_frame;
				}
				default:
					//pass
					break;
			}

			vector<Mat> input_fr_array;
			input_fr_array.push_back(frame_1st);
			input_fr_array.push_back(frame_2nd);

			Stitcher::Status ret_status = image_stitcher.stitch(input_fr_array, stitching_res);
			if (ret_status == Stitcher::OK){
				cout<<"succeed in stitching"<<endl;

				res_sz = stitching_res.size();
				tmp_frame.release();

				resize(stitching_res, tmp_frame, frame_sz);

				video_output << tmp_frame;
			}
			else if (ret_status == Stitcher::ERR_NEED_MORE_IMGS){
				//see what happens in the stitching process
				Stitcher::Status ret_trans = image_stitcher.estimateTransform(input_fr_array);
				if (ret_trans == Stitcher::OK){
					Stitcher::Status ret_comp = image_stitcher.composePanorama(input_fr_array, stitching_res);

					if (ret_comp == Stitcher::ERR_NEED_MORE_IMGS){
						cout<<"image composition has failed"<<endl;
					}
				}
				else{
					cout<<"Transform matrix estimation has failed : ERR_NEED_MORE_IMGS"<<endl;
				}

			}
			else if (ret_status == Stitcher::ERR_HOMOGRAPHY_EST_FAIL){
				cout<<"Transform matrix estimation has failed: ERR_HOMOGRAPHY_EST_FAIL"<<endl;
			}
			else if (ret_status == Stitcher::ERR_CAMERA_PARAMS_ADJUST_FAIL){
				cout<<"Camera estimation has failed"<<endl;
			}
		}
	}


	waitKey();
	return 0;
}

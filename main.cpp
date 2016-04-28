/*
 * main.cpp
 *
 *  Created on: Apr 24, 2016
 *      Author: Wenbo Wang
 *
 *  Description: a test project for video stitching with openCV.
 */
#include "../Inc/VideoWrapper.hpp"
#include "../Inc/VideoWindow.hpp"
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

	Mat stitching_res;

	Stitcher image_stitcher = Stitcher::createDefault();

	Size res_sz;

	if (string(argv[1]) == "-i"){
		Mat frame_1st;
		Mat frame_2nd;

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
		Ptr<CVideoReaderWrapper> p_first_cap {new CVideoReaderWrapper(argv[2])};
		Ptr<CVideoReaderWrapper> p_second_cap {new CVideoReaderWrapper(argv[3])};

		double fr_width_1st = p_first_cap->get_frame_width();
		double fr_height_1st = p_first_cap->get_frame_height();

		double fr_width_2nd = p_second_cap->get_frame_width();
		double fr_height_2nd = p_second_cap->get_frame_height();

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

		int fourcc_code = 0x00000020;
		Ptr<CVideoWriterWrapper> pvideo_out{new CVideoWriterWrapper()};

		char dst_name[] = "test.mp4";
		pvideo_out->create_video_file(dst_name, fourcc_code, 25, frame_sz);
		//reading frame sequences

		CVideoWindowWrapper first_video_window(string("first").c_str(), p_first_cap.get());
		first_video_window.resize(800, 600);
		CVideoWindowWrapper second_video_window(string("second").c_str(), p_second_cap.get());
		second_video_window.resize(800, 600);

		char key_ret = cvWaitKey(0);

		if (key_ret > 0){
			int pos_first_frame = first_video_window.get_current_frame_pos();
			int pos_second_frame = second_video_window.get_current_frame_pos();

			cout<<"fisrt pos: "<<pos_first_frame<<" second pos: "<<pos_second_frame<<endl;

			bool flag_exit = false;
			Mat tmp_frame;
			while (!flag_exit){
				Ptr<Mat> first_frame = p_first_cap->get_frame(pos_first_frame++);
				Ptr<Mat> second_frame = p_second_cap->get_frame(pos_second_frame++);

				if (first_frame->empty() || second_frame->empty()){
					flag_exit = true;
					break;
				}

				switch(flag_resize){
					case 1:{
						tmp_frame.release();
						resize(*first_frame, tmp_frame, Size(fr_width_2nd, fr_height_2nd));

						first_frame.reset(& tmp_frame);
						break;
					}
					case 2:{
						tmp_frame.release();
						resize(*second_frame, tmp_frame, Size(fr_width_2nd, fr_height_2nd));

						second_frame.reset(& tmp_frame);
						break;
					}
					default:
						//pass
						break;
				}

				vector<Mat> input_fr_array;
				input_fr_array.push_back(*first_frame);
				input_fr_array.push_back(*second_frame);

				Stitcher::Status ret_status = image_stitcher.stitch(input_fr_array, stitching_res);
				if (ret_status == Stitcher::OK){
					cout<<"succeed in stitching"<<endl;

					res_sz = stitching_res.size();
					tmp_frame.release();

					resize(stitching_res, tmp_frame, frame_sz);

					pvideo_out->Write(tmp_frame, true);
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
	}

	destroyAllWindows();
	return 0;
}

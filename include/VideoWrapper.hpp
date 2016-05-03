/*
 * VideoWrapper.hpp
 *
 *  Created on: Apr 27, 2016
 *      Author: Wenbo Wang
 *
 *      Memo:
 *      (1) The code is not multi-thread safe yet.
 *      (2) Need to switch to smart pointer but my current MinGW doesn't support c++11
 */

#ifndef INC_VIDEOWRAPPER_HPP_
#define INC_VIDEOWRAPPER_HPP_
#pragma once

#include "core/cvstd.hpp"
#include "videoio.hpp"
#include "cv.hpp"
#include "opencv2/opencv.hpp"

class CVideoReaderWrapper{
public:
	CVideoReaderWrapper();
	CVideoReaderWrapper(const char* video_name);
	~CVideoReaderWrapper();

	bool set_video_src(const char* video_name);

	inline int get_frame_count() {return m_frame_count;};
	inline double get_frame_width() {return m_frame_width;};
	inline double get_frame_height() {return m_frame_height;};
	inline double get_fourcc_code() {return (int)m_video_fourcc;};

	cv::Ptr<cv::Mat> get_frame(int frame_id);
	cv::Ptr<cv::Mat> get_next_frame();

protected:
	cv::Ptr<cv::VideoCapture> m_pvideo_cap;
	int m_frame_count;

	double m_frame_width;
	double m_frame_height;
	double m_video_fourcc;
};


class CVideoWriterWrapper{
public:
	CVideoWriterWrapper();
	CVideoWriterWrapper(const char* file_name, int fourcc, double fps, cv::Size frame_size);
	~CVideoWriterWrapper();

	bool create_video_file(const char* file_name, int fourcc, double fps, cv::Size frame_size);

	bool Write(cv::Mat* pinput_frame);
	bool Write(const cv::Mat& input_frame, bool bforce_writting=false);
protected:
	cv::Ptr<cv::VideoWriter> m_pvideo_writer;
	int m_fourcc;
	double m_fps;
	cv::Size m_frame_sz;
};

#endif /* INC_VIDEOWRAPPER_HPP_ */

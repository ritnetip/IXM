/*
 * CVideoWrapper.cpp
 *
 *  Created on: Apr 27, 2016
 *      Author: Wenbo
 */

#include "videoio.hpp"
#include "cv.hpp"
#include "highgui/highgui.hpp"

#include "../include/VideoWrapper.hpp"
#include <string>

using namespace cv;
using namespace std;

CVideoReaderWrapper::CVideoReaderWrapper(): m_frame_count{0}, m_frame_width{0},m_frame_height{0}, m_video_fourcc{-1}
{
}

CVideoReaderWrapper::CVideoReaderWrapper(const char* video_name) : m_pvideo_cap{new VideoCapture(video_name)}
{
	if (!m_pvideo_cap->isOpened()){
		m_frame_count = 0;
		m_pvideo_cap->release();
		m_pvideo_cap.release();

		m_frame_count = 0;
		m_frame_width = 0;
		m_frame_height = 0;
		m_video_fourcc = -1;
	}
	else{
		m_frame_count = m_pvideo_cap->get(CAP_PROP_FRAME_COUNT);

		m_frame_width = m_pvideo_cap->get(CAP_PROP_FRAME_WIDTH);
		m_frame_height = m_pvideo_cap->get(CAP_PROP_FRAME_HEIGHT);

		m_video_fourcc = m_pvideo_cap->get(CAP_PROP_FOURCC);
	}
}

CVideoReaderWrapper::~CVideoReaderWrapper(){
	if (!m_pvideo_cap.empty()){
		m_pvideo_cap->release();
	}
}

bool CVideoReaderWrapper::set_video_src(const char* video_name){
	if (!m_pvideo_cap.empty()){
		m_pvideo_cap->release();
	}

	m_pvideo_cap.reset(new VideoCapture(video_name));

	if (!m_pvideo_cap->isOpened()){
		m_frame_count = 0;
		m_pvideo_cap->release();;
		m_pvideo_cap.release();

		return false;
	}

	m_frame_count = m_pvideo_cap->get(CAP_PROP_FRAME_COUNT);

	m_frame_width = m_pvideo_cap->get(CAP_PROP_FRAME_WIDTH);
	m_frame_height = m_pvideo_cap->get(CAP_PROP_FRAME_HEIGHT);

	m_video_fourcc = m_pvideo_cap->get(CAP_PROP_FOURCC);
	return true;
}

Ptr<Mat> CVideoReaderWrapper::get_frame(int frame_id){
	Ptr<Mat> pframe{new Mat()};
	if(!m_pvideo_cap.empty()){
		m_pvideo_cap->set(CAP_PROP_POS_FRAMES, frame_id);

		m_pvideo_cap->read(*pframe);
	}

	return pframe;
}

Ptr<Mat> CVideoReaderWrapper::get_next_frame(){
	Ptr<Mat> pframe{new Mat()};

	if(!m_pvideo_cap.empty()){
		if (m_pvideo_cap->grab()){
			m_pvideo_cap->retrieve(*pframe);
		}
	}

	return pframe;
}


CVideoWriterWrapper::CVideoWriterWrapper():
		m_pvideo_writer{new VideoWriter()},
		m_fourcc{-1},
		m_fps(0),
		m_frame_sz{Size(int(0),
		int(0))}
{
}

CVideoWriterWrapper::CVideoWriterWrapper(const char* file_name, int fourcc, double fps, cv::Size frame_size):
	m_pvideo_writer{new VideoWriter()}
{
	string str_file_name(file_name);

	m_pvideo_writer->open(str_file_name, fourcc, fps, frame_size);

	if(!m_pvideo_writer.empty() && !m_pvideo_writer->isOpened()){
		m_fourcc = -1;
		m_fps = 0;
		m_frame_sz.height = 0;
		m_frame_sz.width = 0;
	}
	else{
		m_fourcc = fourcc;
		m_fps = fps;
		m_frame_sz = frame_size;
	}
}

CVideoWriterWrapper::~CVideoWriterWrapper(){
	if (!m_pvideo_writer.empty()){
		m_pvideo_writer->release();
	}
}

bool CVideoWriterWrapper::create_video_file(const char* file_name, int fourcc, double fps, cv::Size frame_size){
	if (!m_pvideo_writer.empty()){
		m_pvideo_writer->release();
	}

	bool bret = m_pvideo_writer->open(file_name, fourcc, fps, frame_size);

	if (! bret){
		m_fourcc = -1;
		m_fps = 0;
		m_frame_sz.height = 0;
		m_frame_sz.width = 0;

		return false;
	}

	m_fourcc = fourcc;
	m_fps = fps;
	m_frame_sz = frame_size;

	return true;
}

bool CVideoWriterWrapper::Write(Mat* pinput_frame){
	if (! m_pvideo_writer->isOpened() || pinput_frame == NULL || pinput_frame->empty()){
		return false;
	}

	m_pvideo_writer->write(*pinput_frame);

	return true;
}

bool CVideoWriterWrapper::Write(const Mat& input_frame, bool bforce_writting){
	if (!bforce_writting && ( ! m_pvideo_writer->isOpened() || input_frame.empty() ) ){
		return false;
	}

	m_pvideo_writer->write(input_frame);

	return true;
}

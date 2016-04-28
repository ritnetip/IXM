/*
 * VideoWindow.cpp
 *
 *  Created on: Apr 27, 2016
 *      Author: Wenbo
 */

#include "../Inc/VideoWindow.hpp"
#include "../Inc/VideoWrapper.hpp"
#include "highgui/highgui.hpp"

using namespace std;
using namespace cv;

CVideoWindowWrapper::CVideoWindowWrapper(const char* window_name){
	m_window_name = window_name;
	m_tracker_bar_name = string(window_name) + string("_tracker_bar");
	m_frame_count = 0;
	m_ref_bar_pos = 0;
}

CVideoWindowWrapper::CVideoWindowWrapper(const char* window_name, CVideoReaderWrapper* pSrc) {
	m_window_name = window_name;
	m_tracker_bar_name = string(window_name) + string("_tracker_bar");
	if (NULL != pSrc){
		m_pvideo_src.reset(pSrc);

		m_frame_count = m_pvideo_src->get_frame_count();
		m_ref_bar_pos = 0;

		namedWindow(m_window_name, WINDOW_NORMAL  | CV_GUI_NORMAL);
		createTrackbar(m_tracker_bar_name.c_str(), m_window_name.c_str(), &m_ref_bar_pos, m_frame_count,
				&CVideoWindowWrapper::tracker_callback, (void*)this);
	}
}

CVideoWindowWrapper::~CVideoWindowWrapper(){
	try{
		destroyWindow(m_window_name);

		m_pvideo_src.release();
	}
	catch(...){
		//TODO: A bug occurs when releasing the resources. But I cannot locate it.
		cout<<"problem"<<endl;
	}
}

void CVideoWindowWrapper::attach_video(CVideoReaderWrapper* pSrc){
	m_pvideo_src.reset(pSrc);
	m_frame_count = m_pvideo_src->get_frame_count();

	createTrackbar(m_tracker_bar_name.c_str(), m_window_name.c_str(), &m_ref_bar_pos, m_frame_count,
			&CVideoWindowWrapper::tracker_callback, (void*)this);
}

int CVideoWindowWrapper::get_current_frame_pos(){
	return m_ref_bar_pos;
}


bool CVideoWindowWrapper::resize(int width, int height){
	if (NULL != m_pvideo_src.get()){
		resizeWindow(m_window_name, width, height);
		return true;
	}

	return false;
}

void CVideoWindowWrapper::tracker_callback(int pos, void* pCB_data){
	if (NULL == pCB_data){
		return;
	}

	CVideoWindowWrapper* pWindowWrapper = (CVideoWindowWrapper*)pCB_data;

	Ptr<Mat> current_frame = pWindowWrapper->m_pvideo_src->get_frame(pos);

	pWindowWrapper->m_ref_bar_pos = pos;
	imshow(pWindowWrapper->m_window_name, *current_frame.get());
}

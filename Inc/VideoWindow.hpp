/*
 * VideoWindow.hpp
 *
 *  Created on: Apr 27, 2016
 *      Author: Wenbo
 */

#ifndef INC_VIDEOWINDOW_HPP_
#define INC_VIDEOWINDOW_HPP_

#pragma once

#include "core/cvstd.hpp"
#include <string>

class CVideoReaderWrapper;

class CVideoWindowWrapper{
public:
	CVideoWindowWrapper(const char* window_name);
	CVideoWindowWrapper(const char* window_name, cv::Ptr<CVideoReaderWrapper> pSrc);
	~CVideoWindowWrapper();

	bool attach_video(cv::Ptr<CVideoReaderWrapper> pSrc);
	inline int get_current_frame_pos() {return m_ref_bar_pos;};

	bool resize(int width, int height);

protected:
	static void tracker_callback(int pos, void* pCB_data=NULL);

	std::string m_window_name;
	std::string m_tracker_bar_name;

	cv::Ptr<CVideoReaderWrapper> m_pvideo_src;

	int m_frame_count;
	int m_ref_bar_pos;

};


#endif /* INC_VIDEOWINDOW_HPP_ */

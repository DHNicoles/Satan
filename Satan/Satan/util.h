/************************************************************************/
/* tools                                                                      */
/************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include "opencv2/contrib/contrib.hpp"
#include <string>
#include <vector>     
#include <algorithm>
#include <assert.h>
#include <caffe/caffe.hpp>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <utility>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>  
}

namespace ice
{
	class KernelAlg;

	/************************************************************************/
	/* 
	filepath	:	read file names from 
	videoList	:	save file name list
	*/
	/************************************************************************/
	int  getFileList(std::string filepath, std::vector<std::string> &videoList);
	/************************************************************************/
	/* parse or paramter                                                    */
	/************************************************************************/
	void parseOrDie(const char* configPath, KernelAlg& alg, std::vector<std::string>& file_list);
}
#endif // __UTIL_H__

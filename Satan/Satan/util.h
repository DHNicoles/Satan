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

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>  
}

namespace ice
{
	/************************************************************************/
	/* 
	filepath	:	read file names from 
	videoList	:	save file name list
	*/
	/************************************************************************/
	int  getFileList(std::string filepath, std::vector<std::string> &videoList);

}
#endif // __UTIL_H__

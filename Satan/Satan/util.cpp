#include "util.h"

namespace ice
{
	int getFileList(std::string filepath, std::vector<std::string>& videoList)
	{
		cv::Directory dir;
		std::string extenl = "*";//"*"  
		bool addPath = false;//true;  

		videoList = dir.GetListFiles(filepath, extenl, addPath);
		for (int i = 0; i < videoList.size(); i++) {
			std::string temp = filepath + "\\" + videoList[i].c_str();
			videoList[i] = temp;
		}
		std::sort(videoList.begin(), videoList.end());
		return videoList.empty();
	}
}

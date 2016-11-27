#include "util.h"
#include "kernel_alg.h"

namespace ice
{
	int getFileList(std::string filepath, std::vector<std::string>& videoList)
	{
		cv::Directory dir;
		std::string extenl = "*";//"*"  
		bool addPath = false;//true;  

		videoList = dir.GetListFiles(filepath, extenl, addPath);
		for (int i = 0; i < videoList.size(); i++) {
			std::string temp = filepath + "/" + videoList[i].c_str();
			videoList[i] = temp;
		}
		std::sort(videoList.begin(), videoList.end());
		return videoList.empty();
	}
	void parseOrDie(const char * configPath, KernelAlg & alg, std::vector<std::string>& file_list)
	{
		std::ifstream config_file(configPath);
		char buffer[512];
		int line_number = 1;
		std::string caffe_config[3];
		while (!config_file.eof())
		{
			config_file.getline(buffer, 500);
			std::stringstream line(buffer);
			if (line.str().empty())
				break;
			std::string str;
			line >> str;
			if(line_number < 4)
				caffe_config[line_number - 1] = str;
			if (line_number == 3)
			{
				alg.OnInit(caffe_config[0], caffe_config[1], "", caffe_config[2]);
				LOG(INFO) << "line#1:	" <<caffe_config[0]<< std::endl;
				LOG(INFO) << "line#2:	" <<caffe_config[1]<< std::endl;
				LOG(INFO) << "line#3:	" <<caffe_config[2]<< std::endl;
			}
			if (line_number == 4)
			{
				LOG(INFO) << "line#4:	" <<str<< std::endl;
				ice::getFileList(str.c_str(), file_list);
			}
			++line_number;
		}
		if (line_number != 5)
		{
			LOG(INFO) << "prase failed." << std::endl;
			exit(-1);
		}
		LOG(INFO) << "parse success."<< std::endl;
	}
}

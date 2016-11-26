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
			std::string temp = filepath + "\\" + videoList[i].c_str();
			videoList[i] = temp;
		}
		std::sort(videoList.begin(), videoList.end());
		return videoList.empty();
	}
	void parseOrDie(const char * configPath, KernelAlg & alg, std::vector<std::string>& file_list)
	{
		std::ifstream config_file(configPath);
		char buffer[300];
		int line_number = 1;
		std::string caffe_config[3];
		while (!config_file.eof())
		{
			config_file.getline(buffer, 256);
			std::stringstream line(buffer);
			if (line.str().empty())
				break;
			std::string str;
			line >> str;
			caffe_config[line_number - 1] = str;
			if (line_number == 3)
			{
				alg.OnInit(caffe_config[0], caffe_config[1], "", caffe_config[2]);
			}
			if (line_number == 4)
			{
				ice::getFileList(str.c_str(), file_list);
			}
			++line_number;
		}
		if (line_number != 5)
		{
			std::cout << "prase failed." << std::endl;
			exit(-1);
		}
	}
}

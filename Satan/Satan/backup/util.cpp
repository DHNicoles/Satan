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
	int Distance(cv::Rect & r1, cv::Rect & r2)
	{
		int cent_x_1 = r1.x + (r1.width >> 1);
		int cent_y_1 = r1.y + (r1.height >> 1);
		int cent_x_2 = r2.x + (r2.width >> 1);
		int cent_y_2 = r2.y + (r2.height >> 1);
		return std::abs(cent_x_1 - cent_x_2) + std::abs(cent_y_1 - cent_y_2);
	}
	int Distance(cv::Rect & r1, cv::Point & pos)
	{
		int cent_x_1 = r1.x + (r1.width >> 1);
		int cent_y_1 = r1.y + (r1.height >> 1);
		return std::abs(cent_x_1 - pos.x) + std::abs(cent_y_1 - pos.y);
	}
	int Distance(cv::Point & pos1, cv::Point & pos2)
	{
		return std::abs(pos1.x - pos2.x) + std::abs(pos1.y - pos2.y);
	}
}


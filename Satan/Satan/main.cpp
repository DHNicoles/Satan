#include "util.h"
#include "video_capture.h"
int main(int argc, char *argv[])
{
	if (argc < 1)
	{
		std::cout << "you should pass the dircetory path!" << std::endl;
		return -1;
	}
	std::vector<std::string> file_list;
	ice::getFileList(argv[1], file_list);
	ice::VideoCapture cap;

	unsigned int frame_count = 0;
	cv::Mat frame;

	for (int i = 0; i < file_list.size(); ++i)
	{
		std::cout << file_list[i] << std::endl;
		cap.Open(file_list[i].c_str());
		if (!cap.isOpened())
		{
			std::cout << "VideoCapture initialisze failed." << std::endl;
		}
		while (true)
		{
			//cap.read(frame);
			for (int s = 0; s < step; ++s)
			{
				cap >> frame;
				if (frame.empty()) break;
			}
			if (frame.empty()) break;
			cv::Mat crop_img = frame(cv::Rect(450, 0, 720, 720));

			frame_count++;
		}
	}

}
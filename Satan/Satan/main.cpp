#include "util.h"
#include "video_capture.h"
#include "kernel_alg.h"
/************************************************************************/
/* argv--->>> config.txt 
deploy.prototxt
xx.caffemodel
mean_file
video directory
*/
/************************************************************************/
int main(int argc, char *argv[])
{
	if (argc < 1)
	{
		std::cout << "you should pass the dircetory path!" << std::endl;
		return -1;
	}

	ice::KernelAlg alg;
	std::vector<std::string> file_list;
	parseOrDie(argv[1], alg, file_list);

	unsigned int frame_count = 0;
	ice::VideoCapture cap;
	cv::Mat frame;

	for (int i = 0; i < file_list.size(); ++i)
	{
		std::cout << file_list[i] << std::endl;
		cap.Open(file_list[i].c_str());
		if (!cap.isOpened())
		{
			std::cout << "VideoCapture initialisze failed." << std::endl;
		}
		cap >> frame;
		while (!frame.empty())
		{
			cv::Mat crop_img = frame(cv::Rect(450, 0, 720, 720));
			alg.DetectAndTrack(crop_img);
			frame_count++;
			cap >> frame;
		}
	}

}
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
		LOG(ERROR) << "you should pass the dircetory path!" << std::endl;
		return -1;
	}

	ice::KernelAlg alg;
	std::vector<std::string> file_list;
	parseOrDie(argv[1], alg, file_list);

	cv::Rect roi(450, 0, 720, 720);
	cv::Rect bound(60, 60, 600, 600);

	alg.SetBound(cv::Size(720, 720), bound);

	unsigned int frame_count = 0;
	ice::VideoCapture cap;
	cv::Mat frame;
	ice::WatchTime T0;
	for (int i = 0; i < file_list.size(); ++i)
	{
		LOG(INFO) << "open video " << file_list[i];
		cap.Open(file_list[i].c_str());
		if (!cap.isOpened())
		{
			LOG(ERROR) << "VideoCapture initialisze failed." << std::endl;
		}
		cap >> frame;
		while (!frame.empty())
		{
			cv::Mat crop_img = frame(roi);
			T0.Reset();	T0.Start();
			alg.DetectAndTrack(crop_img);
			frame_count++;
			T0.Stop();
			LOG(INFO) << "#" << frame_count << "\t->\t" << T0.GetTime();
			cap >> frame;
		}
	}
	return 0;

}

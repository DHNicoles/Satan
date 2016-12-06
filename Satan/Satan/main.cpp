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

	cv::Rect roi(TOP_LEFT_X, TOP_LEFT_Y, WIDTH, HIGHT);
	cv::Rect bound((WIDTH - BOUND_WIDTH) / 2, (WIDTH - BOUND_WIDTH) / 2, BOUND_WIDTH, BOUND_HIGHT);

	alg.SetBound(cv::Size(roi.width, roi.height), bound);

	unsigned int frame_count = 0;
	double time_unit = 1000 / 25;
	ice::VideoCapture cap;
	cv::Mat frame;
	ice::WatchTime T0;
	for (int i = 0; i < file_list.size(); ++i)
	{
		std::string video_name = file_list[i].substr(file_list[i].size() - 32);
		size_t video_frame_count = 0;
		LOG(INFO) << "open video " << file_list[i];
		cap.Open(file_list[i].c_str());
		if (!cap.isOpened())
		{
			LOG(ERROR) << "VideoCapture initialize failed." << std::endl;
		}
		cap >> frame;
		while (!frame.empty())
		{
			cv::Mat crop_img = frame(roi);
			T0.Reset();	T0.Start();
			alg.OnUpdate(crop_img, video_name, video_frame_count*time_unit);
			frame_count++;
			T0.Stop();
			LOG(INFO) << "#" << frame_count << "\t->\t" << T0.GetTime();
			cap >> frame;
		}
	}
	return 0;

}

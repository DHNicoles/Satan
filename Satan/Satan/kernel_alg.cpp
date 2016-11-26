#include "kernel_alg.h"
#include "ssd_detector.h"
namespace ice
{
	KernelAlg::KernelAlg() :
		detector_ptr_(NULL)
	{
	}

	KernelAlg::~KernelAlg()
	{
		OnDestroy();
	}

	void KernelAlg::OnInit(
		const std::string& model_file,
		const std::string& weights_file,
		const std::string& mean_file,
		const std::string& mean_value)
	{
		detector_ptr_ = new Detector;
		detector_ptr_->OnInit(model_file, weights_file, mean_file, mean_value);
	}

	void KernelAlg::OnDestroy()
	{
		if (detector_ptr_)
		{
			delete detector_ptr_;
		}
		detector_ptr_ = NULL;
	}

	void KernelAlg::DetectAndTrack(cv::Mat & frame)
	{
		static unsigned int frame_count = 0;
		static char buffer[40];
		//trackers to update
		
		//multi_trackers_.OnUpdate(frame);
		
		//detector
		//detect head and draw
		std::vector<std::vector<float> > detected_boxes;
		detector_ptr_->Detect(frame, detected_boxes);


		for (int i = 0; i < detected_boxes.size(); i++)
		{
			std::vector<float> box = detected_boxes.at(i);
			int  width = (int)(box.at(2) - box.at(0));
			int  height = (int)(box.at(3) - box.at(1));
			cv::rectangle(frame, cv::Rect((int)box.at(0), (int)box.at(1), width, height), cv::Scalar(255, 255, 255), 2, 1, 0);
		}
		sprintf(buffer, "/home/lt/cheguangfu/count/Output/detect_%d.jpg", frame_count++);
		imwrite(buffer, frame);
	}
}

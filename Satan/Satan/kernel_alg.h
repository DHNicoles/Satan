/************************************************************************/
/* detect and track process .                                           */
/************************************************************************/

#ifndef __KERNEL_ALG_H__
#define __KERNEL_ALG_H__

#include "util.h"
#include "multi_tracker.h"
namespace ice
{
	struct ThreadParam;
	class Detector;
	class Detect_behavior;
	class Counter;
	class KernelAlg
	{
	public:
		KernelAlg();
		~KernelAlg();
		void OnInit(const std::string& model_file, const std::string& weights_file, const std::string& mean_file, const std::string& mean_value);
		void OnDestroy();
		void OnUpdate(cv::Mat& frame, std::string& filename, size_t offset);
		void SetBound(cv::Size, cv::Rect);
		//static void *thread(void *ptr);
	private:
		void Match(std::vector<cv::Rect>& obj_boxes, std::vector<float>& scores, cv::Mat& frame);
	private:
		Detector* detector_ptr_;
		Detect_behavior* behavior_ptr_;
		MultiTracker multi_trackers_;
		Counter* region_counter_;
	};

	struct ThreadParam
	{
		KernelAlg* kerner_alg_ptr;
		std::string filename;
		size_t offset;
	};
}

#endif // __KERNEL_ALG_H__

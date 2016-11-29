/************************************************************************/
/* detect and track process .                                           */
/************************************************************************/

#ifndef __KERNEL_ALG_H__
#define __KERNEL_ALG_H__

#include "util.h"
#include "multi_tracker.h"
namespace ice
{
	class Detector;
	class KernelAlg
	{
	public:
		KernelAlg();
		~KernelAlg();
		void OnInit(const std::string& model_file, const std::string& weights_file, const std::string& mean_file, const std::string& mean_value);
		void OnDestroy();
		void DetectAndTrack(cv::Mat& frame, bool update_trackers = true, bool update_detector = true);
		void SetBound(cv::Size, cv::Rect);
	private:
		void Match(std::vector<cv::Rect>& obj_boxes, std::vector<float>& scores, cv::Mat& frame);
	private:
		Detector* detector_ptr_;
		MultiTracker multi_trackers_;
	};
}

#endif // __KERNEL_ALG_H__

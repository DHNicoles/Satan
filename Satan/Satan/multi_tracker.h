/************************************************************************/
/* Multi-tracker based on KCF                                           */
/************************************************************************/
#ifndef __MULTI_TRACKER_H__
#define __MULTI_TRACKER_H__

#include "util.h"

class KCFTracker;
namespace ice
{
	class ObjectInfo;

	class MultiTracker
	{
	public:
		MultiTracker();
		~MultiTracker();
		void OnInit();
		void OnDestroy();
		void AddTracker(cv::Mat& frame, cv::Rect& positionBox);
		void OnUpdate(cv::Mat & frame);

	private:
		int GetIndex_();
	private:
		std::map<size_t, KCFTracker*> traker_map_;
		std::map<size_t, ObjectInfo*> object_info_map_;
		size_t key_max_;
		std::set<size_t> index_set_;
		const bool HOG_ = true;
		const bool FIXEDWINDOW_ = false;
		const bool MULTISCALE_ = true;
		const bool SILENT_ = true;
		const bool LAB_ = false;
	};
}


#endif // __MULTI_TRACKER_H__

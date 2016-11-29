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
		void AddTracker(cv::Mat& frame, cv::Rect& positionBox, float score);
		void OnUpdate(cv::Mat & frame);
		void Mark(size_t index);
		void RemoveInvalid();
		void Replace(cv::Mat& frame, cv::Rect& positionBox, size_t index);
		void DrawTrace(cv::Mat& frame);
		std::map<size_t, ObjectInfo*>& TrackingObectorMap() { return object_info_map_; }
		void SetAllMatchFalse();
		void SetMatchTrue(size_t index);
		void SetBound(cv::Size, cv::Rect);
	private:
		int GetIndex_();
		void RemoveTrackerByIndex_(size_t index);
	private:
		std::map<size_t, KCFTracker*> traker_map_;
		std::map<size_t, ObjectInfo*> object_info_map_;
		std::list<size_t> invalid_index_list_;
		size_t key_max_;
		std::set<size_t> index_pool_set_;
		cv::Mat bound_mask_;
		const bool HOG_ = true;
		const bool FIXEDWINDOW_ = false;
		const bool MULTISCALE_ = true;
		const bool SILENT_ = true;
		const bool LAB_ = false;
	};
}


#endif // __MULTI_TRACKER_H__

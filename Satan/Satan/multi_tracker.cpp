#include "multi_tracker.h"
#include "kcftracker.hpp"
#include "object_info.h"

namespace ice
{
	MultiTracker::MultiTracker() :
		key_max_(0)
	{
		OnInit();
	}
	MultiTracker::~MultiTracker()
	{
	}
	void MultiTracker::OnInit()
	{
		OnDestroy();
	}
	void MultiTracker::OnDestroy()
	{
		//delete kcf_trakers
		std::map<size_t, KCFTracker*>::iterator tracker_itr = traker_map_.begin();
		std::map<size_t, KCFTracker*>::iterator tracker_end_itr = traker_map_.end();
		for (; tracker_itr != tracker_end_itr; ++tracker_itr)
		{
			if (tracker_itr->second)
				delete tracker_itr->second;
			tracker_itr->second = NULL;
		}
		//delete object
		//std::map<size_t, ObjectInfo*> object_info_map_;
		std::map<size_t, ObjectInfo*>::iterator obj_itr = object_info_map_.begin();
		std::map<size_t, ObjectInfo*>::iterator obj_end_itr = object_info_map_.end();
		for (; obj_itr != obj_end_itr; ++obj_itr)
		{
			if (obj_itr->second)
				delete obj_itr->second;
			obj_itr->second = NULL;
		}
	}
	void MultiTracker::AddTracker(cv::Mat & frame, cv::Rect & positionBox)
	{
		int index = GetIndex_();
		KCFTracker* tracker_ptr = new KCFTracker(HOG_, FIXEDWINDOW_, MULTISCALE_, LAB_);
	}
	void MultiTracker::OnUpdate(cv::Mat & frame)
	{
		//update kcf_trakers and object information
		std::map<size_t, KCFTracker*>::iterator tracker_itr = traker_map_.begin();
		std::map<size_t, KCFTracker*>::iterator tracker_end_itr = traker_map_.end();
		for (; tracker_itr != tracker_end_itr; ++tracker_itr)
		{
			cv::Rect pos = tracker_itr->second->update(frame);
			int cent_x = pos.x + (pos.width >> 1);
			int cent_y = pos.y + (pos.height >> 1);
			object_info_map_[tracker_itr->first]->GetTrace().push_back(cv::Point(cent_x, cent_y));
		}
	}
	int MultiTracker::GetIndex_()
	{

		if (index_set_.empty())
		{
			return ++key_max_;
		}
		else
		{
			int index = *(index_set_.begin());
			index_set_.erase(index_set_.begin());
			return index;
		}
	}
}

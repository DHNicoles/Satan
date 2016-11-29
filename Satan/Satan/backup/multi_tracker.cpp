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
		OnDestroy();
	}
	void MultiTracker::OnInit()
	{

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
		std::map<size_t, ObjectInfo*>::iterator obj_itr = object_info_map_.begin();
		std::map<size_t, ObjectInfo*>::iterator obj_end_itr = object_info_map_.end();
		for (; obj_itr != obj_end_itr; ++obj_itr)
		{
			if (obj_itr->second)
				delete obj_itr->second;
			obj_itr->second = NULL;
		}
	}
	void MultiTracker::AddTracker(cv::Mat & frame, cv::Rect & positionBox, float score)
	{
		int index = GetIndex_();
		LOG(INFO) << "add tracker GetIndex_=" << index;
		KCFTracker* tracker_ptr = new KCFTracker(HOG_, FIXEDWINDOW_, MULTISCALE_, LAB_);
		tracker_ptr->init(positionBox, frame);
		int cent_x = positionBox.x + (positionBox.width >> 1);
		int cent_y = positionBox.y + (positionBox.height >> 1);
		traker_map_[index] = tracker_ptr;
		object_info_map_[index] = new ObjectInfo(cv::Point(cent_x, cent_y), positionBox, score);
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
			object_info_map_[tracker_itr->first]->Box() = pos;
		}
		//update object information
		std::map<size_t, ObjectInfo*>::iterator obj_itr = object_info_map_.begin();
		std::map<size_t, ObjectInfo*>::iterator obj_end_itr = object_info_map_.end();
		for (; obj_itr != obj_end_itr; ++obj_itr)
		{
			obj_itr->second->OnUpdate(bound_mask_);
		}
	}
	void MultiTracker::Mark(size_t index)
	{

	}
	void MultiTracker::RemoveInvalid()
	{
		std::map<size_t, ObjectInfo*>::iterator obj_itr = object_info_map_.begin();
		std::map<size_t, ObjectInfo*>::iterator obj_end_itr = object_info_map_.end();
		for (; obj_itr != obj_end_itr; ++obj_itr)
		{
			if (obj_itr->second->IsShit())
				invalid_index_list_.push_back(obj_itr->first);
		}
		//remove invalid index
		std::list<size_t>::iterator iil_it = invalid_index_list_.begin();
		std::list<size_t>::iterator iil_end_it = invalid_index_list_.end();
		for (; iil_it != iil_end_it; ++iil_it)
		{
			size_t idx = *iil_it;
			LOG(INFO) << "remove tracker index=" << idx;
			RemoveTrackerByIndex_(idx);
			index_pool_set_.insert(idx);
		}
		invalid_index_list_.clear();
	}
	void MultiTracker::Replace(cv::Mat & frame, cv::Rect & positionBox, size_t index)
	{

		std::map<size_t, KCFTracker*>::iterator tracker_itr = traker_map_.find(index);
		if (tracker_itr != traker_map_.end())
		{
			if (tracker_itr->second)
			{
				delete tracker_itr->second;
				KCFTracker* tracker_ptr = new KCFTracker(HOG_, FIXEDWINDOW_, MULTISCALE_, LAB_);
				tracker_ptr->init(positionBox, frame);
				tracker_itr->second = tracker_ptr;
			}
		}
	}
	void MultiTracker::DrawTrace(cv::Mat& frame)
	{
		std::map<size_t, ObjectInfo*>::iterator obj_itr = object_info_map_.begin();
		std::map<size_t, ObjectInfo*>::iterator obj_end_itr = object_info_map_.end();
		for (; obj_itr != obj_end_itr; ++obj_itr)
		{
			if (obj_itr->second->IsHuman() == true)
			{
				std::list<cv::Point>::iterator point_it = obj_itr->second->GetTrace().begin();
				std::list<cv::Point>::iterator point_end_it = obj_itr->second->GetTrace().end();
				for (; point_it != point_end_it; ++point_it)
				{
					cv::circle(frame, *point_it, 0, cv::Scalar(255, 128, 255), 1, 1);
				}
				cv::rectangle(frame, obj_itr->second->Box(), cv::Scalar(0, 0, 255), 2, 1, 0);
				cv::putText(frame, std::to_string(obj_itr->first).c_str(), cv::Point(obj_itr->second->Box().x + 10, obj_itr->second->Box().y + 15), 2, 0.8, cv::Scalar(255, 2, 25), 1);
				cv::putText(frame, std::to_string(obj_itr->second->Score()).c_str(), cv::Point(obj_itr->second->Box().x + 10, obj_itr->second->Box().y + 30), 2, 0.4, cv::Scalar(255, 2, 25), 1);
			}
		}
	}
	void MultiTracker::SetAllMatchFalse()
	{
		std::map<size_t, ObjectInfo*>::iterator obj_itr = object_info_map_.begin();
		std::map<size_t, ObjectInfo*>::iterator obj_end_itr = object_info_map_.end();
		for (; obj_itr != obj_end_itr; ++obj_itr)
		{
			obj_itr->second->SetMatchFalse();
		}
	}
	void MultiTracker::SetMatchTrue(size_t index)
	{
		std::map<size_t, ObjectInfo*>::iterator obj_itr = object_info_map_.find(index);
		obj_itr->second->SetMatchTrue();
	}
	void MultiTracker::SetBound(cv::Size size, cv::Rect bound)
	{
		bound_mask_ = cv::Mat(size, CV_8UC1);
		bound_mask_ = cv::Scalar::all(0);
		bound_mask_(bound) = cv::Scalar::all(255);
	}
	void MultiTracker::RemoveTrackerByIndex_(size_t index)
	{
		std::map<size_t, KCFTracker*>::iterator tracker_itr = traker_map_.find(index);
		std::map<size_t, ObjectInfo*>::iterator obj_itr = object_info_map_.find(index);
		if (tracker_itr != traker_map_.end() && obj_itr != object_info_map_.end())
		{
			traker_map_.erase(tracker_itr);
			object_info_map_.erase(obj_itr);
		}
		index_pool_set_.insert(index);
	}
	int MultiTracker::GetIndex_()
	{

		if (index_pool_set_.empty())
		{
			return ++key_max_;
		}
		else
		{
			int index = *(index_pool_set_.begin());
			index_pool_set_.erase(index_pool_set_.begin());
			return index;
		}
	}
}

/************************************************************************/
/* object information container                                         */
/************************************************************************/
#ifndef __OBJECT_INFO_H__
#define __OBJECT_INFO_H__

#include "util.h"
namespace ice
{
	class ObjectInfo
	{
	public:
		ObjectInfo(cv::Point point,cv::Rect& box,float score) :
			trace_(1, point), newest_box_(box), score_(score)
		{
			match_success_ = true;
			true_human_ = false;
			true_shit_ = false;
			life_ = 0;
			match_time_ = 0;
		}
		std::list<cv::Point>& GetTrace() { return trace_; }
		cv::Point& Position() { return trace_.back(); }
		cv::Rect& Box() { return newest_box_; }
		float& Score() { return score_; }
		void SetMatchFalse() { match_success_ = false; }
		void SetMatchTrue() { match_success_ = true; }
		bool IsShit() { return true_shit_; }
		bool IsHuman() { return true_human_; }
		void OnUpdate(cv::Mat& bound_mask)
		{
			++life_;
			cv::Point& center = Position();
			if(life_ == MIN_FRAME_LAST)
			{
				stop_iter_ = trace_.begin();
			}
			else if(life_ > MIN_FRAME_LAST)
			{
				stop_iter_++;
			}
			if (bound_mask.at<uchar>(center) == 0 || StopMoving())
			{
				true_shit_ = true;
			}
			else
			{
				match_time_ += (match_success_);
				
				if (!true_human_ && !true_shit_)
				{
					if (life_ == JUDGE_TIME)
					{
						if (match_time_ >= MIN_MATCH)
						{
							true_human_ = true;
						}
						else
							true_shit_ = true;
					}
				}
			}
			
		}
	private:
		bool StopMoving()
		{
			if (trace_.size() > MIN_FRAME_LAST)
				return Distance(*stop_iter_, trace_.back()) < MIN_MOVE_PIX;
			else
				return false;
		}
	private:
		bool match_success_;
		bool true_human_;
		bool true_shit_;
		int match_time_;
		std::list<cv::Point>::iterator stop_iter_;
		size_t life_;
		float score_;
		cv::Rect newest_box_;
		std::list<cv::Point> trace_;
	};
}

#endif // __OBJECT_INFO_H__

#include "region.h"
namespace ice
{
	Region::Region(cv::Point anchor_first, cv::Point anchor_second, cv::Point direction_point):
		anchor_first_(anchor_first), anchor_second_(anchor_second), direction_point_(direction_point), number_(0)
	{
		//according to anchors to calculate [Ax + By + C = 0] and the shadow of direction_point
		A_ = anchor_second.y - anchor_first.y;
		B_ = anchor_first.x - anchor_second.x;
		C_ = -anchor_first.x*A_ - anchor_first.y*B_;

		if (A_ == 0)
		{
			shadow_point_.x = direction_point.x;
			shadow_point_.y = -C_ / B_;
		}
		else if (B_ == 0)
		{
			shadow_point_.x = -C_ / A_;
			shadow_point_.y = direction_point.y;
		}
		else
		{
			shadow_point_.x = (B_ / (A_*A_ + B_*B_))*(B_*direction_point.x - A_*direction_point.y - A_*C_ / B_);
			shadow_point_.y = (C_ + A_*shadow_point_.x) / (-B_);
		}
		double k = REGION_MIN_MOVE_ONESTEP / sqrt(A_*A_ + B_*B_);
		double s_x = direction_point.x - shadow_point_.x;
		double s_y = direction_point.y - shadow_point_.y;
		if ((A_*s_x + B_*s_y) < 0)
		{
			A_ = -A_;
			B_ = -B_;
		}
		anchor_first_to_.x = anchor_first_.x + A_*k;
		anchor_first_to_.y = anchor_first_.y + B_*k;
		anchor_second_to_.x = anchor_second_.x + A_*k;
		anchor_second_to_.y = anchor_second_.y + B_*k;
	}

	bool Region::OnUpdate(cv::Point cur_position, cv::Point pre_position)
	{
		
		double s_x = cur_position.x - pre_position.x;
		double s_y = cur_position.y - pre_position.y;
		if ((A_*s_x + B_*s_y) > 0)
		{
			++number_;
			return true;
		}
		else return false;
	}
	
}

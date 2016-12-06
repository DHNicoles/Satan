/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __REGION_H__
#define __REGION_H__
#include "util.h"
namespace ice
{
	class Region
	{
	public:
		Region(cv::Point anchor_first,cv::Point anchor_second,cv::Point direction_point);
		bool OnUpdate(cv::Point cur_position, cv::Point pre_position);
	public:

		double A_, B_, C_;
		cv::Point anchor_first_, anchor_second_;
		cv::Point anchor_first_to_, anchor_second_to_;
		cv::Point direction_point_, shadow_point_;
		size_t number_;
	};
}


#endif//__REGION_H__

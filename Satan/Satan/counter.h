/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifndef __COUNTER_H__
#define __COUNTER_H__


#include "util.h"
namespace ice
{
	class Region;
	class ObjectInfo;
	class Counter
	{
	public:
		Counter();
		~Counter();
		void OnInit();
		void OnDestroy();
		void OnUpdate(std::map<size_t, ObjectInfo*>& object_info_map);
		std::map<uchar, Region*>& RegionMap() { return region_map_; }
		void MaskRegion(Region* region, uchar color);
		bool avaliable(cv::Mat& count_Mask, cv::Point& pt, int color);
		uchar GetColor();

	private:
		uchar color_;
		cv::Mat region_mask_;
		cv::Mat region_mask_roi_;
		std::map<uchar, Region*> region_map_;
	};
}
#endif//__COUNTER_H__

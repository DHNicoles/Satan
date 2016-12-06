#include "counter.h"
#include "region.h"
#include "object_info.h"
namespace ice
{
	Counter::Counter() :
		color_(20)
	{
		region_mask_ = cv::Mat(cv::Size(ORIGIN_WIDTH, ORIGIN_HIGHT), CV_8UC1);
		region_mask_ = cv::Scalar::all(0);
		region_mask_roi_ = region_mask_(cv::Rect(TOP_LEFT_X, TOP_LEFT_Y, WIDTH, HIGHT));
	}
	Counter::~Counter()
	{
		OnDestroy();
	}
	void Counter::OnInit()
	{
		//load data from database . after that, the point below is opposite to TOP_LEFT_X, TOP_LEFT_Y
		/**
		//int x1 = 300, y1 = 50, x2 = 550, y2 = 150, xp = 400, yp = 50;
		int x1 = 50, y1 = 500, x2 = 550, y2 = 550, xp = 400, yp = 580;
		*/
		cv::Point p11(300, 50), p12(550, 150), p13(400, 50);
		cv::Point p21(50, 500), p22(550, 550), p23(400, 580);
		{
			uchar color = GetColor();
			if (color == 0)
			{
				LOG(ERROR) << "fatal error : no enough color space to draw mask.";
			}
			Region* region = new Region(p11, p12, p13);
			MaskRegion(region, color);
			region_map_[color] = region;
		}
		{
			uchar color = GetColor();
			if (color == 0)
			{
				LOG(ERROR) << "fatal error : no enough color space to draw mask.";
			}
			Region* region = new Region(p21, p22, p23);
			MaskRegion(region, color);
			region_map_[color] = region;
		}
		cv::imwrite("/home/lt/cheguangfu/workspace/satan/Output/region_mask.jpg", region_mask_);
	}
	void Counter::OnDestroy()
	{
		std::map<uchar, Region*>::iterator rm_it = region_map_.begin();
		std::map<uchar, Region*>::iterator rm_end_it = region_map_.end();
		for (; rm_it != rm_end_it; ++rm_it)
		{
			if (rm_it->second)
				delete rm_it->second;
			rm_it->second = NULL;
		}
		region_map_.clear();
	}
	void Counter::OnUpdate(std::map<size_t, ObjectInfo*>& object_info_map)
	{
		std::map<size_t, ObjectInfo*>::iterator oim_it = object_info_map.begin();
		std::map<size_t, ObjectInfo*>::iterator oim_end_it = object_info_map.end();
		for (; oim_it != oim_end_it; ++oim_it)
		{
			if (!oim_it->second->Counted()&& oim_it->second->GetTrace().size()>2)
			{
				cv::Point cur_position = *(oim_it->second->GetTrace().rbegin());
				cv::Point pre_position = *(oim_it->second->GetTrace().rbegin()++);
				uchar postion_color = region_mask_roi_.at<uchar>(cur_position);
				LOG(INFO) << "cur_position:" << cur_position.x << ", " << cur_position.y;
				LOG(INFO) << "pre_position:" << pre_position.x << ", " << pre_position.y;
				LOG(INFO) << "postion_color:" << (int)postion_color;
				if (region_map_.count(postion_color) > 0)
				{
					oim_it->second->Counted() = region_map_[postion_color]->OnUpdate(cur_position, pre_position);
				}
			}
		}
	}
	void Counter::MaskRegion(Region* region, uchar color)
	{
		cv::line(region_mask_roi_, region->anchor_first_, region->anchor_first_to_, cv::Scalar(color, color, color), 1);
		cv::line(region_mask_roi_, region->anchor_second_, region->anchor_second_to_,cv::Scalar(color, color, color), 1);
		cv::line(region_mask_roi_, region->anchor_first_to_, region->anchor_second_to_, cv::Scalar(color, color, color), 1);
		int begin_x = (region->anchor_first_.x + region->anchor_second_to_.x) / 2;
		int begin_y = (region->anchor_first_.y + region->anchor_second_to_.y) / 2;
		//假设此点位于ROI内部
		std::deque<cv::Point> dq;	
		dq.push_back(cv::Point(begin_x, begin_y));
		region_mask_roi_.at<uchar>(begin_x, begin_y) = color;

		while (!dq.empty())
		{
			cv::Point pt = dq.front();
			dq.pop_front();
			//上下左右
			cv::Point u(pt.x, pt.y - 1), d(pt.x, pt.y + 1), l(pt.x - 1, pt.y), r(pt.x + 1, pt.y);
			if (avaliable(region_mask_roi_, u, color))
			{
				region_mask_roi_.at<uchar>(u) = color;
				dq.push_back(u);
			}
			if (avaliable(region_mask_roi_, d, color))
			{
				region_mask_roi_.at<uchar>(d) = color;
				dq.push_back(d);
			}
			if (avaliable(region_mask_roi_, l, color))
			{
				region_mask_roi_.at<uchar>(l) = color;
				dq.push_back(l);
			}
			if (avaliable(region_mask_roi_, r, color))
			{
				region_mask_roi_.at<uchar>(r) = color;
				dq.push_back(r);
			}
		}
	}
	bool Counter::avaliable(cv::Mat& count_Mask, cv::Point& pt, int color)
	{
		if (pt.x < 0 || pt.x == count_Mask.cols || pt.y < 0 || pt.y == count_Mask.rows || count_Mask.at<uchar>(pt.y, pt.x) == color) return false;
		return true;
	}
	uchar Counter::GetColor()
	{
		if (color_ > 255) return 0;
		uchar temp = color_;
		color_ += 20;
		return temp;
	}
}

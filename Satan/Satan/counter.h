#ifndef __COUNTER_H__
#define __COUNTER_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
#include "util.h"
namespace ice
{
	class Counter
	{
	public:
		Counter() {}
		~Counter() {}
		static cv::Mat& RegionMask() { return region_mask_; }
		static void Add() { ++add_; }
		static void Minus() { ++minus_; }
		static const size_t AddNumber() { return add_; }
		static const size_t MinusNumber() { return minus_; }

		static void CreateRegionMask(cv::Size size)
		{
			region_mask_ = cv::Mat(size, CV_8UC1);
			region_mask_ = cv::Scalar::all(0);

			cv::Point start, end;
			int begin_x = 0, begin_y = 0;
			switch (COUNT_STRATEGY)
			{
			case 1:
				start = cv::Point((size.width >> 1), 0);
				end = cv::Point((size.width >> 1), size.height - 1);
				break;
			case 2:
				start = cv::Point(0, 0);
				end = cv::Point(size.width - 1, size.height - 1);
				begin_x = size.width - 1;
				break;
			case 3:
				start = cv::Point(0, (size.height >> 1));
				end = cv::Point(size.width - 1, (size.height >> 1));
				break;
			case 4:
				start = cv::Point(0, size.height - 1);
				end = cv::Point(size.width - 1, 0);
				break;
			default:
				break;
			}
			cv::line(region_mask_, start, end, cv::Scalar(255, 255, 255), 2);

			//假设此点位于ROI内部
			std::deque<cv::Point2i> dq;// = { Point(0, Scale_Height - 1), Point(Scale_Width - 1, 0) };

			dq.push_back(cv::Point(begin_x, begin_y));
			region_mask_.at<uchar>(begin_x, begin_y) = 255;
			while (!dq.empty())
			{
				cv::Point pt = dq.front();
				region_mask_.at<uchar>(pt) = 255;
				dq.pop_front();
				//上下左右
				cv::Point u(pt.x, pt.y - 1), d(pt.x, pt.y + 1), l(pt.x - 1, pt.y), r(pt.x + 1, pt.y);
				if (!notavaliable(region_mask_, u, 255)) dq.push_back(u);
				if (!notavaliable(region_mask_, d, 255)) dq.push_back(d);
				if (!notavaliable(region_mask_, l, 255)) dq.push_back(l);
				if (!notavaliable(region_mask_, r, 255)) dq.push_back(r);
			}
			
		}
		static bool notavaliable(cv::Mat& count_Mask, cv::Point& pt, int color) {
			if (pt.x < 0 || pt.x == count_Mask.cols || pt.y < 0 || pt.y == count_Mask.rows || count_Mask.at<uchar>(pt.y, pt.x) == 255) return true;
			return false;
		}

	private:
		static cv::Mat region_mask_;
		static size_t add_;
		static size_t minus_;
	};
	cv::Mat Counter::region_mask_;
	size_t  Counter::add_;
	size_t  Counter::minus_;
}
#endif//__COUNTER_H__

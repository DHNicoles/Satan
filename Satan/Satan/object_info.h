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
		ObjectInfo() {}
		ObjectInfo(cv::Point& point) :
			trace_(1, point)
		{
		}
		std::list<cv::Point>& GetTrace() { return trace_; }
	private:
		std::list<cv::Point> trace_;
	};
}

#endif // __OBJECT_INFO_H__
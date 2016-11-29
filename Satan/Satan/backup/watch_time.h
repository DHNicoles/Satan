#ifndef __WATCH_TIME_H__
#define __WATCH_TIME_H__

#include <sys/time.h>

namespace ice
{
	class WatchTime
	{
	public:
		WatchTime() :
			cost_time_(0.0)
		{}
		~WatchTime() {}
		void Start() { gettimeofday(&start_time_, 0); }
		void Reset() { cost_time_ = 0.0; }
		void Stop() { gettimeofday(&end_time_, 0); }
		double GetTime()
		{
			cost_time_ = 1000000 * (end_time_.tv_sec - start_time_.tv_sec) + end_time_.tv_usec - start_time_.tv_usec;
			cost_time_ /= 1000000;
			return cost_time_;
		}
	private:
		timeval start_time_, end_time_;
		double	cost_time_;
	};
}
#endif//__WATCH_TIME_H__


#ifndef detect_behavior_h__
#define detect_behavior_h__

#include "util.h"


//class video4;
//class VideoFrameGetter;
namespace ice
{
	class ObjectInfo;
	class Detect_behavior {
	public:
		Detect_behavior();
		~Detect_behavior();
		virtual void process(std::map<size_t, ObjectInfo*>& object_info_map, std::string& filename, size_t offset);
	private:
		cv::Point getcenter(std::vector<cv::Point> points);
		std::vector<int> getgather_spread_nums();
		bool isSameObject(ObjectInfo& object, ObjectInfo& other);
		void getPoints(std::map<size_t, ObjectInfo*>& objects);
	
	private:
		//数据库对象
		//video4 * video_mysql; 
		//帧数
		int frameNum;
		//上一帧的坐标点
		std::vector<cv::Point> pro_points;
		//当前帧的坐标点
		std::vector<cv::Point> cur_points;
		//中心点坐标
		cv::Point center_point;
		//聚集次数
		int aggregationNum;
		//true为聚集, false为不在聚集状态
		bool isAggregation;
		//扩散次数
		int diffusionNum;
		//true为扩散, false为不在扩散状态
		bool isDisffusion;
	};
}
#endif // detect_behavior_h__


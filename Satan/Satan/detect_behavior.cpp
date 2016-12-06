#include "detect_behavior.h"
#include "object_info.h"

namespace ice
{
	Detect_behavior::Detect_behavior():
		frameNum(0), isAggregation(false), isDisffusion(false)
	{
		//video_mysql = video4::creat();
	}
	
	Detect_behavior::~Detect_behavior(){
		std::vector<cv::Point>().swap(pro_points);
		std::vector<cv::Point>().swap(cur_points);
	}
	
	//获取人群状态
	void Detect_behavior::process(std::map<size_t, ObjectInfo*>& object_info_map, std::string& filename, size_t offset) 
	{
		if(object_info_map.size() == 0)
		    return;
		//记录当前帧数
		frameNum++;
		frameNum %= 20;
		//记录帧对象个数
		int objectsNum = 0;
		std::map<size_t, ObjectInfo*> firstObjects;
		//若为第一帧
		if(frameNum == 1)
		{
			//std::cout << "------first frame------" << std::endl;
			//记录第一帧所有对象
			firstObjects = object_info_map;
			//第一帧对象个数
			objectsNum = firstObjects.size();
			//初始化聚集and扩散状态
			isAggregation = false;
			isDisffusion = false;
			//当对象个数超过10人才进行状态判断
			if(objectsNum >= 10){
				//记录当前帧的所有对象的坐标点
				std::vector<cv::Point>().swap(cur_points);
				getPoints(firstObjects);
				center_point = getcenter(cur_points);
				//初始化聚集与扩散次数
				aggregationNum = 0;
				diffusionNum = 0;
			}
			else{	//否则帧数重新归0
				frameNum = 0;
			}
			return;
		}
		//std::cout << "------" << frameNum << "frame------" << std::endl;
		//记录当前帧所有对象
		std::map<size_t, ObjectInfo*> curFrameObjects = object_info_map;
		//上一帧对象位置
		std::vector<cv::Point>().swap(pro_points);
		pro_points = cur_points;
		//回收内存
		std::vector<cv::Point>().swap(cur_points);
		//判断是否为同一对象,并更新当前帧的对象
		std::map<size_t, ObjectInfo*>::iterator pro_object;
		std::vector<cv::Point>::iterator pro_Pot = pro_points.begin(); 
		for(pro_object = firstObjects.begin(); pro_object != firstObjects.end();)
		{
			std::map<size_t, ObjectInfo*>::iterator cur_object;
			for (cur_object = curFrameObjects.begin(); cur_object != curFrameObjects.end();++cur_object)
			{
				if(isSameObject(*pro_object->second, *cur_object->second))
				{
					*pro_object->second = *cur_object->second;
					break;
				}
			}
			if(cur_object == curFrameObjects.end()){
				pro_object = firstObjects.erase(pro_object);
				pro_Pot = pro_points.erase(pro_Pot);
			}else{
				++pro_object;
				++pro_Pot;
			}
		}
		//记录当前帧对象的个数
		objectsNum = firstObjects.size();
		//当对象个数超过10人才进行状态判断
		if(objectsNum >= 10)
		{
			//记录当前帧的所有对象的坐标点
			getPoints(firstObjects);
			//统计扩散or聚集对象的个数
			std::vector<int> nums = getgather_spread_nums();
			if(nums[0] >= 4 * nums[1])
				aggregationNum++;
			else if(nums[1] >= 4 * nums[0])
				diffusionNum++;
			//如果是1s的结束，这里以20帧为结束，判断聚集or扩散
			if(frameNum == 0)
			{
				std::string video_name = filename;
				int current_frame_time = offset;

				char realtime[20];
				sprintf(realtime, "%d", current_frame_time);
				std::string str_time(realtime);
				
				std::string base_time = video_name.substr(9, 19);
				//判断聚集or扩散
				if(aggregationNum > 3 * diffusionNum)
				{
					isAggregation = true;
					isDisffusion = false;
					LOG(INFO) << "------聚集中------" << std::endl;
					std::string str_count("gather");

					//video_mysql->Query("0",base_time.c_str(),str_time.c_str(),"2","(0,0, 1, 1)",str_count.c_str());
				}
				else if(diffusionNum > 3 * aggregationNum)
				{
					isAggregation = false;
					isDisffusion = true;
					LOG(INFO) << "------扩散中------" << std::endl;
					std::string str_count("spread");

					//video_mysql->Query("0",base_time.c_str(),str_time.c_str(),"2","(0,0, 1, 1)",str_count.c_str());
				}
				else{
					isAggregation = false;
					isDisffusion = false;
					LOG(INFO) << "------正常中------" << std::endl;
				}
			}
		}else{	//否则帧数重新归0
			frameNum = 0;
		}
	}
	
	//获取当前帧所有对象的坐标点
	void Detect_behavior::getPoints(std::map<size_t, ObjectInfo*>& objects){
		
		std::map<size_t, ObjectInfo*>::iterator it = objects.begin();
		std::map<size_t, ObjectInfo*>::iterator end_it = objects.end();
		for (; it != end_it; ++it)
		{
			cv::Point curPoint;// = {0, 0};
			curPoint.x = curPoint.y = 0;
			curPoint = it->second->GetTrace().back();
			cur_points.push_back(curPoint);
		}
	}
	
	
	//判断是否为同一对象
	bool Detect_behavior::isSameObject(ObjectInfo& object, ObjectInfo& other){
		return object.Box() == other.Box();
	}
	
	
	//求特征点中心
	cv::Point Detect_behavior::getcenter(std::vector<cv::Point> points){
		cv::Point center;
		if(points.size () > 0)
		{
			float center_x = points[0].x;
			float center_y = points[0].y;
			for(int i = 1; i< points.size (); ++i)
			{
				center_x += points[i].x;
				//center_x /= 2;
				center_y += points[i].y;
				//center_y /= 2;
			}
			center.x = center_x / points.size ();
			center.y = center_y / points.size ();
		}
		else
		{
			center.x = 0;
			center.y = 0;
		}
		return center;
	}
	
	//获取聚集及扩散特征点个数
	std::vector<int> Detect_behavior::getgather_spread_nums()
	{
		//0表示聚集，1表示扩散
		std::vector<int> nums;
		float pro_distance;
		float cur_distance;
		float center_x = center_point.x;
		float center_y = center_point.y;
		nums.push_back(0);
		nums.push_back(0);
		for(int i = 0; i < cur_points.size(); i++)
		{
			pro_distance = (pro_points[i].x - center_x) * (pro_points[i].x - center_x) + (pro_points[i].y - center_y) * (pro_points[i].y - center_y);
			cur_distance = (cur_points[i].x - center_x) * (cur_points[i].x - center_x) + (cur_points[i].y - center_y) * (cur_points[i].y - center_y);
			if(pro_distance > cur_distance)
				nums[0]++;
			else if(pro_distance < cur_distance)
				nums[1]++;
		}
		return nums;
	}
}

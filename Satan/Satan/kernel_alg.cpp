#include "kernel_alg.h"
#include "ssd_detector.h"
#include "object_info.h"
#include "counter.h"
#include "region.h"
#include "detect_behavior.h"
#include <pthread.h>
namespace ice
{
	KernelAlg::KernelAlg() :
		detector_ptr_(NULL)
	{
	}

	KernelAlg::~KernelAlg()
	{
		OnDestroy();
	}

	void KernelAlg::OnInit(
		const std::string& model_file,
		const std::string& weights_file,
		const std::string& mean_file,
		const std::string& mean_value)
	{
		detector_ptr_ = new Detector;
		detector_ptr_->OnInit(model_file, weights_file, mean_file, mean_value);
		region_counter_ = new Counter;
		region_counter_->OnInit();

		behavior_ptr_ = new Detect_behavior;
	}

	void KernelAlg::OnDestroy()
	{
		if (detector_ptr_)
		{
			delete detector_ptr_;
		}
		detector_ptr_ = NULL;
		if (region_counter_)
		{
			delete region_counter_;
		}
		region_counter_ = NULL;

		if (behavior_ptr_)
		{
			delete behavior_ptr_;
		}
		behavior_ptr_ = NULL;
	}

	void KernelAlg::OnUpdate(cv::Mat& frame, std::string& filename, size_t offset)
	{
		static unsigned int frame_count = 0;
		static char buffer[100];
		static WatchTime T;
		//trackers to update
		T.Reset();	T.Start();

		multi_trackers_.OnUpdate(frame);
		multi_trackers_.RemoveInvalid();

		region_counter_->OnUpdate(multi_trackers_.TrackingObectorMap());
		T.Stop();
		LOG(INFO) << "update trackers\t->\t" << T.GetTime();
		T.Reset();	T.Start();
		behavior_ptr_->process(multi_trackers_.TrackingObectorMap(), filename, offset);
		//ThreadParam thread_param;
		//thread_param.kerner_alg_ptr = this;
		//thread_param.filename = filename;
		//thread_param.offset = offset;



		T.Stop();
		LOG(INFO) << "update behavior\t->\t" << T.GetTime();


		//detector
		//detect head and draw
		std::vector<std::vector<float> > detected_boxes;
		std::vector<float> scores;
		detector_ptr_->Detect(frame, detected_boxes, scores);
		//put into obj_boxes
		std::vector<cv::Rect> obj_boxes(detected_boxes.size());
		for (int i = 0; i < detected_boxes.size(); i++)
		{
			std::vector<float> box = detected_boxes.at(i);
			int  width = (int)(box.at(2) - box.at(0));
			int  height = (int)(box.at(3) - box.at(1));
			obj_boxes[i] = cv::Rect((int)box.at(0), (int)box.at(1), width, height);

		}
		T.Reset();	T.Start();

		Match(obj_boxes, scores, frame);

		T.Stop();
		LOG(INFO) << "match boxes\t->\t" << T.GetTime();
		if (DRAW_DETECTOR)
		{
			for (size_t i = 0; i < obj_boxes.size(); ++i)
			{
				cv::rectangle(frame, obj_boxes[i], cv::Scalar(255, 255, 255), 2, 1, 0);
			}
		}
		T.Reset();	T.Start();
		if (DRAW_TRACKER)
		{
			multi_trackers_.DrawTrace(frame);
			cv::rectangle(frame, cv::Rect((WIDTH - BOUND_WIDTH) / 2, (WIDTH - BOUND_WIDTH) / 2, BOUND_WIDTH, BOUND_HIGHT), cv::Scalar(255, 255, 0), 2, 1, 0);
		}
		if (DRAW_COUNTER)
		{
			std::map<uchar, Region*>::iterator rm_it = region_counter_->RegionMap().begin();
			std::map<uchar, Region*>::iterator rm_end_it = region_counter_->RegionMap().end();
			for (; rm_it != rm_end_it; ++rm_it)
			{
				cv::putText(frame, std::to_string(rm_it->second->number_).c_str(), rm_it->second->direction_point_, 2, 0.8, cv::Scalar(255, 255, 255), 1);
			}

		}
		T.Stop();
		LOG(INFO) << "draw trace\t->\t" << T.GetTime();

		std::string name = std::to_string(frame_count++);
		std::string pre(7 - name.size(), '0');
		name = std::string("/home/lt/cheguangfu/workspace/satan/Output/") + pre + name + ".jpg";
		//sprintf(buffer, "/home/lt/cheguangfu/workspace/satan/Output/detect_%d.jpg", frame_count++);
		imwrite(name.c_str(), frame);
	}
	void KernelAlg::SetBound(cv::Size size, cv::Rect r)
	{
		multi_trackers_.SetBound(size, r);
	}

	void * KernelAlg::thread(void * ptr)
	{
		return nullptr;
	}

	void KernelAlg::Match(std::vector<cv::Rect>& obj_boxes, std::vector<float>& scores, cv::Mat& frame)
	{
		multi_trackers_.SetAllMatchFalse();

		std::map<size_t, ObjectInfo*>& tracking_obector_map = multi_trackers_.TrackingObectorMap();

		for (size_t i = 0; i < obj_boxes.size(); ++i)
		{
			std::map<size_t, ObjectInfo*>::iterator tom_it = tracking_obector_map.begin();
			std::map<size_t, ObjectInfo*>::iterator tom_end_it = tracking_obector_map.end();
			std::map<size_t, ObjectInfo*>::iterator min_it = tom_end_it;
			int min_dist = 1000000;
			for (; tom_it != tom_end_it; ++tom_it)
			{
				int dist = Distance(obj_boxes[i], tom_it->second->Position());
				if (dist < min_dist && dist < DIST_HUMAN)
				{
					min_dist = dist;
					min_it = tom_it;
				}
			}
			if (min_it == tom_end_it)
			{
				LOG(INFO) << "add trcker.";
				multi_trackers_.AddTracker(frame, obj_boxes[i], scores[i]);
			}
			else
			{
				multi_trackers_.SetMatchTrue(min_it->first);
				if (min_it->second->Score() < scores[i])
				{
					min_it->second->Score() = scores[i];
					LOG(INFO) << "replace tracker.index=" << min_it->first;
					multi_trackers_.Replace(frame, obj_boxes[i], min_it->first);
				}
			}
		}
	}
}

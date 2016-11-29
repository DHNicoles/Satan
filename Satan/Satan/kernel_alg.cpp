#include "kernel_alg.h"
#include "ssd_detector.h"
#include "object_info.h"
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
	}

	void KernelAlg::OnDestroy()
	{
		if (detector_ptr_)
		{
			delete detector_ptr_;
		}
		detector_ptr_ = NULL;
	}

	void KernelAlg::DetectAndTrack(cv::Mat& frame, bool update_trackers, bool update_detector)
	{
		static unsigned int frame_count = 0;
		static char buffer[100];
		static WatchTime T;
		//trackers to update
		T.Reset();	T.Start();
		if (update_trackers)
		{
			multi_trackers_.OnUpdate(frame);
			multi_trackers_.RemoveInvalid();
		}
		T.Stop();
		LOG(INFO) << "update trackers\t->\t" << T.GetTime();

		if (update_detector)
		{
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
			if (update_trackers)
			{
				Match(obj_boxes, scores, frame);
			}
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
				cv::rectangle(frame, cv::Rect(60, 60, 600, 600), cv::Scalar(255, 255, 0), 1, 1, 0);
			}
			T.Stop();
			LOG(INFO) << "draw trace\t->\t" << T.GetTime();
		}
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
	void KernelAlg::SetCountStrategy(cv::Size size)
	{
		Counter::CreateRegionMask(size)
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

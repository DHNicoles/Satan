/************************************************************************/
/* ssd detector                                                                     */
/************************************************************************/

#ifndef __SSD_DETECTOR_H__
#define __SSD_DETECTOR_H__

#include "util.h"
namespace ice
{
	using namespace caffe;
	class Detector
	{
	public:
		Detector();
		~Detector();
		void OnInit(const std::string& model_file, const std::string& weights_file, const std::string& mean_file, const std::string& mean_value);
		void OnDestroy();
		void Detect(cv::Mat& frame, std::vector<std::vector<float> >& boxes, std::vector<float>& scores);
	private:
		void SetMean(const std::string& mean_file, const std::string&mean_value);
		void WrapInputLayer(std::vector<cv::Mat>* input_channels);
		void Preprocess(const cv::Mat&img, std::vector<cv::Mat>* input_channels);

	private:
		shared_ptr<caffe::Net<float> > net_;
		cv::Size input_geometry_;
		int num_channels_;
		cv::Mat mean_;

	};
}
#endif // __SSD_DETECTOR_H__

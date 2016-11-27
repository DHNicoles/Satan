#include "ssd_detector.h"

namespace ice
{
	Detector::Detector()
	{
	}

	Detector::~Detector()
	{
	}

	void Detector::OnInit(
		const std::string & model_file,
		const std::string & weights_file,
		const std::string & mean_file,
		const std::string& mean_value)
	{
		std::vector<int> gpus;
		//get_gpus(&gpus);
		Caffe::set_mode(Caffe::GPU);
		//Caffe::SetDevice(gpus[2]);;
		net_.reset(new Net<float>(model_file, TEST));
		net_->CopyTrainedLayersFrom(weights_file);
		Blob<float>* input_layer = net_->input_blobs()[0];
		num_channels_ = input_layer->channels();
		input_geometry_ = cv::Size(input_layer->width(), input_layer->height());
		SetMean(mean_file, mean_value);
	}

	void Detector::OnDestroy()
	{
	}

	void Detector::Detect(cv::Mat & frame, std::vector<std::vector<float> >& boxes)
	{
		Blob<float>* input_layer = net_->input_blobs()[0];

		input_layer->Reshape(1, num_channels_,
			input_geometry_.height, input_geometry_.width);

		/* Forward dimension change to all layers. */
		net_->Reshape();

		std::vector<cv::Mat> input_channels;
		WrapInputLayer(&input_channels);

		Preprocess(frame, &input_channels);

		net_->Forward();

		/* Copy the output layer to a std::vector */
		Blob<float>* result_blob = net_->output_blobs()[0];
		const float* result = result_blob->cpu_data();
		const int num_det = result_blob->height();
		std::vector<std::vector<float> > detections;
		for (int k = 0; k < num_det; ++k) {
			if (result[0] == -1) {
				// Skip invalid detection.
				result += 7;
				continue;
			}
			std::vector<float> detection(result, result + 7);
			detections.push_back(detection);
			result += 7;
		}
		for (int i = 0; i < detections.size(); i++)
		{
			std::vector<float> d = detections[i];
			float score = d[2];
			int label = static_cast<int>(d[1]);
			if (score >= 0.3&&label == 1)
				//if(score>=0.3 && ( label== 6 ||label ==7  ||label ==15  ))
			{
				std::vector<float> box;
				box.push_back(d[3] * frame.cols);
				box.push_back(d[4] * frame.rows);
				box.push_back(d[5] * frame.cols);
				box.push_back(d[6] * frame.rows);
				boxes.push_back(box);
			}
		}
	}

	void Detector::SetMean(const std::string & mean_file, const std::string & mean_value)
	{
		cv::Scalar channel_mean;
		if (!mean_file.empty()) {
			BlobProto blob_proto;
			ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

			/* Convert from BlobProto to Blob<float> */
			Blob<float> mean_blob;
			mean_blob.FromProto(blob_proto);

			/* The format of the mean file is planar 32-bit float BGR or grayscale. */
			std::vector<cv::Mat> channels;
			float* data = mean_blob.mutable_cpu_data();
			for (int i = 0; i < num_channels_; ++i) {
				/* Extract an individual channel. */
				cv::Mat channel(mean_blob.height(), mean_blob.width(), CV_32FC1, data);
				channels.push_back(channel);
				data += mean_blob.height() * mean_blob.width();
			}

			/* Merge the separate channels into a single image. */
			cv::Mat mean;
			cv::merge(channels, mean);

			/* Compute the global mean pixel value and create a mean image
			* filled with this value. */
			channel_mean = cv::mean(mean);
			mean_ = cv::Mat(input_geometry_, mean.type(), channel_mean);
		}
		if (!mean_value.empty()) {
			stringstream ss(mean_value);
			vector<float> values;
			string item;
			while (getline(ss, item, ',')) {
				float value = std::atof(item.c_str());
				values.push_back(value);
			}

			std::vector<cv::Mat> channels;
			for (int i = 0; i < num_channels_; ++i) {
				/* Extract an individual channel. */
				cv::Mat channel(input_geometry_.height, input_geometry_.width, CV_32FC1,
					cv::Scalar(values[i]));
				channels.push_back(channel);
			}
			cv::merge(channels, mean_);
		}
	}

	void Detector::WrapInputLayer(std::vector<cv::Mat>* input_channels)
	{
		Blob<float>* input_layer = net_->input_blobs()[0];

		int width = input_layer->width();
		int height = input_layer->height();
		float* input_data = input_layer->mutable_cpu_data();
		for (int i = 0; i < input_layer->channels(); ++i) {
			cv::Mat channel(height, width, CV_32FC1, input_data);
			input_channels->push_back(channel);
			input_data += width * height;
		}
	}

	void Detector::Preprocess(const cv::Mat & img, std::vector<cv::Mat>* input_channels)
	{
		/* Convert the input image to the input image format of the network. */
		cv::Mat sample;
		if (img.channels() == 3 && num_channels_ == 1)
			cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
		else if (img.channels() == 4 && num_channels_ == 1)
			cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
		else if (img.channels() == 4 && num_channels_ == 3)
			cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
		else if (img.channels() == 1 && num_channels_ == 3)
			cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
		else
			sample = img;

		cv::Mat sample_resized;
		if (sample.size() != input_geometry_)
			cv::resize(sample, sample_resized, input_geometry_);
		else
			sample_resized = sample;

		cv::Mat sample_float;
		if (num_channels_ == 3)
			sample_resized.convertTo(sample_float, CV_32FC3);
		else
			sample_resized.convertTo(sample_float, CV_32FC1);

		cv::Mat sample_normalized;
		cv::subtract(sample_float, mean_, sample_normalized);

		/* This operation will write the separate BGR planes directly to the
		* input layer of the network because it is wrapped by the cv::Mat
		* objects in input_channels. */
		cv::split(sample_normalized, *input_channels);
	}
}

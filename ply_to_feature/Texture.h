#pragma once
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

namespace AISTER_GRAPHICS_ENGINE {
	class Texture {
	public:
		int width;
		int height;
		int type;
		uchar* data;

		Texture(std::string filePath) {
			cv::Mat image = cv::imread(filePath);

			width = image.cols;
			height = image.rows;

			data = image.data;
			type = image.type();
		}
	};
}
// StbWrapper.cpp: 定义应用程序的入口点。
//
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize2.h"
#include "stb_wrapper.h"
#include <vector>
#include <unordered_map>
#include <math.h>   
#include <corecrt_math_defines.h>

namespace xiaochufuji
{
	static const std::unordered_map<std::string, ImageFormat> ImageFormatMap = {
		{"jpg", ImageFormat::JPG}, {"png", ImageFormat::PNG}, {"bmp", ImageFormat::BMP}, {"hdr", ImageFormat::HDR}, {"tga", ImageFormat::TGA}
	};
	static const ResizeOption ResizeOptionNone;
	struct Image
	{
		explicit Image() {};
		explicit Image(const std::string& imgPath, ColorFormat format) {
			void* imgPtr{ nullptr };
			imgPtr = stbi_load(imgPath.c_str(), &x, &y, &channels, static_cast<int>(format));
			if (!imgPtr) return;
			img.reset(imgPtr);
		}
		explicit Image(unsigned char* memoryBuffer, int bufferLen, ColorFormat format) {
			void* imgPtr{ nullptr };
			imgPtr = stbi_load_from_memory(memoryBuffer, bufferLen, &x, &y, &channels, static_cast<int>(format));
			if (!imgPtr) return;
			img.reset(imgPtr);
		};
		~Image() {
		}
		void* data() const {
			return img.get();
		}
		struct AutoFreeImg
		{
			void operator()(void* ptr) const {
				if (ptr) {
					stbi_image_free(ptr);
					ptr = nullptr;
				}
			}
		};
		std::unique_ptr<void, AutoFreeImg> img{ nullptr };
		int x = 0;
		int y = 0;
		int channels = 0;
	};
	bool operator==(const ResizeOption& lval, const ResizeOption& rval)
	{
		return lval.pixelLayout == rval.pixelLayout && lval.edge == rval.edge && lval.filter == rval.filter && lval.dataType == rval.dataType;
	}
	StbWrapper::StbWrapper(const std::string& path, ColorFormat format)
	{
		reload(path, format);
	}
	StbWrapper::StbWrapper(StbWrapper&& old) noexcept
	{
		if (old.m_img.get())
		{
			this->m_img = std::move(old.m_img);
			this->outputWidth = old.outputWidth;
			this->outputHeight = old.outputHeight;
			this->outputChannels = old.outputChannels;
		}
	}
	StbWrapper& StbWrapper::operator=(StbWrapper&& old) noexcept
	{
		if (old.m_img.get())
		{
			this->m_img = std::move(old.m_img);
			this->outputWidth = old.outputWidth;
			this->outputHeight = old.outputHeight;
			this->outputChannels = old.outputChannels;
		}
		return *this;
	}
	StbWrapper::~StbWrapper()
	{
	}
	unsigned char* StbWrapper::data()
	{
		return static_cast<unsigned char*>(m_img->data());
	}
	void StbWrapper::reload(const std::string& path, ColorFormat format)
	{
		m_img.reset(new Image(path, format));
		if (!m_img.get()) return;
		if (format == ColorFormat::ORIGIN) outputChannels = m_img->channels;
		outputChannels = static_cast<int>(format);
		copyProperties();
	}
	void StbWrapper::reload(unsigned char* memoryBuffer, int bufferLen, ColorFormat format)
	{
		m_img.reset(new Image(memoryBuffer, bufferLen, format));
		if (!m_img.get()) return;
		if (format == ColorFormat::ORIGIN) outputChannels = m_img->channels;
		outputChannels = static_cast<int>(format);
		copyProperties();
	}
	int StbWrapper::resize(int newWidth, int newHeight, const ResizeOption& option)
	{
		outputWidth = newWidth;
		outputHeight = newHeight;
		if (option == ResizeOptionNone) return resizeSimple(newWidth, newHeight);
		return resizeOption(newWidth, newHeight, option);
	}
	static 	int is_90_degree_multiple(float angle) {
		float remainder = fmodf(fabsf(angle), 90.0f);
		return remainder < 1e-5; // 允许一定的浮点误差
	}
	int StbWrapper::rotate(float angleDegree)
	{
		if (!is_90_degree_multiple(angleDegree)) return rotateN90_nearest(angleDegree);
		else return rotate90(angleDegree);
	}
	int StbWrapper::save(const std::string& outputPath, ImageFormat format, SaveOption option)
	{
		int retFlag = 0;
		// if channels change, such as: load--GRAY but save--RGB
		switch (format)
		{
		case ImageFormat::JPG:
		{
			retFlag = handleSaveJpg(outputPath.c_str(), option);
			break;
		}
		case ImageFormat::PNG:
		{
			retFlag = handleSavePng(outputPath.c_str(), option);
			break;
		}
		case ImageFormat::BMP:
		{
			retFlag = handleSaveBmp(outputPath.c_str(), option);
			break;
		}
		case ImageFormat::HDR:
		{
			retFlag = handleSaveHdr(outputPath.c_str(), option);
			break;
		}
		case ImageFormat::TGA:
		{
			retFlag = handleSaveTga(outputPath.c_str(), option);
			break;
		}
		default:
			break;
		}
		return retFlag;
	}
	int StbWrapper::autoSave(const std::string& outputPath, SaveOption option)
	{
		auto suffixPos = outputPath.rfind('.');
		if (suffixPos == std::string::npos) return 0;
		std::string suffix = outputPath.substr(suffixPos + 1);
		auto saveFormatIte = ImageFormatMap.find(suffix);
		if (saveFormatIte == ImageFormatMap.end()) return 0;
		return save(outputPath, saveFormatIte->second, option);
	}
	void StbWrapper::copyProperties()
	{
		outputWidth = m_img->x;
		outputHeight = m_img->y;
	}
	int StbWrapper::handleSaveJpg(const char* filename, SaveOption option)
	{
		if (!m_img->data() || option.quality < 1 || option.quality > 100) return 0;
		return stbi_write_jpg(filename, outputWidth, outputHeight, outputChannels, static_cast<const void*>(m_img->data()), option.quality);
	}
	int StbWrapper::handleSavePng(const char* filename, SaveOption option)
	{
		if (!m_img->data()) return 0;
		int stride_in_bytes = outputWidth * outputChannels;
		return stbi_write_png(filename, outputWidth, outputHeight, outputChannels, static_cast<const void*>(m_img->data()), stride_in_bytes);
	}
	int StbWrapper::handleSaveBmp(const char* filename, SaveOption option)
	{
		if (!m_img->data()) return 0;
		return stbi_write_bmp(filename, outputWidth, outputHeight, outputChannels, static_cast<const void*>(m_img->data()));
	}
	int StbWrapper::handleSaveHdr(const char* filename, SaveOption option)
	{
		if (!m_img->data()) return 0;
		auto data = static_cast<unsigned char*>(m_img->data());
		std::vector<float> hdr_data(outputWidth * outputHeight * outputChannels);
		if (outputChannels == 1) {
			for (int i = 0; i < outputWidth * outputHeight; ++i) {
				int index = i * outputChannels;
				hdr_data[index] = static_cast<float>(data[index]) / 255.0f;
			}
		}
		else if (outputChannels == 3) {
			for (int i = 0; i < outputWidth * outputHeight; ++i) {
				int index = i * outputChannels;
				hdr_data[index + 0] = static_cast<float>(data[index + 0]) / 255.0f;
				hdr_data[index + 1] = static_cast<float>(data[index + 1]) / 255.0f;
				hdr_data[index + 2] = static_cast<float>(data[index + 2]) / 255.0f;
			}
		}
		else if (outputChannels == 4) {
			for (int i = 0; i < outputWidth * outputHeight; ++i) {
				int index = i * outputChannels;
				hdr_data[index + 0] = static_cast<float>(data[index + 0]) / 255.0f;
				hdr_data[index + 1] = static_cast<float>(data[index + 1]) / 255.0f;
				hdr_data[index + 2] = static_cast<float>(data[index + 2]) / 255.0f;
				hdr_data[index + 3] = static_cast<float>(data[index + 3]) / 255.0f;
			}
		}
		return stbi_write_hdr(filename, outputWidth, outputHeight, outputChannels, hdr_data.data());
	}
	int StbWrapper::handleSaveTga(const char* filename, SaveOption option)
	{
		if (!m_img->data()) return 0;
		return stbi_write_tga(filename, outputWidth, outputHeight, outputChannels, static_cast<const void*>(m_img->data()));
	}
	int StbWrapper::resizeSimple(int newWidth, int newHeight)
	{
		std::unique_ptr<Image> resizer(new Image);
		ResizePixelLayout pixelLayout{ ResizePixelLayout::RGB };
		ResizeDataType dataType{ ResizeDataType::UINT8 };
		ResizeEdge edge{ ResizeEdge::CLAMP };
		ResizeFilter filter{ ResizeFilter::MITCHELL };
		switch (outputChannels) {
		case 1: {
			pixelLayout = ResizePixelLayout::ONRCHANNEL;
			break;
		}
		case 3: {
			pixelLayout = ResizePixelLayout::RGB;
			break;
		}
		case 4: {
			pixelLayout = ResizePixelLayout::RGBA;
			break;
		}
		default:
			break;
		}
		auto afterResize = stbir_resize(
			m_img->data(), m_img->x, m_img->y, 0,
			NULL, newWidth, newHeight, 0,
			static_cast<stbir_pixel_layout>(pixelLayout),
			static_cast<stbir_datatype>(dataType),
			static_cast<stbir_edge>(edge),
			static_cast<stbir_filter>(filter)
		);
		if (!afterResize) return 0;
		resizer->img.reset(static_cast<unsigned char*>(afterResize));
		m_img = std::move(resizer);
		return 1;
	}
	int StbWrapper::resizeOption(int newWidth, int newHeight, const ResizeOption& option)
	{
		std::unique_ptr<Image> resizer(new Image);
		ResizePixelLayout pixelLayout{ ResizePixelLayout::RGB };
		ResizeDataType dataType{};
		ResizeEdge edge{};
		ResizeFilter filter{};
		switch (outputChannels) {
		case 1: {
			pixelLayout = ResizePixelLayout::ONRCHANNEL;
			break;
		}
		case 3: {
			pixelLayout = ResizePixelLayout::RGB;
			break;
		}
		case 4: {
			pixelLayout = ResizePixelLayout::RGBA;
			break;
		}
		default:
			break;
		}
		dataType = option.dataType != ResizeDataType::UNKNOWN ? option.dataType : ResizeDataType::UINT8;
		edge = option.edge != ResizeEdge::UNKNOWN ? option.edge : ResizeEdge::CLAMP;
		filter = option.filter != ResizeFilter::UNKNOWN ? option.filter : ResizeFilter::MITCHELL;
		auto afterResize = stbir_resize(
			m_img->data(), m_img->x, m_img->y, 0,
			NULL, newWidth, newHeight, 0,
			static_cast<stbir_pixel_layout>(pixelLayout),
			static_cast<stbir_datatype>(dataType),
			static_cast<stbir_edge>(edge),
			static_cast<stbir_filter>(filter)
		);
		if (!afterResize) return 0;
		resizer->img.reset(static_cast<unsigned char*>(afterResize));
		m_img = std::move(resizer);
		return 1;
	}
	static unsigned char bilinear_interpolate(
			const unsigned char* input,
			int w, int h, int channels,
			float x, float y, int channel
		) {
		int x0 = (int)floorf(x);
		int y0 = (int)floorf(y);
		int x1 = x0 + 1;
		int y1 = y0 + 1;
		// 边界检查
		x0 = static_cast<int>(fmaxf(0, fminf(static_cast<float>(x0), static_cast<float>(w - 1))));
		y0 = static_cast<int>(fmaxf(0, fminf(static_cast<float>(y0), static_cast<float>(h - 1))));
		x1 = static_cast<int>(fmaxf(0, fminf(static_cast<float>(x1), static_cast<float>(w - 1))));
		y1 = static_cast<int>(fmaxf(0, fminf(static_cast<float>(y1), static_cast<float>(h - 1))));
		// 获取四个邻近像素的值
		unsigned char p00 = input[(y0 * w + x0) * channels + channel];
		unsigned char p01 = input[(y0 * w + x1) * channels + channel];
		unsigned char p10 = input[(y1 * w + x0) * channels + channel];
		unsigned char p11 = input[(y1 * w + x1) * channels + channel];
		// 计算插值权重
		float dx = x - x0;
		float dy = y - y0;
		// 双线性插值公式
		float val =
			p00 * (1 - dx) * (1 - dy) +
			p01 * dx * (1 - dy) +
			p10 * (1 - dx) * dy +
			p11 * dx * dy;

		return (unsigned char)fminf(fmaxf(val, 0), 255);
	}
	// 
	int xiaochufuji::StbWrapper::rotateN90_nearest(float angleDegree)
	{
		int out_w = 0, out_h = 0, channels = outputChannels;
		float angle = static_cast<float>(angleDegree * M_PI / 180.0f);
		float cos_theta = cosf(angle);
		float sin_theta = sinf(angle);

		float new_w = fabsf(outputWidth * cos_theta) + fabsf(outputHeight * sin_theta);
		float new_h = fabsf(outputWidth * sin_theta) + fabsf(outputHeight * cos_theta);
		out_w = (int)ceilf(new_w);
		out_h = (int)ceilf(new_h);

		unsigned char* rotated = new unsigned char[out_w * out_h * channels];
		if (!rotated) return 0;

		// 旋转中心坐标
		float cx = outputWidth / 2.0f;
		float cy = outputHeight / 2.0f;
		float new_cx = out_w / 2.0f;
		float new_cy = out_h / 2.0f;

		for (int y = 0; y < out_h; ++y) {
			for (int x = 0; x < out_w; ++x) {
				// 将坐标转换到旋转前的坐标系
				float xpos = (x - new_cx) * cos_theta + (y - new_cy) * sin_theta + cx;
				float ypos = -(x - new_cx) * sin_theta + (y - new_cy) * cos_theta + cy;
				// 最近邻插值
				int x0 = (int)roundf(xpos);
				int y0 = (int)roundf(ypos);
				// 计算像素索引
				unsigned char* pixel = rotated + (y * out_w + x) * channels;
				if (x0 >= 0 && x0 < outputWidth && y0 >= 0 && y0 < outputHeight) {
					const unsigned char* src_pixel = static_cast<unsigned char*>(m_img->data()) + (y0 * outputWidth + x0) * channels;
					for (int c = 0; c < channels; ++c) {
						pixel[c] = src_pixel[c];
					}
				}
				else {
					// 超出范围区域填充透明/黑色
					for (int c = 0; c < channels; ++c) {
						// Alpha通道设为0，其他通道0（透明黑）
						pixel[c] = (c == 3) ? 0 : 0;
					}
				}
			}
		}
		std::unique_ptr<Image> rotater(new Image);
		rotater->img.reset(rotated);
		m_img = std::move(rotater);
		outputWidth = out_w;
		outputHeight = out_h;
		return 1;
	}
	// bilinear
	int xiaochufuji::StbWrapper::rotateN90_bilinear(float angleDegree)
	{
		int out_w = 0, out_h = 0, channels = outputChannels;
		// 将角度转换为弧度
		float angle = static_cast<float>(angleDegree * M_PI / 180.0f);
		float cos_theta = cosf(angle);
		float sin_theta = sinf(angle);
		// 计算新图像尺寸
		float new_w = fabsf(outputWidth * cos_theta) + fabsf(outputHeight * sin_theta);
		float new_h = fabsf(outputWidth * sin_theta) + fabsf(outputHeight * cos_theta);
		out_w = (int)ceilf(new_w);
		out_h = (int)ceilf(new_h);
		unsigned char* rotated = new unsigned char[out_w * out_h * channels];
		if (!rotated) return 0;

		// 旋转中心坐标
		float cx = outputWidth / 2.0f;
		float cy = outputHeight / 2.0f;
		float new_cx = out_w / 2.0f;
		float new_cy = out_h / 2.0f;
		for (int y = 0; y < out_h; ++y) {
			for (int x = 0; x < out_w; ++x) {
				// 将坐标转换到旋转前的坐标系
				float xpos = (x - new_cx) * cos_theta + (y - new_cy) * sin_theta + cx;
				float ypos = -(x - new_cx) * sin_theta + (y - new_cy) * cos_theta + cy;

				// 计算目标像素的索引
				unsigned char* pixel = rotated + (y * out_w + x) * channels;
				if (xpos >= 0 && xpos < outputWidth - 1 && ypos >= 0 && ypos < outputHeight - 1) {
					// 双线性插值
					for (int c = 0; c < channels; ++c) {
						pixel[c] = bilinear_interpolate(static_cast<unsigned char*>(m_img->data()), outputWidth, outputHeight, channels, xpos, ypos, c);
					}
				}
				else {
					// 超出范围区域填充透明/黑色
					for (int c = 0; c < channels; ++c) {
						// Alpha通道设为0，其他通道0（透明黑）
						pixel[c] = (c == 3) ? 0 : 0;
					}
				}
			}
		}
		std::unique_ptr<Image> rotater(new Image);
		rotater->img.reset(rotated);
		m_img = std::move(rotater);
		outputWidth = out_w;
		outputHeight = out_h;
		return 1;
	}
	int xiaochufuji::StbWrapper::rotate90(float angleDegree)
	{ 
		int out_w = 0, out_h = 0, channels = outputChannels;
		// 计算旋转后的图像尺寸
		int rotation_count = ((int)(angleDegree / 90.0f) % 4 + 4) % 4; // 归一化到 [0, 3]
		if (rotation_count % 2 == 1) {
			out_w = outputHeight;
			out_h = outputWidth;
		}
		else {
			out_w = outputWidth;
			out_h = outputHeight;
		}

		unsigned char* rotated = new unsigned char[out_w * out_h * channels];
		if (!rotated) return 0;

		// 根据旋转次数进行像素坐标变换
		for (int y = 0; y < outputHeight; ++y) {
			for (int x = 0; x < outputWidth; ++x) {
				int src_index = (y * outputWidth + x) * channels;
				int dst_index;

				switch (rotation_count) {
				case 1: // 90
					dst_index = (x * out_w + (out_w - y - 1)) * channels;
					break;
				case 2: // 180 
					dst_index = ((outputHeight - y - 1) * outputWidth + (outputWidth - x - 1)) * channels;
					break;
				case 3: // 270 
					dst_index = ((outputHeight - x - 1) * out_w + y) * channels;
					break;
				default: // 0 
					dst_index = src_index;
					break;
				}
				// 复制像素数据
				for (int c = 0; c < channels; ++c) {
					rotated[dst_index + c] = static_cast<unsigned char*>(m_img->data())[src_index + c];
				}
			}
		}
		std::unique_ptr<Image> rotater(new Image);
		rotater->img.reset(rotated);
		m_img = std::move(rotater);
		outputWidth = out_w;
		outputHeight = out_h;
		return 1;
	}
}


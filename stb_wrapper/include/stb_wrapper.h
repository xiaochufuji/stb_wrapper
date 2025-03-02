#pragma once
#include <iostream>
#include <memory>
#include "stb_api_abstract.h"

namespace xiaochufuji
{
	// declare Imgae instance
	struct Image;
	enum ColorFormat : char { ORIGIN = 0, GRAY = 1, RGB = 3, RGBA = 4 };
	// save option
	enum ImageFormat : char { JPG, PNG, BMP, HDR, TGA };
	union SaveOption
	{
		int quality;
		int stride_in_bytes;
	};
#pragma region resize option
	// resize option
	enum class ResizePixelLayout : int
	{
		UNKNOWN = -1,
		ONRCHANNEL = stbir_pixel_layout_api::STBIR_1CHANNEL,
		TWOCHANNEL = stbir_pixel_layout_api::STBIR_2CHANNEL,
		RGB = stbir_pixel_layout_api::STBIR_RGB,               // 3-chan, with order specified (for channel flipping)
		BGR = stbir_pixel_layout_api::STBIR_BGR,               // 3-chan, with order specified (for channel flipping)
		FORCHANNEL = stbir_pixel_layout_api::STBIR_4CHANNEL,

		RGBA = stbir_pixel_layout_api::STBIR_RGBA,                   // alpha formats, where alpha is NOT premultiplied into color channels
		BGRA = stbir_pixel_layout_api::STBIR_BGRA,
		ARGB = stbir_pixel_layout_api::STBIR_ARGB,
		ABGR = stbir_pixel_layout_api::STBIR_ABGR,
		RA = stbir_pixel_layout_api::STBIR_RA,
		AR = stbir_pixel_layout_api::STBIR_AR,

		RGBA_PM = stbir_pixel_layout_api::STBIR_RGBA_PM,               // alpha formats, where alpha is premultiplied into color channels
		BGRA_PM = stbir_pixel_layout_api::STBIR_BGRA_PM,
		ARGB_PM = stbir_pixel_layout_api::STBIR_ARGB_PM,
		ABGR_PM = stbir_pixel_layout_api::STBIR_ABGR_PM,
		RA_PM = stbir_pixel_layout_api::STBIR_RA_PM,
		AR_PM = stbir_pixel_layout_api::STBIR_AR_PM,

		RGBA_NO_AW = stbir_pixel_layout_api::STBIR_RGBA_NO_AW,            // alpha formats, where NO alpha weighting is applied at all!
		BGRA_NO_AW = stbir_pixel_layout_api::STBIR_BGRA_NO_AW,            //   these are just synonyms for the _PM flags (which also do
		ARGB_NO_AW = stbir_pixel_layout_api::STBIR_ARGB_NO_AW,            //   no alpha weighting). These names just make it more clear
		ABGR_NO_AW = stbir_pixel_layout_api::STBIR_ABGR_NO_AW,            //   for some folks).
		RA_NO_AW = stbir_pixel_layout_api::STBIR_RA_NO_AW,
		AR_NO_AW = stbir_pixel_layout_api::STBIR_AR_NO_AW,
	};
	enum class ResizeEdge : int {
		UNKNOWN = -1,
		CLAMP = stbir_edge_api::STBIR_EDGE_CLAMP,		// 超出范围时，使用最近的边缘像素填充
		REFLECT = stbir_edge_api::STBIR_EDGE_REFLECT, 	// 镜像填充（如 A B C D -> D C B A）
		WRAP = stbir_edge_api::STBIR_EDGE_WRAP,  		// 环绕填充（如 A B C D -> D A B C）
		ZERO = stbir_edge_api::STBIR_EDGE_ZERO  		// 超出范围的部分填充为黑色（0 值）
	};
	enum class ResizeFilter :int {
		UNKNOWN = -1,
		DEFAULT = stbir_filter_api::STBIR_FILTER_DEFAULT,				// 默认（自动选择）
		BOX = stbir_filter_api::STBIR_FILTER_BOX,						// 盒子滤波（模糊但最快）
		TRIANGLE = stbir_filter_api::STBIR_FILTER_TRIANGLE,				// 双线性插值（默认）
		CUBICBSPLINE = stbir_filter_api::STBIR_FILTER_CUBICBSPLINE,		// 三次 B 样条曲线
		CATMULLROM = stbir_filter_api::STBIR_FILTER_CATMULLROM,			// Catmull-Rom 插值（更锐利）
		MITCHELL = stbir_filter_api::STBIR_FILTER_MITCHELL,				// Mitchell 插值（平衡锐度 & 平滑度）
		POINT_SAMPLE = stbir_filter_api::STBIR_FILTER_POINT_SAMPLE,		// 直接取最近点（无插值）
		OTHER = stbir_filter_api::STBIR_FILTER_OTHER					// 用户自定义插值
	};
	enum class ResizeDataType :int {
		UNKNOWN = -1,
		UINT8 = stbir_datatype_api::STBIR_TYPE_UINT8,				// 8-bit（普通 PNG/JPG）
		UINT8_SRGB = stbir_datatype_api::STBIR_TYPE_UINT8_SRGB,		// 8-bit sRGB
		UINT16 = stbir_datatype_api::STBIR_TYPE_UINT16,				// 16-bit（HDR）
		FLOAT = stbir_datatype_api::STBIR_TYPE_FLOAT				// 32-bit float（HDR）
	};
	struct ResizeOption
	{
		explicit ResizeOption() {};
		explicit ResizeOption(ResizePixelLayout pixelLayout, ResizeEdge edge, ResizeFilter filter, ResizeDataType dataType)
			: pixelLayout(pixelLayout), edge(edge), filter(filter), dataType(dataType) {
		}
		ResizePixelLayout pixelLayout{ ResizePixelLayout::UNKNOWN };
		ResizeEdge edge{ ResizeEdge::UNKNOWN };
		ResizeFilter filter{ ResizeFilter::UNKNOWN };
		ResizeDataType dataType{ ResizeDataType::UNKNOWN };
	};
	bool operator==(const ResizeOption& lval, const ResizeOption& rval);
#pragma endregion
	class StbWrapper
	{
	public:
		explicit StbWrapper(const std::string& path, ColorFormat format = ColorFormat::ORIGIN);
		StbWrapper(StbWrapper&& old) noexcept;
		StbWrapper& operator=(StbWrapper&& old) noexcept;
		~StbWrapper();
	public:
		unsigned char* data();
		void reload(const std::string& path, ColorFormat format = ColorFormat::ORIGIN);
		void reload(unsigned char* memoryBuffer, int bufferLen, ColorFormat format = ColorFormat::ORIGIN);
		int resize(int newWidth, int newHeight, const ResizeOption& option = ResizeOption());
		int rotate(float angleDegree);
		int save(const std::string& outputPath, ImageFormat format = ImageFormat::JPG, SaveOption option = SaveOption(75));
		int autoSave(const std::string& outputPath, SaveOption option = SaveOption(75));
	private:
		void copyProperties();
		int handleSaveJpg(const char* filename, SaveOption option);
		int handleSavePng(const char* filename, SaveOption option);
		int handleSaveBmp(const char* filename, SaveOption option);
		int handleSaveHdr(const char* filename, SaveOption option);
		int handleSaveTga(const char* filename, SaveOption option);

		int resizeSimple(int newWidth, int newHeight);
		int resizeOption(int newWidth, int newHeight, const ResizeOption& option);
		int rotateN90_nearest(float angleDegree);
		int rotateN90_bilinear(float angleDegree);
		int rotate90(float angleDegree);
	private:
		std::unique_ptr<Image> m_img{ nullptr };
		int outputChannels = 0;
		int outputWidth = 0;
		int outputHeight = 0;
	};
}
#pragma once
#ifdef _MSC_VER
#define DECLARE_UNIMPLEMENTED_FUNCTION(...) \
    __pragma(warning(push))                 \
    __pragma(warning(disable:4505))         \
    __VA_ARGS__;                            \
    __pragma(warning(pop))
#else
#define DECLARE_UNIMPLEMENTED_FUNCTION(...) __attribute__((unused)) __VA_ARGS__
#endif
//===============================================================
//  简单 API
//
//  如果 `output_pixels` 传入 NULL（0），我们会自动分配缓冲区，并返回指针。
//===============================================================

// stbir_pixel_layout_api specifies:
//   number of channels
//   order of channels
//   whether color is premultiplied by alpha
// for back compatibility, you can cast the old channel count to an stbir_pixel_layout_api
typedef enum class stbir_pixel_layout_api
{
    STBIR_1CHANNEL = 1,
    STBIR_2CHANNEL = 2,
    STBIR_RGB = 3,               // 3-chan, with order specified (for channel flipping)
    STBIR_BGR = 0,               // 3-chan, with order specified (for channel flipping)
    STBIR_4CHANNEL = 5,

    STBIR_RGBA = 4,                   // alpha formats, where alpha is NOT premultiplied into color channels
    STBIR_BGRA = 6,
    STBIR_ARGB = 7,
    STBIR_ABGR = 8,
    STBIR_RA = 9,
    STBIR_AR = 10,

    STBIR_RGBA_PM = 11,               // alpha formats, where alpha is premultiplied into color channels
    STBIR_BGRA_PM = 12,
    STBIR_ARGB_PM = 13,
    STBIR_ABGR_PM = 14,
    STBIR_RA_PM = 15,
    STBIR_AR_PM = 16,

    STBIR_RGBA_NO_AW = 11,            // alpha formats, where NO alpha weighting is applied at all!
    STBIR_BGRA_NO_AW = 12,            //   these are just synonyms for the _PM flags (which also do
    STBIR_ARGB_NO_AW = 13,            //   no alpha weighting). These names just make it more clear
    STBIR_ABGR_NO_AW = 14,            //   for some folks).
    STBIR_RA_NO_AW = 15,
    STBIR_AR_NO_AW = 16,

} stbir_pixel_layout_api;
/*****************************************************************
       用于 8-bit sRGB 图片（PNG/JPG）。
       pixel_type 指定像素格式（如 STBIR_RGB）。
       output_pixels = NULL 时，库会自动分配输出缓冲区，并返回指针（记得 free() 释放）。
 *******************************************************************/
DECLARE_UNIMPLEMENTED_FUNCTION(
unsigned char* stbir_resize_uint8_srgb_api(
    const unsigned char* input_pixels, int input_w, int input_h, int input_stride_in_bytes,
    unsigned char* output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout_api pixel_type);
)

/*****************************************************************
      与 stbir_resize_uint8_srgb() 类似，但用于 线性颜色空间（非 sRGB）。
 *******************************************************************/
DECLARE_UNIMPLEMENTED_FUNCTION(
unsigned char* stbir_resize_uint8_linear_api(
    const unsigned char* input_pixels, int input_w, int input_h, int input_stride_in_bytes,
    unsigned char* output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout_api pixel_type);
)

/*****************************************************************
     用于 HDR 图片（.hdr 格式）。
     float* 代表 浮点型像素数据（0.0~1.0 颜色值）。
 *******************************************************************/
DECLARE_UNIMPLEMENTED_FUNCTION(
float* stbir_resize_float_linear_api(
    const float* input_pixels, int input_w, int input_h, int input_stride_in_bytes,
    float* output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout_api pixel_type);
)

//===============================================================
//  中等复杂度 API
//
//  这个 API 相比简单 API 增加了以下功能：
//
//    * 可指定数据类型（U8, U8_SRGB, U16, FLOAT, HALF_FLOAT）
//    * 可指定边缘处理模式（Edge wrap）
//    * 可选择插值算法（Filter）
//===============================================================
/********************************************************************
    stbir_edge_api_CLAMP（默认）：缩放时，边缘使用最近的像素填充，防止黑边。
    stbir_edge_api_REFLECT：适用于 对称图像，如面部或纹理。
    stbir_edge_api_WRAP：适用于 重复贴图，比如游戏纹理。
    stbir_edge_api_ZERO：填充黑色，适用于透明背景。
********************************************************************/
typedef enum class stbir_edge_api
{
    STBIR_EDGE_CLAMP = 0,   // 超出范围时，使用最近的边缘像素填充
    STBIR_EDGE_REFLECT = 1, // 镜像填充（如 A B C D -> D C B A）
    STBIR_EDGE_WRAP = 2,    // 环绕填充（如 A B C D -> D A B C）
    STBIR_EDGE_ZERO = 3,    // 超出范围的部分填充为黑色（0 值）
} stbir_edge_api;


/*********************************************************************
    默认：stbir_filter_api_TRIANGLE（双线性插值），适用于普通缩放。
    锐利效果：stbir_filter_api_CATMULLROM，适用于文本/高对比图像。
    柔和平滑：stbir_filter_api_CUBICBSPLINE，适用于照片。
    最精确：stbir_filter_api_MITCHELL，适用于高质量缩放。
********************************************************************/
typedef enum class stbir_filter_api
{
    STBIR_FILTER_DEFAULT = 0,  // 默认（自动选择）
    STBIR_FILTER_BOX = 1,  // 盒子滤波（模糊但最快）
    STBIR_FILTER_TRIANGLE = 2,  // 双线性插值（默认）
    STBIR_FILTER_CUBICBSPLINE = 3,  // 三次 B 样条曲线
    STBIR_FILTER_CATMULLROM = 4,  // Catmull-Rom 插值（更锐利）
    STBIR_FILTER_MITCHELL = 5,  // Mitchell 插值（平衡锐度 & 平滑度）
    STBIR_FILTER_POINT_SAMPLE = 6,  // 直接取最近点（无插值）
    STBIR_FILTER_OTHER = 7,  // 用户自定义插值
} stbir_filter_api;


/*********************************************************************
    普通 PNG/JPG：STBIR_TYPE_UINT8
    HDR：STBIR_TYPE_FLOAT
********************************************************************/
typedef enum class stbir_datatype_api
{
    STBIR_TYPE_UINT8 = 0,  // 8-bit（普通 PNG/JPG）
    STBIR_TYPE_UINT8_SRGB = 1,  // 8-bit sRGB
    STBIR_TYPE_UINT16 = 3,  // 16-bit（HDR）
    STBIR_TYPE_FLOAT = 4,  // 32-bit float（HDR）
} stbir_datatype_api;


/*********************************************************************
    允许你 自定义插值算法、边界模式，适用于 高质量缩放。
********************************************************************/
DECLARE_UNIMPLEMENTED_FUNCTION(
void* stbir_resize_api(const void* input_pixels, int input_w, int input_h, int input_stride_in_bytes,
    void* output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout_api pixel_layout, stbir_datatype_api data_type,
    stbir_edge_api edge, stbir_filter_api filter);
)
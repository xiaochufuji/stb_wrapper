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
//  �� API
//
//  ��� `output_pixels` ���� NULL��0�������ǻ��Զ����仺������������ָ�롣
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
       ���� 8-bit sRGB ͼƬ��PNG/JPG����
       pixel_type ָ�����ظ�ʽ���� STBIR_RGB����
       output_pixels = NULL ʱ������Զ����������������������ָ�루�ǵ� free() �ͷţ���
 *******************************************************************/
DECLARE_UNIMPLEMENTED_FUNCTION(
unsigned char* stbir_resize_uint8_srgb_api(
    const unsigned char* input_pixels, int input_w, int input_h, int input_stride_in_bytes,
    unsigned char* output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout_api pixel_type);
)

/*****************************************************************
      �� stbir_resize_uint8_srgb() ���ƣ������� ������ɫ�ռ䣨�� sRGB����
 *******************************************************************/
DECLARE_UNIMPLEMENTED_FUNCTION(
unsigned char* stbir_resize_uint8_linear_api(
    const unsigned char* input_pixels, int input_w, int input_h, int input_stride_in_bytes,
    unsigned char* output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout_api pixel_type);
)

/*****************************************************************
     ���� HDR ͼƬ��.hdr ��ʽ����
     float* ���� �������������ݣ�0.0~1.0 ��ɫֵ����
 *******************************************************************/
DECLARE_UNIMPLEMENTED_FUNCTION(
float* stbir_resize_float_linear_api(
    const float* input_pixels, int input_w, int input_h, int input_stride_in_bytes,
    float* output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout_api pixel_type);
)

//===============================================================
//  �еȸ��Ӷ� API
//
//  ��� API ��ȼ� API ���������¹��ܣ�
//
//    * ��ָ���������ͣ�U8, U8_SRGB, U16, FLOAT, HALF_FLOAT��
//    * ��ָ����Ե����ģʽ��Edge wrap��
//    * ��ѡ���ֵ�㷨��Filter��
//===============================================================
/********************************************************************
    stbir_edge_api_CLAMP��Ĭ�ϣ�������ʱ����Եʹ�������������䣬��ֹ�ڱߡ�
    stbir_edge_api_REFLECT�������� �Գ�ͼ�����沿������
    stbir_edge_api_WRAP�������� �ظ���ͼ��������Ϸ����
    stbir_edge_api_ZERO������ɫ��������͸��������
********************************************************************/
typedef enum class stbir_edge_api
{
    STBIR_EDGE_CLAMP = 0,   // ������Χʱ��ʹ������ı�Ե�������
    STBIR_EDGE_REFLECT = 1, // ������䣨�� A B C D -> D C B A��
    STBIR_EDGE_WRAP = 2,    // ������䣨�� A B C D -> D A B C��
    STBIR_EDGE_ZERO = 3,    // ������Χ�Ĳ������Ϊ��ɫ��0 ֵ��
} stbir_edge_api;


/*********************************************************************
    Ĭ�ϣ�stbir_filter_api_TRIANGLE��˫���Բ�ֵ������������ͨ���š�
    ����Ч����stbir_filter_api_CATMULLROM���������ı�/�߶Ա�ͼ��
    ���ƽ����stbir_filter_api_CUBICBSPLINE����������Ƭ��
    �ȷ��stbir_filter_api_MITCHELL�������ڸ��������š�
********************************************************************/
typedef enum class stbir_filter_api
{
    STBIR_FILTER_DEFAULT = 0,  // Ĭ�ϣ��Զ�ѡ��
    STBIR_FILTER_BOX = 1,  // �����˲���ģ������죩
    STBIR_FILTER_TRIANGLE = 2,  // ˫���Բ�ֵ��Ĭ�ϣ�
    STBIR_FILTER_CUBICBSPLINE = 3,  // ���� B ��������
    STBIR_FILTER_CATMULLROM = 4,  // Catmull-Rom ��ֵ����������
    STBIR_FILTER_MITCHELL = 5,  // Mitchell ��ֵ��ƽ����� & ƽ���ȣ�
    STBIR_FILTER_POINT_SAMPLE = 6,  // ֱ��ȡ����㣨�޲�ֵ��
    STBIR_FILTER_OTHER = 7,  // �û��Զ����ֵ
} stbir_filter_api;


/*********************************************************************
    ��ͨ PNG/JPG��STBIR_TYPE_UINT8
    HDR��STBIR_TYPE_FLOAT
********************************************************************/
typedef enum class stbir_datatype_api
{
    STBIR_TYPE_UINT8 = 0,  // 8-bit����ͨ PNG/JPG��
    STBIR_TYPE_UINT8_SRGB = 1,  // 8-bit sRGB
    STBIR_TYPE_UINT16 = 3,  // 16-bit��HDR��
    STBIR_TYPE_FLOAT = 4,  // 32-bit float��HDR��
} stbir_datatype_api;


/*********************************************************************
    ������ �Զ����ֵ�㷨���߽�ģʽ�������� ���������š�
********************************************************************/
DECLARE_UNIMPLEMENTED_FUNCTION(
void* stbir_resize_api(const void* input_pixels, int input_w, int input_h, int input_stride_in_bytes,
    void* output_pixels, int output_w, int output_h, int output_stride_in_bytes,
    stbir_pixel_layout_api pixel_layout, stbir_datatype_api data_type,
    stbir_edge_api edge, stbir_filter_api filter);
)
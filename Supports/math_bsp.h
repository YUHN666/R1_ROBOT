#ifndef _MATH_BSP_H_
#define _MATH_BSP_H_

#include "main.h"
#include <math.h>
#include <stdbool.h>
#define MATHBSP_PI 3.14159265358979323846f
#define MATHBSP_E  2.71828182845904523536f

#define MathBsp_Abs(x) ((x) < 0 ? -(x) : (x))
#define MathBsp_Min(x, y) ((x) < (y) ? (x) : (y))
#define MathBsp_Max(x, y) ((x) > (y) ? (x) : (y))
#define MathBsp_Clamp(x, min, max) MathBsp_Min(MathBsp_Max((x), (min)), (max))
#define MathBsp_Square(x) ((x) * (x))
#define MathBsp_Mean(x, y) (((x) + (y)) / 2.0f)
#define MathBsp_GeometricMean(x, y) (sqrt((x) * (y)))
#define MathBsp_HarmonicMean(x, y) (2.0f / (1.0f / (x) + 1.0f / (y)))
#define MathBsp_Hypot(x, y) (sqrt((x) * (x) + (y) * (y)))

#define MathBsp_XMode(a, b) (((a) >= 0) ? (fmod((a), (b))) : ((b) - fmod(-(a), (b))))

const uint8_t* MathBsp_Crc8Table(void);
const uint16_t* MathBsp_Crc16Table(void);
uint8_t MathBsp_Crc8(void* pdata, uint32_t byte_len);
uint16_t MathBsp_Crc16(void* pdata, uint32_t byte_len);

#define MATHBSP_TYPEBASED_FUNC_REFERENCE_LIST(reference, func)          \
    reference(double    ,   func##_double)                              \
    reference(float     ,   func##_float)                               \
    reference(uint64_t  ,   func##_uint64)                              \
    reference(int64_t   ,   func##_int64)                               \
    reference(uint32_t  ,   func##_uint32)                              \
    reference(int32_t   ,   func##_int32)                               \
    reference(uint16_t  ,   func##_uint16)                              \
    reference(int16_t   ,   func##_int16)                               \
    reference(uint8_t   ,   func##_uint8)                               \
    reference(int8_t    ,   func##_int8)
#define MATHBSP_TYPEBASED_INTEGER_FUNC_REFERENCE_LIST(reference, func)  \
    reference(uint64_t  ,   func##_uint64)                              \
    reference(int64_t   ,   func##_int64)                               \
    reference(uint32_t  ,   func##_uint32)                              \
    reference(int32_t   ,   func##_int32)                               \
    reference(uint16_t  ,   func##_uint16)                              \
    reference(int16_t   ,   func##_int16)                               \
    reference(uint8_t   ,   func##_uint8)                               \
    reference(int8_t    ,   func##_int8)

/*
*   归并排序：支持基本类型
*   type* MathBsp_MergeSort_type(type* source, type* buff, uint32_t len); 其中type为基本类型
*   source: 待排序数组
*   buff:   临时缓冲区
*   len:    数组长度
*   返回值: 排序后的数组(指向source)，其中source和buff都会得到排完后的数组
*/
#define MATHBSP_MERGESORT_REFERENCE(type, func)  type* func(type* source, type* buff, uint32_t len);
MATHBSP_TYPEBASED_FUNC_REFERENCE_LIST(MATHBSP_MERGESORT_REFERENCE, MathBsp_MergeSort)
/*
*   快速排序：支持基本类型
*   type* MathBsp_QuickSort_type(type* source, uint32_t len); 其中type为基本类型
*   source: 待排序数组
*   len:    数组长度
*   返回值: 排序后的数组(指向source)
*/
#define MATHBSP_QUICKSORT_REFERENCE(type, func)  type* func(type* source, uint32_t len);
MATHBSP_TYPEBASED_FUNC_REFERENCE_LIST(MATHBSP_QUICKSORT_REFERENCE, MathBsp_QuickSort)
/*
*   堆排序：支持基本类型
*   type* MathBsp_HeapSort_type(type* source, uint32_t len); 其中type为基本类型
*   source: 待排序数组
*   len:    数组长度
*   返回值: 排序后的数组(指向source)
*/
#define MATHBSP_HEAPSORT_REFERENCE(type, func)  type* func(type* source, uint32_t len);
MATHBSP_TYPEBASED_FUNC_REFERENCE_LIST(MATHBSP_HEAPSORT_REFERENCE, MathBsp_HeapSort)

//获取一个整数的二进制表示中最高位的1的位置
#define MATHBSP_GETHIGHESTBIT_REFERENCE(type, func) uint32_t func(type num);
MATHBSP_TYPEBASED_INTEGER_FUNC_REFERENCE_LIST(MATHBSP_GETHIGHESTBIT_REFERENCE, MathBsp_GetHighestBit)
//获取一个数的二进制表示中最低位的1的位置
#define MATHBSP_GETLOWESTBIT_REFERENCE(type, func) uint32_t func(type num);
MATHBSP_TYPEBASED_INTEGER_FUNC_REFERENCE_LIST(MATHBSP_GETLOWESTBIT_REFERENCE, MathBsp_GetLowestBit)
//将一个整数的第n位设置为1/0
#define MATHBSP_SETBIT_REFERENCE(type, func) type func(type num, uint32_t n, bool bitstate);
MATHBSP_TYPEBASED_INTEGER_FUNC_REFERENCE_LIST(MATHBSP_SETBIT_REFERENCE, MathBsp_SetBit)
//计算一个整数的二进制表示中1的个数
#define MATHBSP_COUNTBIT_REFERENCE(type, func) uint32_t func(type num);
MATHBSP_TYPEBASED_INTEGER_FUNC_REFERENCE_LIST(MATHBSP_COUNTBIT_REFERENCE, MathBsp_CountBit)
//获取一个二进制整数的第n位是1还是0
#define MATHBSP_GETBIT_REFERENCE(type, func) bool func(type num, uint32_t n);
MATHBSP_TYPEBASED_INTEGER_FUNC_REFERENCE_LIST(MATHBSP_GETBIT_REFERENCE, MathBsp_GetBit)


#endif
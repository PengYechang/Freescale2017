#ifndef	__IMAGE_CONTROL_H__
#define	__IMAGE_CONTROL_H__

#include "camera.h"

#define FIND_COUNT		3				// 定义跟踪搜索的范围
#define TYPE_1			1
#define TYPE_2			2
#define FIND_TYPE		TYPE_2			// 定义中线的查找模式
#define POINT_COUNT		10				// 定义最小二乘法拟合的点数


// 传入二维图形数组，返回中线数组
int16_t *findCenterLine(uint8_t (* image)[Image_Width]);


#endif
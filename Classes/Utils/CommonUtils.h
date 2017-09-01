/*
 * CommonUtils.h
 *
 *  Created on: 2016年8月5日
 *      Author: arvin.li
 */


#ifndef COMMONUTILS_H_
#define COMMONUTILS_H_

#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/prettywriter.h"

/**
 * 获取字符、字符串数组长度模板
 */
template <class T>
int getArrayLen(T &array) {
	return (sizeof(array) / sizeof(array[0]));
}

/**
 * 将JSON转换成string模板
 */
template <class T>
std::string getStringFromJson(T &json) {
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	json.Accept(writer);

	return buffer.GetString();
}

/**
 * 根据触摸点获取旋转角度
 */
template <class T>
float getDegreeByPoints(const T &touchPoint, const T &objPoint) {
	return atan2(touchPoint.x - objPoint.x, touchPoint.y - objPoint.y) * 180 / 3.1415926;
}

#endif /* COMMONUTILS_H_ */

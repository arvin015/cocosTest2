/*
 * PacToast.h
 *
 *  Created on: 2016年8月12日
 *      Author: arvin.li
 */

#ifndef PACTOAST_H_
#define PACTOAST_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class PacToast {
    
public:

	PacToast();
	virtual ~PacToast();

    static void makeText(const std::string &msg);
    static void makeText(const std::string &msg, float time);
    static void makeText(cocos2d::Node* node, const std::string &msg, float time, bool autoDissmiss=true);
    static void makeTextNoDismiss(const std::string &msg); //Toast不消失
    
    //隐藏Toast
    static void dismissToast();
};

#endif /* PACTOAST_H_ */

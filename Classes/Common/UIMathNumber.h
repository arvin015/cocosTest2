//
//  UIMathNumber.h
//  modelmethod
//
//  Created by Alex on 7/11/2016.
//
//

#ifndef _UI_MATH_NUMBER_H_
#define _UI_MATH_NUMBER_H_

#include <string>

#include "ui/UILayout.h"
#include "ui/UIText.h"

#include "cocos2d.h"
USING_NS_CC;

class MathNumber :

public cocos2d::ui::Layout

{
    
public:
    
    MathNumber();
    ~MathNumber();
    
    CREATE_FUNC(MathNumber);
    
    virtual bool init();
    
    enum NumberType {
        Integer,// 整數
        FractionProper,// 真分數
        FractionImproper,// 假分數
        FractionMixed,// 帶分數
        Other// 其他類型
    };
    
    /**
     * 設置分數字體大小
     */
    void setFontSize(float fontSize);
    
    /**
     * 獲取分數字體大小
     */
    float getFontSize() {
        return _fontSize;
    }
    
    /**
     * 設置分數字體顏色
     */
    void setFontColor(const cocos2d::Color4B& fontColor);
    
    /**
     * 通過預定義數字描述格式初始化
     * @param pattern 整數如"a"，分數如"c/b" or "a+c/b"
     * @param isAdaptive 是否自適應，即：是否使得contentSize與文本內容大小相等
     */
    void setString(const std::string& pattern, bool isAdaptive = false);
    std::string getString();
    
    /**
     * 用戶輸入數字，每次輸入一個字符
     */
    void putString(const std::string& str);
    
    /**
     * 獲取左邊內容
     */
    std::string getLeftString();
    
    /**
     * 設置左邊內容
     */
    void setLeftString(const std::string &str);
    
    /**
     * 設置分數不可輸入字符-主要是小數點
     */
    void setIgnoreWord(const std::string &word);
    
    /**
     * 顯示數字的輸入區域
     */
    void showBorder();
    
    /**
     * 隱藏數字的輸入區域
     */
    void hideBorder();
    
    /**
     * 獲取數字的類型，是分數還是整數
     */
    NumberType getNumberType();
    
    int getInteger();// get整數
    int getDenominator();// get分母
    int getNumerator();// get分子
    
    bool isEmpty();
    
    bool isFractionLegal();
    
private:
    
    /**
     * 測量并設置當前分數的Size使其自適應，一般無需手動調用
     */
    void measureAndReSize();
    
    NumberType _type;// 數字類型
    
    float _fontSize;// 文字大小
    float _dividerHeight;// 分數分割線高度
    float _dividerWidthExt;// 分割線兩邊延長的長度in pixel
    
    std::string _integer;// 整數
    std::string _denominator;// 分母
    std::string _numerator;// 分子
    
    int _inputType;// 輸入類型有3種，0代表整數部分，1代表分母部分，2代表分子部分
    
    bool _showDefaultDivider;// 輸入分數的整數部分后，應顯示分數的默認分割線
    
    cocos2d::Color4B _fontColor;// 文字顏色
    
    std::string _ignoreWord; //忽略字符-arvin edit

    cocos2d::ui::Layout* _container;// 分數的父Layout
    cocos2d::ui::Text* _integerText;// 分數，整數部分
    cocos2d::ui::Layout* _dividerBar;// 分數，分割線
    cocos2d::ui::Text* _denominatorText;// 分數，分母
    cocos2d::ui::Text* _numeratorText;// 分數，分子
    
    cocos2d::DrawNode* _drawNode;
};

#endif /* _UI_MATH_NUMBER_H_ */

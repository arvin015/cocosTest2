//
//  UIMathNumber.cpp
//  modelmethod
//
//  Created by Alex on 7/11/2016.
//
//

#include "UIMathNumber.h"

USING_NS_CC;

using namespace cocos2d::ui;

MathNumber::MathNumber()
{
    _fontSize = 24.0f;
    _fontColor = Color4B::BLACK;
    _dividerHeight = 2.0f;
    _dividerWidthExt = 2.0f;
    
    _integer = "";
    _denominator = "";
    _numerator = "";
    
    _inputType = 0;
    
    _ignoreWord = "";
    
    _showDefaultDivider = false;
}

MathNumber::~MathNumber()
{}

bool MathNumber::init()
{
    if (!Layout::init()) {
        return false;
    }
    
    _drawNode = DrawNode::create();
    addChild(_drawNode);
    
    setLayoutType(Layout::Type::RELATIVE);
    
    // 分數佈局，用于佈局分數。且該佈局在當前佈局中居中
    _container = Layout::create();
    _container->setLayoutType(Layout::Type::RELATIVE);
    _container->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _container->setCascadeOpacityEnabled(true);
    addChild(_container);
    
    RelativeLayoutParameter* rp_layout = RelativeLayoutParameter::create();
    rp_layout->setAlign(RelativeLayoutParameter::RelativeAlign::CENTER_IN_PARENT);
    _container->setLayoutParameter(rp_layout);
    
    // 整數
    _integerText = Text::create();
    _integerText->setString("");
    _integerText->setFontSize(_fontSize);
    _integerText->setTextColor(_fontColor);
    _integerText->setCascadeColorEnabled(true);
    _container->addChild(_integerText);
    
    RelativeLayoutParameter* rp_integer = RelativeLayoutParameter::create();
    rp_integer->setRelativeName("integer");
    rp_integer->setAlign(RelativeLayoutParameter::RelativeAlign::PARENT_LEFT_CENTER_VERTICAL);
    _integerText->setLayoutParameter(rp_integer);
    
    // 分割線
    _dividerBar = Layout::create();
    _dividerBar->setBackGroundColor(Color3B(_fontColor.r, _fontColor.g, _fontColor.b));
    _dividerBar->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    _dividerBar->setCascadeColorEnabled(true);
    _container->addChild(_dividerBar);
    
    RelativeLayoutParameter* rp_division_bar = RelativeLayoutParameter::create();
    rp_division_bar->setRelativeName("divider");
    rp_division_bar->setRelativeToWidgetName("integer");
    rp_division_bar->setAlign(RelativeLayoutParameter::RelativeAlign::LOCATION_RIGHT_OF_CENTER);
    rp_division_bar->setMargin(Margin(0, 0, 0, 0));
    _dividerBar->setLayoutParameter(rp_division_bar);
    
    // 分母
    _denominatorText = Text::create();
    _denominatorText->setString("");
    _denominatorText->setFontSize(_fontSize);
    _denominatorText->setTextColor(_fontColor);
    _denominatorText->setCascadeColorEnabled(true);
    _container->addChild(_denominatorText);
    
    RelativeLayoutParameter* rp_denominator = RelativeLayoutParameter::create();
    rp_denominator->setRelativeToWidgetName("divider");
    rp_denominator->setAlign(RelativeLayoutParameter::RelativeAlign::LOCATION_BELOW_CENTER);
    rp_denominator->setMargin(Margin(0, 0, 0, 0));
    _denominatorText->setLayoutParameter(rp_denominator);
    
    // 分子
    _numeratorText = Text::create();
    _numeratorText->setString("");
    _numeratorText->setFontSize(_fontSize);
    _numeratorText->setTextColor(_fontColor);
    _numeratorText->setCascadeColorEnabled(true);
    _container->addChild(_numeratorText);
    
    RelativeLayoutParameter* rp_numerator = RelativeLayoutParameter::create();
    rp_numerator->setRelativeToWidgetName("divider");
    rp_numerator->setAlign(RelativeLayoutParameter::RelativeAlign::LOCATION_ABOVE_CENTER);
    rp_numerator->setMargin(Margin(0, 0, 0, 0));
    _numeratorText->setLayoutParameter(rp_numerator);
    
    measureAndReSize();
    
    return true;
}

void MathNumber::setFontSize(float fontSize)
{
    _fontSize = fontSize;
    _integerText->setFontSize(fontSize);
    _denominatorText->setFontSize(fontSize);
    _numeratorText->setFontSize(fontSize);
}

void MathNumber::setFontColor(const cocos2d::Color4B &fontColor)
{
    _fontColor = fontColor;
    _integerText->setTextColor(fontColor);
    _denominatorText->setTextColor(fontColor);
    _numeratorText->setTextColor(fontColor);
    _dividerBar->setBackGroundColor(Color3B(fontColor.r, fontColor.g, fontColor.b));
}

void MathNumber::setString(const std::string& pattern, bool isAdaptive)
{
    // pattern as "a" "c/b" or "a+c/b"
    
    if (pattern.empty()) {
        _integer = "";
        _denominator = "";
        _numerator = "";
        _integerText->setString("");
        _denominatorText->setString("");
        _numeratorText->setString("");
        _showDefaultDivider = false;
        measureAndReSize();
        return;
    }
    
    std::size_t start = 0;
    std::size_t end = pattern.length() - 1;
    
    std::size_t findPlus = pattern.find("+");
    std::size_t findSlash = pattern.find("/");
    
    // 整數
    if (std::string::npos == findSlash) {
        _integerText->setString(pattern);
        _denominatorText->setString("");
        _numeratorText->setString("");
        _integer = pattern;
        _denominator = "";
        _numerator = "";
        
        _inputType = 0;// 修改時，默認輸入整數部分
        _showDefaultDivider = false;
    } else {// 分數
        std::string integerString = "";
        std::string numeratorString = "";
        std::string denominatorString = "";
        
        if (std::string::npos == findPlus) {
            numeratorString = pattern.substr(start, findSlash - start);
            denominatorString = pattern.substr(findSlash + 1, end - findSlash);
        } else {
            integerString = pattern.substr(start, findPlus - start);
            numeratorString = pattern.substr(findPlus + 1, findSlash - findPlus - 1);
            denominatorString = pattern.substr(findSlash + 1, end - findSlash);
        }
        
        _integerText->setString(integerString);
        _numeratorText->setString(numeratorString);
        _denominatorText->setString(denominatorString);
        
        _integer = integerString;
        _numerator = numeratorString;
        _denominator = denominatorString;
        
        _inputType = 2;// 修改時，默認輸入分子部分
        _showDefaultDivider = true;
    }
    
    measureAndReSize();
    
    if (isAdaptive) {
        setContentSize(_container->getContentSize());
    }
}

std::string MathNumber::getString()
{
    std::string data;
    
    std::string integer = _integerText->getString();
    std::string denominator = _denominatorText->getString();
    std::string numerator = _numeratorText->getString();
    
//    NumberType type = getNumberType();
    
    int type = -1;
    
    if (0 < integer.length()) {
        if (0 < denominator.length() && 0 < numerator.length()) {
            type = 3;// 帶分數
        }
        
        if (0 == denominator.length() && 0 == numerator.length()) {
            type = 0;// 只有整數部分
        }
    }
    
    if (0 == integer.length()) {
        if (0 < denominator.length() && 0 < numerator.length()) {
            type = 2;// 真分數或假分數
        }
    }
    
    switch (type) {
        case 0:
            data = integer;
            break;
        case 1:
        case 2:
            data = StringUtils::format("%s/%s", numerator.c_str(), denominator.c_str());
            break;
        case 3:
            data = StringUtils::format("%s+%s/%s", integer.c_str(), numerator.c_str(), denominator.c_str());
            break;
            
        default:
            data = "";
            break;
    }
    
    return data;
}

void MathNumber::putString(const std::string& str)
{
    // 不允許修改分母
    if ("?" == _denominatorText->getString()) {
        _inputType = 2;
    }
    
    // 不允許修改分子
    if ("?" == _numeratorText->getString()) {
        _inputType = 1;
    }
    
    // 分數不能輸入忽略文字-arvin edit
    if (_inputType == 1 || _inputType == 2) {
        if(str == _ignoreWord) {
            return;
        }
    }
    
    Text* currentText = nullptr;
    switch (_inputType) {
        case 1:
        currentText = _denominatorText;
        break;
        case 2:
        currentText = _numeratorText;
        break;
        default:
        currentText = _integerText;
    }
    
    log("MathNumber::putString str = %s, inputType = %d", str.c_str(), _inputType);
    
    // 避免輸入數據過長
    if ("bc" != str && "del" != str && _container->getContentSize().width > getContentSize().width) {
        if (2 == _inputType) {// 當分數寬度達到最大長度，分子尚未輸入時，應允許分子輸入
            if (_numeratorText->getContentSize().width < _denominatorText->getContentSize().width) {
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    
    // 存在應忽略的字符，無法變成真分數-arvin edit
    if ("a" == str || "bc" == str) {
        std::string leftString = getLeftString();
        if (!_ignoreWord.empty() && leftString.find(_ignoreWord) != -1) {
            return;
        }
    }
    // end
    
    if ("a" == str) {// 整數部分輸入結束，開始輸入分母
        if (0 == _inputType && !_integerText->getString().empty()) {
            _inputType = 1;
            _showDefaultDivider = true;
        }
    } else if ("bc" == str) {// 分母部分輸入結束，開始輸入分子
        if (1 == _inputType && !_denominatorText->getString().empty()) {
            _inputType = 2;
        }
        
        if (0 == _inputType && !_integerText->getString().empty()) {
            _inputType = 2;
            _denominatorText->setString(_integerText->getString());
            _integerText->setString("");
            _showDefaultDivider = true;
        }
        
    } else if("del" == str) {// 刪除分數，先刪分子，然後分母，最後整數部分
        std::string content = currentText->getString();
        std::string content1 = _denominatorText->getString();
        std::string content2 = _integerText->getString();
        if (0 < content.length()) {
            currentText->setString(content.substr(0, content.length()-1));
        } else {
            if (2 == _inputType) {
                if (0 < content1.length() && content1 != "?") {
                    _denominatorText->setString(content1.substr(0, content1.length()-1));
                }
                _inputType = 1;
            } else if (1 == _inputType) {
                if (0 == content1.length() && _showDefaultDivider) {
                    _showDefaultDivider = false;
                    _inputType = 0;
                }
            }
        }
    } else if("ok" == str) {// 分數輸入完成
        
        std::string integerString = _integerText->getString();
        std::string denominatorString = _denominatorText->getString();
        std::string numeratorString = _numeratorText->getString();
        
        // 判斷分數輸入是否合法
        bool flag1 = false, flag2 = false, flag3 = false;
        
        if (!integerString.empty()) {
            _integer = _integerText->getString();
            flag1 = true;
        }
        
        if (!denominatorString.empty()) {
            _denominator = _denominatorText->getString();
            flag2 = true;
        }
        
        if (!numeratorString.empty()) {
            _numerator = _numeratorText->getString();
            flag3 = true;
        }
        
        if (flag1 && flag2 && flag3) {// 輸入成功
            ;
        }
        
    } else {
        currentText->setString(currentText->getString() + str);
    }
    
    measureAndReSize();
}

std::string MathNumber::getLeftString()
{
    return _integerText->getString();
}

void MathNumber::setLeftString(const std::string &str)
{
    _integerText->setString(str);
}

void MathNumber::setIgnoreWord(const std::string &word)
{
    this->_ignoreWord = word;
}

void MathNumber::showBorder()
{
    _drawNode->clear();
    _drawNode->drawRect(Vec2::ZERO, Vec2(getContentSize().width, getContentSize().height), Color4F::BLACK);
}

void MathNumber::hideBorder()
{
    _drawNode->clear();
}

/**
 * 此方法有BUG
 */
MathNumber::NumberType MathNumber::getNumberType()
{
    _type = NumberType::Other;
    if (0 < Value(_integer).asFloat()) {
        if (0 < Value(_denominator).asInt() && 0 < Value(_numerator).asInt()) {// 帶分數
            _type = NumberType::FractionMixed;
        } else if (0 == Value(_denominator).asInt() && 0 == Value(_numerator).asInt()){// 整數
            _type = Integer;
        }
    } else {
        if (_numerator < _denominator) {// 真分數
            _type = NumberType::FractionProper;
        } else if (_numerator > _denominator) {// 假分數
            _type = NumberType::FractionImproper;
        }
    }
    
    return _type;
}

int MathNumber::getInteger()
{
    std::string str = _integerText->getString();
    if (str.empty()) {
        _integer = "";
    } else {
        _integer = str;
    }
    
    return Value(_integer).asInt();
}

int MathNumber::getDenominator()
{
    std::string str = _denominatorText->getString();
    if (str.empty()) {
        _denominator = "";
    } else if ("?" == str) {
        _denominator = "-1";
    } else {
        _denominator = str;
    }
    return Value(_denominator).asInt();
}

int MathNumber::getNumerator()
{
    std::string str = _numeratorText->getString();
    if (str.empty()) {
        _numerator = "";
    } else if ("?" == str) {
        _numerator = "-1";
    } else {
        _numerator = str;
    }
    return Value(_numerator).asInt();
}

bool MathNumber::isEmpty()
{
    return "" == _integer && "" == _denominator && "" == _numerator;
}

bool MathNumber::isFractionLegal()
{
    getNumberType();
    
    return _type != Integer && _type != Other;
}

void MathNumber::measureAndReSize()
{
    float dividerWidth = _numeratorText->getContentSize().width > _denominatorText->getContentSize().width ? _numeratorText->getContentSize().width : _denominatorText->getContentSize().width;
    
    // 重設分數分割線大小
    if (0 < dividerWidth) {
        _dividerBar->setContentSize(Size(dividerWidth + _dividerWidthExt, _dividerHeight));
    } else if(_showDefaultDivider) {// 輸入分數整數部分完成后，要顯示默認分割線
        _dividerBar->setContentSize(Size(16, _dividerHeight));
    } else {
        _dividerBar->setContentSize(Size::ZERO);
    }
    
    float width = _integerText->getContentSize().width + _dividerBar->getContentSize().width;
    float height = _numeratorText->getContentSize().height + _dividerBar->getContentSize().height + _denominatorText->getContentSize().height;
    
    log("MathNumber::measureAndReSize width = %.2f, height = %.2f", width, height);
    
    // 重設分數區域大小
    _container->setContentSize(Size(width, height));
}

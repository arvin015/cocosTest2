//
//  ActionTest.cpp
//  CocosTest2
//
//  Created by arvin on 2017/8/22.
//
//

#include "ActionTest.h"
#include "ui/UIButton.h"
#include "ui/UIText.h"

USING_NS_CC;
using namespace ui;
using namespace std;

const string names[] = {"Sequence", "Repeat", "Spawn", "ActionEase", "Bezier", "Animation", "CardinalSpline", "Progress"};

inline int getMRow(int index, int col) {
    return index / col;
}

inline int getMCol(int index, int col) {
    return index % col;
}

ActionTest::ActionTest()
: testSprite(nullptr) {

}

ActionTest::~ActionTest() {

}

void ActionTest::onEnter() {
    Layer::onEnter();
}

void ActionTest::onExit() {

    testSprite->stopAllActions();
    
    Text* introText = static_cast<Text*>(getChildByTag(400));
    if(introText) {
        introText->stopAllActions();
    }
    
    //清理缓存
    AnimationCache::getInstance()->destroyInstance(); //先清理动画缓存
    SpriteFrameCache::getInstance()->removeUnusedSpriteFrames(); //然后清理精灵帧缓存
    Director::getInstance()->getTextureCache()->removeUnusedTextures(); //最后是纹理缓存
    
    Layer::onExit();
}

bool ActionTest::init() {
    if(!BaseLayer::init()) {
        return false;
    }
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("grossini_dance.plist", "grossini_dance.png");
    
    float paddingLeft = 20;
    float paddingTop = 20;
    float width = 125;
    float height = 54;
    int column = 6;
    float startX = (V_WIDTH - column * width - (column - 1) * paddingLeft) / 2;
    float startY = V_HEIGHT - 50;
    
    for(int i = 0; i < 8; i++) {
        Button* btn = Button::create("mian_button_01_125x54.png");
        btn->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        btn->setContentSize(Size(125, 54));
        btn->setPosition(Vec2(startX + (getMCol(i, column) * (width + paddingLeft)),
                              startY - (getMRow(i, column) * (height + paddingTop))));
        btn->setTitleFontSize(20);
        btn->setTitleText(names[i]);
        btn->addClickEventListener(CC_CALLBACK_1(ActionTest::onBtnClick, this));
        btn->setTag(i);
        addChild(btn);
    }
    
    testSprite = Sprite::create("elephant1_Diffuse.png");
    testSprite->setPosition(Vec2(300, V_HEIGHT / 2));
    addChild(testSprite, 1);
    
    return true;
}

void ActionTest::onBtnClick(Ref* pSender) {
    
    Button* btn = static_cast<Button*>(pSender);
    if(btn) {
        
        testSprite->stopAllActions();
        testSprite->setPosition(Vec2(300, V_HEIGHT / 2));
        testSprite->setScale(1);
        testSprite->setOpacity(255);
        testSprite->setRotation(0);
        testSprite->setVisible(true);
        
        Text* introText = static_cast<Text*>(getChildByTag(400));
        if(introText) {
            introText->stopAllActions();
            introText->removeFromParent();
        }
        
        DrawNode* drawNode = static_cast<DrawNode*>(getChildByTag(600));
        if(drawNode) {
            drawNode->removeFromParent();
        }
        
        Sprite* sprite = static_cast<Sprite*>(getChildByTag(101));
        if(sprite) {
            sprite->removeFromParent();
        }
        
        ProgressTimer* test = static_cast<ProgressTimer*>(getChildByTag(700));
        if(test) {
            test->removeFromParent();
        }
        
        int tag = btn->getTag();
        
        if(tag != 5) {
            testSprite->setTexture("elephant1_Diffuse.png");
        }
        
        switch (tag) {
            case 0: {
                
                Sprite* sprite = Sprite::createWithSpriteFrameName("grossini_dance_10.png");
                sprite->setPosition(Vec2(200, 200));
                sprite->setTag(101);
                this->addChild(sprite);
                
                auto jump = JumpBy::create(1.0f, Vec2(100, 0), 100, 1);
                
                auto targetAction = TargetedAction::create(sprite, jump); //指定目标，指定动作
                
                auto action = Sequence::create(jump, targetAction, Blink::create(0.5f, 3), jump->reverse(), CallFunc::create([sprite](){
                    sprite->removeFromParent();
                }), NULL);
                testSprite->runAction(action);
                break;
            }
            case 1: {
                auto jump = JumpBy::create(1.0f, Vec2(100, 0), 100, 1);
                auto action = RepeatForever::create(Sequence::create(jump, DelayTime::create(0.5f), Blink::create(0.5f, 2), jump->reverse(), DelayTime::create(0.5f), NULL));
                testSprite->runAction(action);
                break;
            }
            case 2: {
                testSprite->setOpacity(0);
                testSprite->setScale(0);
                auto action = Spawn::create(RotateBy::create(0.5f, 360), FadeIn::create(0.5f), ScaleTo::create(0.5f, 1), NULL);
                testSprite->runAction(action);
                break;
            }
            case 3: {
                
                Text* introText = Text::create();
                introText->setPosition(Vec2(300, V_HEIGHT - 250));
                introText->setString("回震缓冲");
                introText->setFontSize(26);
                introText->setTextColor(Color4B::RED);
                introText->setOpacity(0);
                introText->setTag(400);
                addChild(introText);
                
                introText->runAction(Sequence::create(RotateBy::create(0.5f, 360), FadeIn::create(0.5f), ScaleTo::create(0.5f, 1), NULL));
                
                //In 刚开始时显示缓冲效果；Out 结束时显示缓冲效果；InOut 开始结束都显示缓冲效果
                auto action = Sequence::create(EaseBounceIn::create(MoveBy::create(2.0f, Vec2(0, -200))), DelayTime::create(1.0f),  EaseBounceOut::create(MoveBy::create(2.0f, Vec2(0, 200))), CallFunc::create([introText](){
                    introText->setOpacity(0);
                    introText->setString("弹性缓冲");
                    introText->runAction(Sequence::create(RotateBy::create(0.5f, 360), FadeIn::create(0.5f), ScaleTo::create(0.5f, 1), NULL));
                }), NULL); //回震缓冲
                auto action1 = Sequence::create(EaseElasticIn::create(MoveBy::create(2.0f, Vec2(0, -200))), DelayTime::create(1.0f),EaseElasticOut::create(MoveBy::create(2.0f, Vec2(0, 200))), CallFunc::create([introText](){
                    introText->setOpacity(0);
                    introText->setString("跳跃缓冲");
                    introText->runAction(Sequence::create(RotateBy::create(0.5f, 360), FadeIn::create(0.5f), ScaleTo::create(0.5f, 1), NULL));
                }),NULL); //弹性缓冲
                auto action2 = Sequence::create(EaseBackIn::create(MoveBy::create(2.0f, Vec2(0, 200))), DelayTime::create(1.0f), EaseBackOut::create(MoveBy::create(2.0f, Vec2(0, -200))), CallFunc::create([introText](){
                    introText->setOpacity(0);
                    introText->setString("Sine缓冲");
                    introText->runAction(Sequence::create(RotateBy::create(0.5f, 360), FadeIn::create(0.5f), ScaleTo::create(0.5f, 1), NULL));
                }), NULL); //跳跃缓冲
                auto action3 = Sequence::create(EaseSineIn::create(MoveBy::create(2.0f, Vec2(0, -200))), DelayTime::create(1.0f), EaseSineOut::create(MoveBy::create(2.0f, Vec2(0, 200))), NULL); //Sine缓冲
                
                testSprite->runAction(Sequence::create(action, DelayTime::create(2.0f), action1, DelayTime::create(2.0f), action2, DelayTime::create(2.0f), action3, CallFunc::create([introText](){
                    introText->removeFromParent();
                }), NULL));
                
                break;
            }
            case 4: {
                ccBezierConfig config;
                config.endPosition = Vec2(200, -100);
                config.controlPoint_1 = Vec2(300, testSprite->getPositionY());
                config.controlPoint_2 = Vec2(100, -100);
                auto action = BezierBy::create(2.0f, config);
                testSprite->runAction(action);
                break;
            }
            case 5: {
                
                testSprite->setSpriteFrame("grossini_dance_01.png");
                
                Animation* animation = Animation::create();
                for(int i = 0; i < 14; i++) {
                    char fileName[256] = {0};
                    if(i + 1 > 9) {
                        sprintf(fileName, "grossini_dance_%d.png", (i + 1));
                    } else {
                        sprintf(fileName, "grossini_dance_0%d.png", (i + 1));
                    }
                    
                    SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(fileName);
                    
                    animation->addSpriteFrame(spriteFrame);
                }
                animation->setDelayPerUnit(3.0f / 10.0f);
                animation->setRestoreOriginalFrame(true);
                
                Animate* animate = Animate::create(animation);
                testSprite->runAction(animate);
                
                break;
            }
            case 6: {
                
                PointArray* points = PointArray::create(20);
                points->addControlPoint(Vec2(testSprite->getPositionX(), testSprite->getPositionY()));
                points->addControlPoint(Vec2(testSprite->getPositionX(), testSprite->getPositionY() + 200));
                points->addControlPoint(Vec2(testSprite->getPositionX() + 200, testSprite->getPositionY() + 200));
                points->addControlPoint(Vec2(testSprite->getPositionX() + 200, testSprite->getPositionY()));
                points->addControlPoint(Vec2(testSprite->getPositionX(), testSprite->getPositionY()));
                
                auto action = RepeatForever::create(CardinalSplineTo::create(3.0f, points, 0));
                testSprite->runAction(action);
                
                DrawNode* drawNode = DrawNode::create();
                drawNode->drawCardinalSpline(points, 0, 100, Color4F::RED); //100表示顶点个数
                drawNode->setTag(600);
                this->addChild(drawNode, 0);
                
                break;
            }
            case 7: {
                
                testSprite->setVisible(false);
                
                auto test = ProgressTimer::create(Sprite::create("elephant1_Diffuse.png"));
                test->setPosition(testSprite->getPosition());
                test->setType(ProgressTimer::Type::RADIAL);
                test->setTag(700);
                this->addChild(test);
                
                auto action = ProgressFromTo::create(2.0f, 0, 100);
                test->runAction(RepeatForever::create(Sequence::create(action, DelayTime::create(0.5f), action->reverse(), DelayTime::create(0.5f), NULL)));
                
                break;
            }
                
            default:
                break;
        }
    }
}

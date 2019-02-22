//
//  VolumeQuestion.h
//  CocosTest2
//
//  Created by mac_arvin on 2019/2/18.
//

#ifndef VolumeQuestion_h
#define VolumeQuestion_h

#include "cocos2d.h"

class VertexInfo {
    
public:
    
    VertexInfo() {}

    VertexInfo(int vId, const cocos2d::Vec2 &p2d) {
        this->vId = vId;
        this->p2d = p2d;
    }
    
    VertexInfo(int vId, const cocos2d::Vec3 &p3d) {
        this->vId = vId;
        this->p3d = p3d;
    }
    
    virtual ~VertexInfo() {};
    
    int vId;
    cocos2d::Vec2 p2d;
    cocos2d::Vec3 p3d;
};

class CutInfo { //处理一些特殊分割情况，包括可切、不可切

public:

    CutInfo() {}
    CutInfo(int vId1, int vId2, bool canCut, int direction) {
        this->vId1 = vId1;
        this->vId2 = vId2;
        this->canCut = canCut;
        this->direction = direction;
    }
    ~CutInfo() {}

    bool isExit(int id1, int id2) {
        return (id1 == vId1 && id2 == vId2) || (id1 == vId2 && id2 == vId1);
    }

    int vId1, vId2;
    bool canCut;
    int direction = 0; //0：错误；1：左右；2：上下
};

class FillInfo {

public:

    FillInfo() {}
    FillInfo(const std::map<int, int> &fillVecMap, const std::vector<cocos2d::Vec2> &fillVecs, float minLen, float maxLen) {
        this->fillVecMap = fillVecMap;
        this->fillVecs = fillVecs;
        this->minLen = minLen;
        this->maxLen = maxLen;
    }
    ~FillInfo() {}

    bool isExit(int vId) {
        return  fillVecMap.find(vId) != fillVecMap.end();
    }

    int getFriendId(int vId) {
        if (isExit(vId)) {
            return fillVecMap[vId];
        }
        return -1;
    }

    std::map<int, int> fillVecMap; //可填补点及其对应的参照线
    std::vector<cocos2d::Vec2> fillVecs; //填充区域顶点集
    bool isFilled = false; //是否已经被填补了
    float minLen = 0; //最短满足的画线
    float maxLen = 0; //最大满足的画线
};

class VolumeQuestion {

public:

    VolumeQuestion() {}
    VolumeQuestion(int qId, const std::vector<VertexInfo> vertexList, const std::vector<FillInfo*> fillInfoList, std::vector<CutInfo> cutInfoList) {
        this->qId = qId;
        this->vertexList = vertexList;
        this->fillInfoList = fillInfoList;
        this->cutInfoList = cutInfoList;
    }
    ~VolumeQuestion() {}

    int qId;
    std::vector<VertexInfo> vertexList;
    std::vector<FillInfo*> fillInfoList;
    std::vector<CutInfo> cutInfoList;

    bool getVertexById(int vId, VertexInfo &vertexInfo) {
        for (VertexInfo vertexInfo1 : vertexList) {
            if (vertexInfo1.vId == vId) {
                vertexInfo = vertexInfo1;
                return true;
            }
        }
        return false;
    }

    FillInfo* getFillInfo(int vId) {
        for (FillInfo* f : fillInfoList) {
            if (f->isExit(vId)) {
                return f;
            }
        }
        return nullptr;
    }

    bool getCutInfo(int vId1, int vId2, CutInfo &cutInfo) {
        for (CutInfo c : cutInfoList) {
            if (c.isExit(vId1, vId2)) {
                cutInfo = c;
                return true;
            }
        }
        return false;
    }

    void reset() {
        for (FillInfo* fillInfo : fillInfoList) {
            fillInfo->isFilled = false;
        }
    }
};

#endif /* VolumeQuestion_h */

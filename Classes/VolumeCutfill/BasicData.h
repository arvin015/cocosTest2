//
//  BasicData.h
//  CocosTest2
//
//  Created by mac_arvin on 2019/2/18.
//

#ifndef BasicData_h
#define BasicData_h

#include "cocos2d.h"

//namespace VolumeCutfill {

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

    class BasicData {
        
    };
//}

#endif /* BasicData_h */

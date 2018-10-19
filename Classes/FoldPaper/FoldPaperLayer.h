//
//  FoldPaperLayer.h
//  CocosTest2-mobile
//
//  Created by mac_arvin on 2018/10/18.
//

#ifndef FoldPaperLayer_h
#define FoldPaperLayer_h

#include <stdio.h>
#include "BaseLayer.h"

class PolygonView;

namespace FoldPaper {
    
    class FoldPaperLayer : public BaseLayer {
        
    public:
        
        FoldPaperLayer();
        ~FoldPaperLayer();
        
        virtual void onEnter();
        virtual void onExit();
        
        virtual bool init();
        
        CREATE_FUNC(FoldPaperLayer);

    private:
//        cocos2d::Vector<PolygonView*> polygonViewList;
    };
}

#endif /* FoldPaperLayer_h */

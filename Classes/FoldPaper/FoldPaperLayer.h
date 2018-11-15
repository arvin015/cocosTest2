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

namespace FoldPaper {

    enum PaperState {
        PAPER_MAKING,
        PAPER_FOLDING
    };

    class FoldPaperMakeLayer;
    class FoldPaperFoldLayer;

    class FoldPaperLayer : public BaseLayer {
        
    public:
        
        FoldPaperLayer();
        ~FoldPaperLayer();
        
        virtual void onEnter();
        virtual void onExit();
        
        virtual bool init();
        
        CREATE_FUNC(FoldPaperLayer);

        virtual bool shouldWhiteBg() {return false;};

        /**
         * 选择物体
         * @param shapeIndex
         */
        void selectShape(int shapeIndex);

        /**
         * 重置
         */
        void reset();

    private:
        cocos2d::ui::CheckBox* foldBtn;
        cocos2d::ui::Button* delBtn;

        PaperState paperState;
        FoldPaperMakeLayer* makeLayer;
        FoldPaperFoldLayer* foldLayer;

        int shapeIndex;
    };
}

#endif /* FoldPaperLayer_h */

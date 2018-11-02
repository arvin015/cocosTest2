#ifndef CMULTITOUCH_H
#define CMULTITOUCH_H

#include "cocos2d.h"
USING_NS_CC;

const int kTouchNone = -1;

struct sTouchState{
    int tid, x_, y_, x, y, movecnt;
	bool isTap;
    inline void init(int touchid, const Vec2 &p){
        tid = touchid;
        x = x_ = p.x;
        y = y_ = p.y;
		movecnt = 0;
		isTap = true;
    }
    inline void update(const Vec2 &p){
		movecnt++;
		x = p.x;
        y = p.y;
		if (isTap) {
			float dx = x_ - x;
			float dy = y_ - y;
			float distsq = (dx * dx) + (dy * dy);
			if ((movecnt > 6)||(distsq > 100)){
				isTap = false;
			}
		}
    }
};

struct sMultiTouchState{
    int cx, cy;
    float rad, lensqrt;
    
    inline void init(int x1, int y1, int x2, int y2){
        cx = (x1 + x2)/2;
        cy = (y1 + y2)/2;
        int dx = x1 - x2;
        int dy = y1 - y2;
		lensqrt = sqrtf((dx * dx) + (dy * dy));
        rad = atan2f(dx, -dy);
    }
	inline void copy(const sMultiTouchState& src) {
		cx = src.cx;
		cy = src.cy;
		rad = src.rad;
		lensqrt = src.lensqrt;
	}
};

class cMultiTouch {
public:
    bool active, wasactive;
    sTouchState t1, t2;
    sMultiTouchState mtStart, mtLast, mt;
public:
    cMultiTouch(){reset();}
    
    void reset(){
		wasactive = false;
        active = false;
        t1.tid = t2.tid = kTouchNone;
    }
    
    bool OnTouchDown(int touchid, const Vec2 &p);
	bool OnTouchMove(int touchid, const Vec2 &p);
	bool OnTouchUp(int touchid, const Vec2 &p);
	bool OnTouchCancel(int touchid, const Vec2 &p);

protected:
	bool onmoveorup(int touchid, const Vec2 &p, bool isup);
	bool updatetouchstate(sTouchState &T, const Vec2 &p, bool isup);
};


#endif 

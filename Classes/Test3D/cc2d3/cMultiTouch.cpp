#include "cMultiTouch.h"

bool cMultiTouch::OnTouchDown(int touchid, const Vec2 &p) {
	if (active)
		return false;
	wasactive = active;

	if (t1.tid == kTouchNone) {
		t1.init(touchid, p);
		active = (t2.tid != kTouchNone);
	} else {
		t2.init(touchid, p);
		active = true;
	}
	if (active) {
		mt.init(t1.x, t1.y, t2.x, t2.y);
		mtLast.copy(mt);
		mtStart.copy(mt);
	}
	return active;
}

bool cMultiTouch::OnTouchMove(int touchid, const Vec2 &p) {
	return onmoveorup(touchid, p, false);
}

bool cMultiTouch::OnTouchUp(int touchid, const Vec2 &p) {
	return onmoveorup(touchid, p, true);
}

bool cMultiTouch::OnTouchCancel(int touchid, const Vec2 &p) {
	return onmoveorup(touchid, p, true);
}

bool cMultiTouch::onmoveorup(int touchid, const Vec2 &p, bool isup) {
	if (touchid == t1.tid) {
		return updatetouchstate(t1, p, isup);
	} else if (touchid == t2.tid) {
		return updatetouchstate(t2, p, isup);
	}
	return false;
}

bool cMultiTouch::updatetouchstate(sTouchState &T, const Vec2 &p, bool isup) {
	wasactive = active;
	T.update(p);
	if (isup) {
		T.tid = kTouchNone;
		active = false;
	}

	if (wasactive)
		mtLast.copy(mt);
	if (active)
		mt.init(t1.x, t1.y, t2.x, t2.y);

	return wasactive;
}


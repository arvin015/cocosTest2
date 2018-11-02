#ifndef CG3DEFMODELGENDONUTH
#define CG3DEFMODELGENDONUTH

#include "CC3DMath.h"
#include "cG3DefModelGen.h"

class cG3DefModelGenDonut: public cG3DefModelGen {
	typedef cG3DefModelGen base;
protected:
    int sidecnt2;
    float radius2, startangle2;
    
public:
    cG3DefModelGenDonut(const char *name, int _sidecnt, int _sidecnt2,
                        float _radius, float _radius2, float _startangle, float _startangle2);
	//virtual void draw();
};

#endif

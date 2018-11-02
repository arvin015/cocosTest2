#ifndef CG3DEFMODELGENSPHEREH
#define CG3DEFMODELGENSPHEREH

#include "CC3DMath.h"
#include "cG3DefModelGen.h"

class cG3DefModelGenSphere: public cG3DefModelGen {
	typedef cG3DefModelGen base;
protected:
    int sidecntz;
    
public:
    cG3DefModelGenSphere(const char *name, int _sidecnt, int _sidecntz,
                         float _radius, float _startangle);
};

#endif
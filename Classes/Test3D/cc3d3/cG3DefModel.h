/*
 *  cGraphcoco.h
 *  mycoco1
 *
 *  Created by Lam Hon Bon on 1/14/11.
 *  Copyright 2011 programmer. All rights reserved.
 *
 */
#ifndef CGRAHP3DMODEL_H
#define CGRAHP3DMODEL_H

#include "cocos2d.h"
USING_NS_CC;

#include "CC3DMath.h"

struct md5_vertex_t{
	Vec2 st;
	int start, count; /* start weight, weight count */
    int posdup; //the dupicated idx
    md5_vertex_t(){
        start = count = posdup = -1;
    }
};

struct md5_triangle_t{
	int index[3];
};

class cG3DefModel: public Ref{
	typedef Ref base;
public:
	std::vector<Vec3> nors;
	std::vector<Vec3> vers;
	std::vector<Vec2> uvs;
	std::vector<ushort> idxs;
    
    int num_verts, num_idx;
public:
	cG3DefModel(const char *name):base(){
        num_verts = num_idx = 0;
    };

    virtual void virtualdelete(){
		//CC_SAFE_DELETE_ARRAY(nors);
        //CC_SAFE_DELETE_ARRAY(nors);
		//CC_SAFE_DELETE_ARRAY(vers);
		//CC_SAFE_DELETE_ARRAY(uvs);
		//CC_SAFE_DELETE_ARRAY(idxs);
        //base::virtualdelete();
    }

	virtual void draw();
}; // shared define of model

#endif 

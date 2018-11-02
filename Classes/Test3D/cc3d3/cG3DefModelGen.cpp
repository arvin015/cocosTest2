//#include "CC2D3Util.h"
#include "cG3DefModelGen.h"
#include "LogUtil.h"

void cG3DefModelGen::print(){
    for(int i = 0; i < num_verts; i++){
        LOGUTIL(LEVEL_INFO, tagtag::TAG(),"%d (%0.2f, %0.2f, %0.2f), (%0.2f, %0.2f, %0.2f), (%f, %f)\n", i,
               vers[i].x, vers[i].y, vers[i].z,
               nors[i].x, nors[i].y, nors[i].z,
			   uvs[i].x, uvs[i].y);
    }
    for(int i = 0; i < num_idx; i+=3){
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"tri: %d, %d, %d\n", idxs[i], idxs[i+1], idxs[i+2]);
    }
}

void cG3DefModelGen::virtualdelete(){
    //NSLog(@"cG3DefModelGen::virtualdelete()");
    corners.clear();
    cornerhards.clear();
    edges.clear();
	trigroupidxs.clear();
    base::virtualdelete();
}

void settriidx(std::vector<ushort> &idxs, int offset, int i1, int i2, int i3){
    idxs[offset + 0] = i1;
    idxs[offset + 1] = i2;
    idxs[offset + 2] = i3;
}

cG3DefModelGen::cG3DefModelGen(const char *name,
                               int _sidecnt, float _goup, float _godown, float _radius,
                               float _scaletop, float _startangle, bool _smoothside):base(name){
    isobj = false;
    isdonut = false;
    issphere = false;

    scaletop = fmaxf(0.0f, _scaletop);
    
    if(0.0f < _scaletop)
        iscylinder = true;
    else
        iscylinder = false;
    iscone = !iscylinder;
    
    if(_sidecnt < 3){
        smoothside = true;
        sidecnt = 6;
    }else{
        smoothside = _smoothside;
        sidecnt = _sidecnt; // number of sides
    }

    //smoothside = true;
    height = _goup - _godown;
    radius = _radius;
    
    startangle = _startangle;
    
    vectorSet(centertop, 0, 0, _goup);
    vectorSet(centerbottom, 0, 0, _godown);
    
    if(!iscone){
        //cylinder
        if(smoothside)
            edges.resize(sidecnt * 2);
        else
            edges.resize(sidecnt * 3);
        allocateCorners(sidecnt * 2);
    }else{
        //cone
        if(smoothside)
            edges.resize(sidecnt);
        else
            edges.resize(sidecnt * 2);
        allocateCorners(sidecnt + 1);
    }
    
	std::vector<float> cornerrad;
	cornerrad.resize(sidecnt);
    
    // === === find bottom corners & edges === ===
    for(int i = 0; i < sidecnt; i++){
        cornerrad[i] = Deg2Rad(startangle + ((360.0f * i) / sidecnt));
        vectorSet(corners[i], cosf(cornerrad[i]) * radius, -sinf(cornerrad[i]) * radius, _godown);

        edges[i].head = i;
        edges[i].tail = (i + 1) % sidecnt;
        edges[i].groupidx = smoothside?0:i;
        edges[i].distsq = -1.0f;
    }
    
    // === === find top corner or corners, and edges=== ===
    if(iscone){
        vectorSet(corners[sidecnt], 0, 0, _goup);
        
        //side edges
        if(!smoothside){
            for(int i = 0; i < sidecnt; i++){
                edges[sidecnt + i].head = i;
                edges[sidecnt + i].tail = sidecnt;
                edges[sidecnt + i].groupidx = sidecnt + i;
            }
        }
        
        for(int i  = 0; i < sidecnt; i++){
            cornerhards[i] = !smoothside;
        }
        cornerhards[sidecnt] = true; // the tip is corner
        
    }else{
        int sidecnt2 = sidecnt * 2;
        for(int i = 0; i < sidecnt; i++){
            vectorSet(corners[sidecnt + i],
                      corners[i].x * scaletop, corners[i].y * scaletop, _goup);

            //top edges
            edges[sidecnt + i].head = sidecnt + i;
            edges[sidecnt + i].tail = sidecnt + ((i + 1) % sidecnt);
            edges[sidecnt + i].groupidx = smoothside?1:(sidecnt + i);

            if(!smoothside){
                //side edges
                edges[sidecnt2 + i].head = i;
                edges[sidecnt2 + i].tail = sidecnt + i;
                edges[sidecnt2 + i].groupidx = sidecnt2 + i;
            }
        }
        
        for(int i  = 0; i < (int)cornerhards.size(); i++){
            cornerhards[i] = !smoothside;
        }
    }
    
    // --- --- --- --- ---
    if(iscone){
        if(smoothside){
            allocateGLobjs((sidecnt + 1) + (sidecnt * 2) + 1, sidecnt * 2);
            genconeface(sidecnt + 1, sidecnt * 3, this);
        }else{
            allocateGLobjs((sidecnt + 1) + (sidecnt * 3), sidecnt * 2);
            for(int i = 0; i < sidecnt; i++){
                gensidetriface((sidecnt + 1) + (i * 3),
                               (sidecnt + i) * 3, i, this);
            }
        }
    }else{
        if(smoothside){
            allocateGLobjs(((sidecnt + 1) * 2) + (sidecnt + 1) * 2, sidecnt * 4);
            genbeltface((sidecnt + 1) * 2, sidecnt * 6, this);
        }else{
            allocateGLobjs(((sidecnt + 1) * 2) + (sidecnt * 4), sidecnt * 4);
            for(int i = 0; i < sidecnt; i++){
                gensidequatface(((sidecnt + 1) * 2) + (i * 4),
                               (sidecnt * 6) + (i * 6), i, this);
            }
        }
        gentopcap(sidecnt + 1, sidecnt * 3, this);
    }
    genbottomcap(0, 0, this);
}



void cG3DefModelGen::allocateGLobjs(int veccnt, int tricnt){
    num_verts = veccnt;
    
    nors.resize(num_verts);
    vers.resize(num_verts);
    uvs.resize(num_verts);

    num_idx = (tricnt * 3);
    idxs.resize(num_idx);
}

void cG3DefModelGen::allocateCorners(int ccnt){
    corners.resize(ccnt);
    cornerhards.resize(ccnt);
}

int cG3DefModelGen::getHardCornerCount(){
    if(isobj){
        int hard = 0;
        for(int i = 0; i < (int)cornerhards.size(); i++){
            if(cornerhards[i])
                hard++;
        }
        return hard;
    }else if(issphere || isdonut){
        return 0;
    }else if(iscone){
        return smoothside?1:(sidecnt + 1);
    }else{
        return smoothside?0:(sidecnt * 2);
    }
}

int cG3DefModelGen::getHardEdgeCount(){
    if(isobj){
        return 0;
    }else if(issphere || isdonut){
        return 0;
    }else if(iscone){
        return smoothside?1:(sidecnt * 2);
    }else{
        return smoothside?2:(sidecnt * 3);
    }
}

int cG3DefModelGen::getHardFaceCount(){
    if(issphere || isdonut)
        return 1;
    if(iscone)
        return smoothside?2:(sidecnt+1);
    else
        return smoothside?3:(sidecnt+2);
}

void cG3DefModelGen::copybaseattributes(cG3DefModelGen *mdlbase){
    smoothside = mdlbase->smoothside;
    iscone = mdlbase->iscone;
    sidecnt = mdlbase->sidecnt;
    radius = mdlbase->radius;
    height = mdlbase->height;
    startangle = mdlbase->startangle;
    vectorCopy(mdlbase->centerbottom, centerbottom);
    vectorCopy(mdlbase->centertop, centertop);
}

void cG3DefModelGen::genconeface(int vidx, int iidx, cG3DefModelGen *mdlbase){
    vectorCopy(mdlbase->corners[0],  vers[vidx]);
    uvs[vidx].x = 1 * TEXTUREFIXSCALE;
    uvs[vidx].y = 1 * TEXTUREFIXSCALE;
    mknormal(vers[vidx], centertop, nors[vidx]);
    
    for(int i = 0; i < sidecnt; i++){
        int vidx2 = vidx + (i * 2) + 1;
        
        vectorCopy(centertop,  vers[vidx2]);
        uvs[vidx2].x = ((((sidecnt - i) * 2) - 1) * TEXTUREFIXSCALE) / sidecnt / 2;
        uvs[vidx2].y = 0 * TEXTUREFIXSCALE;
        
        vectorCopy(mdlbase->corners[(i + 1) % sidecnt],  vers[vidx2+1]);
        uvs[vidx2+1].x = (((sidecnt - i - 1)) * TEXTUREFIXSCALE) / sidecnt;
        uvs[vidx2+1].y = 1 * TEXTUREFIXSCALE;
        
        mknormal(vers[vidx2+1], centertop, nors[vidx2+1]);
        avgnormal(nors[vidx2-1], nors[vidx2+1], nors[vidx2]);

        settriidx(idxs, (iidx + (i * 3)), vidx2-1, vidx2, vidx2+1);
    }
}

void cG3DefModelGen::genbeltface(int vidx_, int iidx_, cG3DefModelGen *mdlbase){
    vectorCopy(mdlbase->corners[0],  vers[vidx_]);
    vectorCopy(mdlbase->corners[sidecnt],  vers[vidx_+1]);
    uvs[vidx_].x = 1 * TEXTUREFIXSCALE;
    uvs[vidx_].y = 1 * TEXTUREFIXSCALE;
    uvs[vidx_+1].x = uvs[0].x;
    uvs[vidx_+1].y = 0 * TEXTUREFIXSCALE;
    mknormal(vers[vidx_], vers[vidx_+1], nors[vidx_]);
    vectorCopy(nors[vidx_], nors[vidx_+1]);
    
    for(int i = 1; i <= sidecnt; i++){
        int cidx = i % sidecnt;
        int vidx = vidx_ + (i * 2);
        int vidx2 = vidx+1;
        
        vectorCopy(mdlbase->corners[cidx],  vers[vidx]);
        vectorCopy(mdlbase->corners[sidecnt + cidx], vers[vidx2]);
        uvs[vidx].x = (sidecnt - i) * TEXTUREFIXSCALE / sidecnt;
		uvs[vidx2].x = uvs[vidx].x;
		uvs[vidx].y = 1 * TEXTUREFIXSCALE;
        uvs[vidx2].y = 0 * TEXTUREFIXSCALE;
        
        mknormal(vers[vidx], vers[vidx2], nors[vidx]);
        vectorCopy(nors[vidx], nors[vidx2]);
        
        int iidx = iidx_ + ((i - 1) * 6);
        settriidx(idxs, (iidx + 0), vidx - 2, vidx - 1, vidx);
        settriidx(idxs, (iidx + 3), vidx, vidx - 1, vidx + 1);
    }
}

void cG3DefModelGen::gensidetriface(int vidx, int iidx, int _sidenum, cG3DefModelGen *mdlbase){
    int croneridx = _sidenum;
    vectorCopy(mdlbase->corners[sidecnt], vers[vidx]);
    vectorCopy(mdlbase->corners[croneridx], vers[vidx+1]);
    vectorCopy(mdlbase->corners[(croneridx + 1) % sidecnt], vers[vidx+2]);
    
    uvs[vidx].x = 1 -(((_sidenum + _sidenum + 1) * TEXTUREFIXSCALE) / sidecnt / 2);
    uvs[vidx].y = 0;
    uvs[vidx+1].x = 1 - (((_sidenum) * TEXTUREFIXSCALE) / sidecnt);
    uvs[vidx+1].y = TEXTUREFIXSCALE;
    uvs[vidx+2].x = 1 - (((_sidenum + 1) * TEXTUREFIXSCALE) / sidecnt);
    uvs[vidx+2].y = TEXTUREFIXSCALE;
    
    mkcrossnormal(vers[vidx+1], vers[vidx+2], vers[vidx], nors[vidx]);
    vectorCopy(nors[vidx], nors[vidx+1]);
    vectorCopy(nors[vidx], nors[vidx+2]);
    
    settriidx(idxs, iidx, vidx, vidx+2, vidx+1);
}


void cG3DefModelGen::gensidequatface(int vidx, int iidx, int _sidenum, cG3DefModelGen *mdlbase){
    int croneridx = _sidenum;
    int croneridx2 = (croneridx + 1) % sidecnt;
    
    vectorCopy(mdlbase->corners[croneridx], vers[vidx]);
    vectorCopy(mdlbase->corners[croneridx + sidecnt], vers[vidx+1]);
    
    vectorCopy(mdlbase->corners[croneridx2], vers[vidx+2]);
    vectorCopy(mdlbase->corners[croneridx2 + sidecnt], vers[vidx+3]);
    
    uvs[vidx].x = 1 - (_sidenum * TEXTUREFIXSCALE / sidecnt);
    uvs[vidx+1].x = uvs[vidx].x;
    uvs[vidx+2].x = 1 - ((_sidenum+1) * TEXTUREFIXSCALE / sidecnt);
    uvs[vidx+3].x = uvs[vidx+2].x;
    uvs[vidx].y = 0;
    uvs[vidx+1].y = TEXTUREFIXSCALE;
    uvs[vidx+2].y = 0;
    uvs[vidx+3].y = TEXTUREFIXSCALE;
    
    mkcrossnormal(vers[vidx], vers[vidx+2], vers[vidx+1], nors[vidx]);
    vectorCopy(nors[vidx], nors[vidx+1]);
    vectorCopy(nors[vidx], nors[vidx+2]);
    vectorCopy(nors[vidx], nors[vidx+3]);
    
    settriidx(idxs, iidx, vidx, vidx+1, vidx+2);
    settriidx(idxs, (iidx+3), vidx+2, vidx+1, vidx+3);
}

void cG3DefModelGen::gentopcap(int vidx, int iidx, cG3DefModelGen *mdlbase){
    int centeridx = vidx + sidecnt;
    vectorCopy(mdlbase->centertop, vers[centeridx]);
    vectorSet(nors[centeridx], 0, 0, 1);
    uvs[centeridx].x = 0.5f * TEXTUREFIXSCALE;
    uvs[centeridx].y = 0.5f * TEXTUREFIXSCALE;
    for(int i = 0; i < sidecnt; i++){
        float cornerrad = Deg2Rad(mdlbase->startangle + ((360.0f * i) / sidecnt));
        
        vectorCopy(mdlbase->corners[i + sidecnt],  vers[vidx + i]);
        vectorSet(nors[vidx + i], 0, 0, 1);
        uvs[vidx + i].x = (0.5f + (cosf(cornerrad)* 0.5f) ) * TEXTUREFIXSCALE;
        uvs[vidx + i].y = (0.5f + (sinf(cornerrad)* 0.5f) ) * TEXTUREFIXSCALE;
        
        settriidx(idxs, iidx + (i*3),
                  vidx + ((i+1) % sidecnt), vidx + i, centeridx);
    }
}

void cG3DefModelGen::genbottomcap(int vidx, int iidx, cG3DefModelGen *mdlbase){
    //bottom cap
    int centeridx = vidx+sidecnt;
    vectorCopy(mdlbase->centerbottom, vers[centeridx]);
    vectorSet(nors[centeridx], 0, 0, -1);
    uvs[centeridx].x = 0.5f * TEXTUREFIXSCALE;
    uvs[centeridx].y = 0.5f * TEXTUREFIXSCALE;
    
    for(int i = 0; i < sidecnt; i++){
        float cornerrad = Deg2Rad(mdlbase->startangle + ((360.0f * i) / sidecnt));
        
        vectorCopy(mdlbase->corners[i],  vers[vidx + i]);
        vectorSet(nors[vidx + i], 0, 0, -1);
        uvs[vidx + i].x = (0.5f + (cosf(cornerrad)* 0.5f) ) * TEXTUREFIXSCALE;
        uvs[vidx + i].y = (0.5f + (sinf(cornerrad)* 0.5f) ) * TEXTUREFIXSCALE;
        
        settriidx(idxs, iidx + (i*3),
                  vidx + i, vidx + ((i+1) % sidecnt), centeridx);
    }
}



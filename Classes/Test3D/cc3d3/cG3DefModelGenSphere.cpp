#include "cG3DefModelGenSphere.h"


cG3DefModelGenSphere::cG3DefModelGenSphere(const char *name, int _sidecnt, int _sidecntz,
                                           float _radius, float _startangle):base(name){
    iscone = false;
    isdonut = false;
    iscylinder = false;
    issphere = true;
    
    smoothside = true;
    sidecnt = _sidecnt;
    sidecntz = _sidecntz;
    height = _radius * 2;
    radius = _radius;
    
    startangle = _startangle;
    
    allocateCorners(2 + (sidecntz * sidecnt)); //2 poles + z rings x s points
    corners.resize(2 + (sidecntz * sidecnt));
    
    //edges = nullptr;//new sModelEdgeIdx[edgecnt];
    
    vectorSet(centerbottom, 0, 0, -radius); //0 = btm-pole
    vectorSet(centertop, 0, 0, radius); //last = top-pole
    
    std::vector<float> cornerrad(sidecnt);
	std::vector<float> fullx(sidecnt), fully(sidecnt); //x & y of all side in full radius
    for(int i = 0; i < sidecnt; i++){
        cornerrad[i] = Deg2Rad(startangle + ((360.0f * i) / sidecnt));
        fullx[i] = cosf(cornerrad[i]) * radius;
        fully[i] = -sinf(cornerrad[i]) * radius;
    }
    
    // find 'corners' ring by ring
    vectorCopy(centerbottom, corners[0]);
    cornerhards[0] = false;
    
    int topidx = (int) corners.size()-1;
    vectorCopy(centertop, corners[topidx]);
    cornerhards[topidx] = false;
    
    int segcnt = sidecntz + 1;
    for(int i = 0; i < sidecntz; i++){
        float zrad = Deg2Rad((180.0f * (i+1)) / segcnt);
        float z = -cosf(zrad) * radius;
        float rate = sinf(zrad);
        
        for(int j = 0; j < sidecnt; j++){
            int cidx = 1+(i * sidecnt)+j;
            vectorSet(corners[cidx], fullx[j] * rate, fully[j] * rate, z);
            cornerhards[cidx] = false;
        }
    }
    
    // ==== ==== ==== ==== base class members ==== ==== ==== ====
    // calcuate and Allocate, needed vecs and idxs
    int dotperring = sidecnt + 1; //the open and close dot overlap
    
    int veccnt = 2 + (sidecntz * dotperring);
    int numtri = sidecnt * sidecntz * 2;
    allocateGLobjs(veccnt, numtri);
    
    // === === === build face vertices === === ===
    //bottom vec
    int vidxbtm = 0; // first vec is the btm vec
    vectorCopy(centerbottom, vers[vidxbtm]);
    vectorSet(nors[vidxbtm], 0, 0, -1);
    uvs[vidxbtm].x = 0.5f * TEXTUREFIXSCALE;
    uvs[vidxbtm].y = 0.0f * TEXTUREFIXSCALE;
    
    
    //find ring all vecs
    for(int j = 0; j < sidecntz; j++){
        for(int i = 0; i < dotperring; i++){
            int idx = 1 + (j * dotperring) + i;
            int corneridx = 1 + (j * sidecnt) + (i % sidecnt);
            vectorCopy(corners[corneridx], vers[idx]);
            vectorCopy(corners[corneridx], nors[idx]);
            normalize(nors[idx]);
            uvs[idx].x = TEXTUREFIXSCALE * i / sidecnt;
            uvs[idx].y = TEXTUREFIXSCALE * (j + 1) / (sidecntz + 1);
        }
    }
    
    //top vec
    int vidxtop = num_verts - 1; // last vec is the top vec
    vectorCopy(centertop, vers[vidxtop]);
    vectorSet(nors[vidxtop], 0, 0, 1);
    uvs[vidxtop].x = 0.5f * TEXTUREFIXSCALE;
    uvs[vidxtop].y = 1.0f * TEXTUREFIXSCALE;
    
    // === === === build face idxs === === ===
    //idxs: btm cap
    for(int i = 0; i < sidecnt; i++){
        settriidx(idxs, (i * 3), vidxbtm, i + 1, i + 2);
    }
    
    //idxs: cylinder sides
    for(int j = 0; j < (sidecntz-1); j++){
        int vring = 1 + j * dotperring;
        for(int i = 0; i < sidecnt; i++){
            int triidx = ((sidecnt) + ((j * sidecnt) * 2) + (i * 2));
            
            int sidei1 = triidx * 3;
            //bottom cap + j rings before + i picec of same ring
            int vright = (i+1);
            settriidx(idxs, sidei1, (vring + i), (vring + i + dotperring), (vring + vright));
            settriidx(idxs, sidei1 + 3, (vring + vright), (vring + i + dotperring), vring + vright + dotperring);
        }
    }
    
    //idxs: top cap
    int toptri1 = ((sidecnt * sidecntz * 2) - sidecnt);
    int topi1 = toptri1 * 3;
    int topv1 = 1 + (dotperring * (sidecntz - 1));
    for(int i = 0; i < sidecnt; i++){
        settriidx(idxs, topi1 + (i * 3), vidxtop, topv1 + i + 1, topv1 + i);
    }
    
    //NSLog(@"num idx=%d  num vec=%d vidxtop=%d", num_idx, num_verts, vidxtop);
}

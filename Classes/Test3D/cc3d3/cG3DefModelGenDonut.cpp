#include "cG3DefModelGenDonut.h"


//static const size_t v3ssize = sizeof(vec3short_t);
//static const size_t vmvsize = sizeof(Vec2);
//static const size_t v3size = sizeof(Vec3);
//void cG3DefModelGenDonut::draw(){
//    base::draw();
//    //glTexCoordPointer (2, GL_SHORT, vmvsize, uvs);
//    //glVertexPointer (3, GL_FLOAT, v3size, vers);
//    //glDrawElements (GL_TRIANGLES, sidecnt * 3 * 3, GL_UNSIGNED_SHORT, idxs);
//    //glDrawElements (GL_TRIANGLES,
//    //                num_idx - (sidecnt * 3),
//    //                GL_UNSIGNED_SHORT, idxs + (sidecnt * 3));
//}


cG3DefModelGenDonut::cG3DefModelGenDonut(const char *name, int _sidecnt, int _sidecnt2,
                                         float _radius, float _radius2,
                                         float _startangle, float _startangle2):base(name){
    iscone = false;
    isdonut = true;
    iscylinder = false;
    issphere = false;
    
    smoothside = true;
    sidecnt = _sidecnt;
    radius = _radius;
    
    sidecnt2 = _sidecnt2;
    radius2 = _radius2;
    height = _radius2 * 2.0f;
    
    startangle = _startangle;
    startangle2 = _startangle2;
    
    // use the big ring to define corner and eged;
    allocateCorners(_sidecnt);
    edges.resize(_sidecnt);
    
    Vec3 basept = {radius, 0, 0};
    std::vector<Quaternion> quats(corners.size());
    for(int i = 0; i < sidecnt; i++){
		quats[i] = Eul2Quat(Vec3(0, 0, Deg2Rad(startangle + ((360.0f * i) / sidecnt))));
        Quat_rotatePoint(quats[i], basept, corners[i]);
        cornerhards[i] = false;
        
        edges[i].head = i;
        edges[i].tail = (i + 1) % sidecnt;
        edges[i].groupidx = i;
        edges[i].distsq = -1.0f;
    }
    
    //point per rings (use corners as ring1 vecs)
    int vpr = sidecnt + 1; // vecs on the big ring, open and close overlap
    int vpr2 = sidecnt2 + 1; // vecs per small ring, open and close overlap
    
    std::vector<Vec3> vec2s(vpr2);
	std::vector<Quaternion> quat2s(vpr2);
	Vec3 basept2(radius2, 0, 0);
    for(int i = 0; i < vpr2; i++){
		quat2s[i] = Eul2Quat(Vec3(0, Deg2Rad(startangle2 + ((360.0f * i) / sidecnt2)), 0));
        Quat_rotatePoint(quat2s[i], basept2, vec2s[i]);
    }
    
    // ==== ==== ==== ==== base class members ==== ==== ==== ====
    // calcuate and Allocate, needed vecs and idxs
    int veccnt = vpr * vpr2;
    int tricnt = sidecnt * sidecnt2 * 2;
    allocateGLobjs(veccnt, tricnt);
    
    for(int i = 0; i < vpr; i++){
        Quaternion &q1 = quats[i % sidecnt];
        Vec3 &v1 = corners[i % sidecnt];
        for(int j = 0; j < vpr2; j++){
            //Quaternion &q2 = quat2s[j];
            Vec3 &v2 = vec2s[j];
            
            int vidx = i * vpr2 + j;
			Vec3 vout;
            Quat_rotatePoint(q1, v2, vout);
            
			vers[vidx] = vout + v1;

			nors[vidx] = vout;
            normalize(nors[vidx]);
            
            uvs[vidx].x = (-TEXTUREFIXSCALE * i) / sidecnt;
            uvs[vidx].y = (-TEXTUREFIXSCALE * j) / sidecnt2;
		
			//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cG3DefModelGenDonut vers[%d] = (%0.3f,%0.3f,%0.3f)", vidx, vers[vidx].x, vers[vidx].y, vers[vidx].z);
		}
    }
    
    for(int i = 0; i < sidecnt; i++){
        for(int j = 0; j < sidecnt2; j++){
            int triidx = 2 * ((i * sidecnt2) + j);
            int iidx = 3 * triidx; //  6 idx (2 tri) per cell
            int vidx = (i * vpr2) + j;
            int vidx2 = vidx + vpr2;
            
            settriidx(idxs, iidx, vidx, vidx+1, vidx2);
            settriidx(idxs, iidx + 3, vidx2, vidx+1, vidx2+1);
        }
    }
    
}



#include "cObjBuffer.h"
#include "cG3DefModelGen.h"
#include "cG3DefModelGenSphere.h"

//#include "CC2D3Util.h"
#include "CC3DHitTest.h"
#include "LogUtil.h"

#include "cStrWalker.h"

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
inline bool v3equal(const Vec3 &a, const Vec3 &b) {
	return a == b;
}

inline bool v2equal(const Vec2 &a, const Vec2 &b) {
	return a == b;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
inline void vmixset(vecmix_t &vm, const Vec3 &v, const Vec3 &n, const Vec2 &u) {
	vectorCopy(v, vm.vec);
	vectorCopy(n, vm.nor);
	vm.uv.x = u.x;
	vm.uv.y = u.y;
}

inline bool vmixequal(const vecmix_t &vm, const Vec3 &v, const Vec3 &n, const Vec2 &u) {
	if (!v3equal(vm.vec, v))
		return false;
	if (!v3equal(vm.nor, n))
		return false;
	return v2equal(vm.uv, u);
}

inline ushort vmixfindoradd(std::vector<vecmix_t> &mixvecs, int mvcount, const Vec3 &v, const Vec3 &n, const Vec2 &u) {
	for (int i = 0; i < mvcount; i++) {
		if (vmixequal(mixvecs[i], v, n, u))
			return i;
	}
	vecmix_t vmix;
	vmixset(vmix, v, n, u);
	mixvecs.push_back(vmix);
	return mvcount;
}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 

int edgefindoradd(ushort shotedv1, ushort shotedv2, std::vector<objedge_t> &edges, ushort num_edge){
    if(shotedv1 > shotedv2)
        return edgefindoradd(shotedv2, shotedv1, edges, num_edge);
    //find...
    //printf("edgefindoradd() v:%d-%d\n", shotedv1, shotedv2);
    for(int i = 0; i < num_edge; i++){
        if(edges[i].equal(shotedv1, shotedv2))
            return i;
    }
    //add if not found...
	objedge_t e;
	e.vidx1 = shotedv1;
	e.vidx2 = shotedv2;
	e.edgegroup = num_edge;
    edges.push_back(e);
    return num_edge;
}

int v3findoradd(std::vector<Vec3> &corners, const Vec3 &v){
    int cnt = (int) corners.size();
    for(int i = 0; i < cnt; i++){
        if(v3equal(v, corners[i])){
			return i;
        }
    }
    corners.push_back(v);
    return cnt;
}
void v3mergesort(int vecnum, std::vector<Vec3> &vecs, std::vector<Vec3> &corners, std::vector<ushort> &vmap){
	corners.reserve(vecnum);
    vmap.resize(vecnum);
    
    for(int i = 0; i < vecnum; i++){
        vmap[i] = v3findoradd(corners, vecs[i]);
		int m = vmap[i];
		Vec3 vs = corners[m];
		//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"vmap[%d] = %d (%f,%f,%f) > (%f,%f,%f)", i, m, vecs[i].x, vecs[i].y, vecs[i].z, vs.x, vs.y, vs.z);
	}
}

float angledot(std::vector<Vec3>&vs, ushort mid, ushort head, ushort tail){
    Vec3 &vhead = vs[head];
	Vec3 &vmid = vs[mid];
	Vec3 &vtail = vs[tail];
    
	Vec3 nor1, nor2;
    vectorSubtract(vhead, vmid, nor1);
    vectorSubtract(vtail, vmid, nor2);
    normalize(nor1);
    normalize(nor2);
    
    return vectorDot(nor1, nor2);
}

void printedgesdump(std::vector<objedge_t> &edges, ushort num_edge){
    printf("num_edge:%d\n", num_edge);
    for(int i = 0; i < num_edge; i++){
        objedge_t &e = edges[i];
        printf("edge %d: vers: %d, %d tris: %d, %d group:%d, hard:%d\n",
               i, e.vidx1, e.vidx2, e.tri1, e.tri2, e.edgegroup, e.hard?1:0);
    }
}

void cObjLoadBuffer::process_rawbuffer(cObjLoadBufferRaw &r){
	clear();

	if(0 >= r.num_tri) //no face and no obj
        return;
    
    //--- find unique corners, UVs, Nors ---
	std::vector<ushort> vecremap(r.vecs.size());
    v3mergesort((int) r.vecs.size(), r.vecs, corners, vecremap);
    LOGUTIL(LEVEL_INFO, tagtag::TAG(),"v3 sorted raw=%d sorted=%d\n", (int)r.vecs.size(), (int)corners.size());
    
    // --- build tri, find egdes and corners ---
    const int num_tri = r.num_tri;
    tris.resize(num_tri);
    edges.reserve(num_tri * 3); //reserve maximum possible

    // --- convert clockwise face into tris fan ---
    int gcnt = 0;
    int tcnt = 0;
    int fcnt = 0;
	int ocnt = (int) r.idx_of1.size();
    for(int o = 0; o < ocnt; o++){
        int of1 = r.idx_of1[o];
        int ofcnt = r.num_of[o];
        //printf("o.%d has %d face, first face is %d\n", io, fcnt, of1);
        for( int f = 0; f < ofcnt; f++){
            fcnt++;
            int fv1 = r.idx_fv1[of1 + f];
            int fvcnt = r.num_fv[of1 + f];
            int ftcnt = fvcnt - 2;
            //printf("o.%d face.%d as %d v, first v= %d\n", io, f, fvcnt, fv1);
            for(int t = 0; t < ftcnt; t++){
                objtri_t &tri = tris[tcnt];
				tri.facegroup = gcnt;
				int vs[3] = { fv1, fv1 + t + 1, fv1 + t + 2 };
				//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"face[%d].Tri[%d] fidx = %d-%d-%d", f, t, vs[0], vs[1], vs[2]);
				// Copy Normals & UV to tri
				//int vi[3] = { r.idx_v[vs[0]], r.idx_v[vs[1]], r.idx_v[vs[2]] };
				//int vm[3] = { vecremap[vi[0]], vecremap[vi[1]], vecremap[vi[2]] };
				//LOGUTIL(LEVEL_INFO, tagtag::TAG()," vi: %d-%d-%d  , vm: %d-%d-%d ", vi[0], vi[1], vi[2], vm[0], vm[1], vm[2] );
				for(int n = 0; n < 3 ; n++){
					tri.vidxs[n] = vecremap[r.idx_v[vs[n]]];
					int uidx = r.idx_u[vs[n]];
					if (-1 == uidx){
						vec2Set(tri.uvs[n], 0, 0);
					}else{
						vec2Copy(r.uvs[uidx], tri.uvs[n]);
					}
					int nidx = r.idx_n[vs[n]];
                    if(-1 == nidx){
                        vectorCopy(tri.facenormal, tri.nors[n]);
                    }else{
                        vectorCopy(r.nors[nidx], tri.nors[n]);
                    }
				}
				tri.e[0] = edgefindoradd(tri.vidxs[0], tri.vidxs[1], edges, edges.size());
				tri.e[1] = edgefindoradd(tri.vidxs[1], tri.vidxs[2], edges, edges.size());
				tri.e[2] = edgefindoradd(tri.vidxs[2], tri.vidxs[0], edges, edges.size());
				//printf("Tri %d Edges = %d, %d, %d \n", tcnt, tri.e[0], tri.e[1], tri.e[2]);
				mkcrossnormal(corners[tri.vidxs[0]], corners[tri.vidxs[1]], corners[tri.vidxs[2]],
                              tri.facenormal);

				tcnt++;
            }
            gcnt++;
        }
    }
    assert(tcnt == num_tri);
	LOGUTIL(LEVEL_INFO, tagtag::TAG(),"%d face break into %d tris of %d groups.\n", fcnt, tcnt, gcnt);
    
	std::vector<ushort> trigroupremap(gcnt);
	for (int i = 0; i < gcnt; i++) {
		trigroupremap[i] = i;
	}

    int mergecnt = 0;
    //--- merge smooth find share groups of tri (init) and mark edge soft or hard ---
    for(int t1 = 0; t1 < num_tri; t1++){ // start loop with tri[1], test will all priv tris
        objtri_t &tri1 = tris[t1];
        for(int t2 = 0; t2 < t1; t2++){ // all tri before the current (t1)
            objtri_t &tri2 = tris[t2];
            float facedot = vectorDot(tri1.facenormal, tri2.facenormal);
            bool sharp = (0.95f > facedot);

            int shareedge = 0;
            for(int e1 = 0; e1 < 3; e1++){
                for(int e2 = 0; e2 < 3; e2++){
                    if(tri1.e[e1] == tri2.e[e2]){
                        //printf("Tri%d-edge%d and Tri%d-egde%d is shared, faceangle=%f\n",
                        //       t1, e1, t2, e2, facedot);
                        shareedge++;
                        objedge_t &edg = edges[tri1.e[e1]];
                        edg.hard = sharp;
						if(!sharp)
							edg.edgegroup = -1;
						edg.tri1 = t1;
                        edg.tri2 = t2;
                    }
                }
            }
            if(sharp)
                continue;
            if(0 >= shareedge)
                continue;
			ushort group1 = trigroupremap[tri1.facegroup];
			ushort group2 = trigroupremap[tri2.facegroup];
			if(group1 == group2)
                continue;
			
			int srcgroup = std::max(group1, group2);
			int destgroup = std::min(group1, group2);
			for (int g = 0; g < gcnt; g++) {
				if (trigroupremap[g] == srcgroup){
					trigroupremap[g] = destgroup;
				}
			}

            mergecnt++;
            //printf("merge %d facegroup %d to %d\n", mergecnt, srcg, destg);
        }
    }

	//=== === pack tri group ids === ===
	int gnum = 0;
	for (int i = 0; i < gcnt; i++) {
		int g = trigroupremap[i];
		if (g == i) {
			//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"group[%d] head get a new id=[%d]", i, gnum);
			trigroupremap[i] = gnum; //
			gnum++;
		}else{
			//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"group[%d] refer group[%d] head for id=[%d]", i, g, trigroupremap[g]);
			trigroupremap[i] = trigroupremap[g]; //
		}
	}
	assert(gnum == (gcnt - mergecnt));
	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"merge facegroup mergecnt=%d ,gnum=%d, gcnt=%d", mergecnt, gnum, gcnt);
    num_facegroup = gnum;

    //--- merge hard edges if smooth ---
    int edgcnt = (int) edges.size();
    int num_edgehard = 0;
    for(int e1 = 0; e1 < edgcnt; e1++){
        objedge_t &edg1 = edges[e1];
        if(!edg1.hard)
            continue;
        num_edgehard++;
        for(int e2 = 0; e2 < e1; e2++){
            objedge_t &edg2 = edges[e2];
            if(!edg2.hard)
                continue;
            if(edg2.edgegroup == edg1.edgegroup)
                continue;
            
            float adot = 1.0f;
            if(edg1.vidx1 == edg2.vidx1){
                adot = angledot(corners, edg1.vidx1, edg1.vidx2, edg2.vidx2);
            }else if(edg1.vidx1 == edg2.vidx2){
                adot = angledot(corners, edg1.vidx1, edg1.vidx2, edg2.vidx1);
            }else if(edg1.vidx2 == edg2.vidx1){
                adot = angledot(corners, edg1.vidx2, edg1.vidx1, edg2.vidx2);
            }else if(edg1.vidx2 == edg2.vidx2){
                adot = angledot(corners, edg1.vidx2, edg1.vidx1, edg2.vidx1);
            }
            //printf("joined edge %d & %d : %f\n", e1, e2, adot);
            if(-0.95 > adot){
                //Merge 2 groups to 1
                ushort srcg = std::max(edg1.edgegroup, edg2.edgegroup);
                ushort desg = std::min(edg1.edgegroup, edg2.edgegroup);
                //printf("merge edge group %d to %d\n", srcg, desg);
                for(int e = 0; e < edgcnt; e++){
                    if(srcg == edges[e].edgegroup){
                        edges[e].edgegroup = desg;
                    }
                }
            }
        }
    }
	//indexing hardedges
	hardedges.clear();
	hardedges.reserve(num_edgehard);
	for (int e1 = 0; e1 < edgcnt; e1++) {
		if (edges[e1].hard) {
			hardedges.push_back(e1);
		}
	}
    //printedgesdump(b.edges, b.num_edge);
    
    //--- find shared normal of all corners ---
    int ccnt = (int) corners.size();
	std::vector<bool> cornerhard(ccnt);
    std::vector<int> corneredge1(ccnt);
	std::vector<Vec3> cornersumnors(ccnt);
	std::vector<ushort> cornercntnors(ccnt);
	for(int i = 0; i < ccnt; i++){
		cornerhard[i] = false;
        corneredge1[i] = -1;
		cornercntnors[i] = 0;
		cornersumnors[i] = Vec3(0,0,0);
    }

	//find corner with join hard edges of 2 group to be hard corner
	int num_cornerhard = 0;
	for(int e = 0; e < edgcnt; e++){
        objedge_t &edg = edges[e];
		for (int i = 0; i < 2; i++) {
			int c = (i == 0) ? edg.vidx1 : edg.vidx2;
			if (cornerhard[c])
				continue;
			if (edg.hard) {
				if (-1 == corneredge1[c]) {
					corneredge1[c] = edg.edgegroup;
				}
				else if (corneredge1[c] != edg.edgegroup) {
					cornerhard[c] = true;
					num_cornerhard++;
				}
			}else{
				cornercntnors[c]+=2;
				cornersumnors[c] += (tris[edg.tri1].facenormal + tris[edg.tri2].facenormal);
			}
		}
    }
		
	//find fake soft corner like top of cone
	for (int i = 0; i < ccnt; i++) {
		if (cornerhard[i])
			continue;
		if (-1 != corneredge1[i])
			continue;
		if (0.5f > (cornersumnors[i].length() / cornercntnors[i])) {
			cornerhard[i] = true;
			num_cornerhard++;
		}
	}

	//indexing hard corners
	hardcorners.reserve(num_cornerhard);
	for (int i = 0; i < ccnt; i++) {
		if (cornerhard[i]){
			hardcorners.push_back(i);
		}
	}

	//=== === remap group and gen mix vec for tris === ===
	mixvecs.reserve(num_tri * 3);
	for (int t1 = 0; t1 < num_tri; t1++) {
		objtri_t &tri = tris[t1];
		for (int i = 0; i < 3; i++) {
			tri.mixvidxs[i] = vmixfindoradd(mixvecs, (int) mixvecs.size(),
                                            corners[tri.vidxs[i]], tri.nors[i], tri.uvs[i]);
		}
		tri.facegroup = trigroupremap[tri.facegroup];
	}

//    for(int t1 = 1; t1 < num_tri; t1++){
//        objtri_t &tri1 = b.tris[t1];
//        for(int t2 = 0; t2 < t1; t2++){
//            objtri_t &tri2 = b.tris[t2];
//            if(tri1.facegroup != tri2.facegroup)
//                continue;
//            //printf("tri %d and tri %d are both group %d\n", t1, t2, tri1.facegroup);
//        }
//    }
}

inline void mixvec2MdlVec(cG3DefModel *mdl, int vidx, const vecmix_t &mv) {
	mdl->vers[vidx] = mv.vec;
	mdl->nors[vidx] = mv.nor;
	mdl->uvs[vidx] = mv.uv;
}

inline void copy3short(std::vector<ushort> &idxs, int iidx, ushort *triidx) {
	idxs[iidx] = triidx[0];
	idxs[iidx+1] = triidx[1];
	idxs[iidx+2] = triidx[2];
}

inline void mdlAlloc(cG3DefModel *mdl, int vcnt, int icnt) {
	mdl->num_verts = vcnt;
	mdl->nors.resize(vcnt);
	mdl->vers.resize(vcnt);
	mdl->uvs.resize(vcnt);

	mdl->num_idx = icnt;
	mdl->idxs.resize(icnt);
}

void cObjLoadBuffer::clear() {
	corners.clear();
	corners.reserve(0);
	hardcorners.clear();
	hardcorners.reserve(0);

	edges.clear();
	edges.reserve(0);
	hardedges.clear();
	hardedges.reserve(0);

	mixvecs.clear();
	mixvecs.reserve(0);
	tris.clear();
	tris.reserve(0);

	num_facegroup = 0; // num of group of tris
}

cG3DefModel * cObjLoadBuffer::makeModel(){
    cG3DefModel *m = new cG3DefModelGen("Obj2Gen");
	const int vcnt = (int) mixvecs.size();
	const int num_tri = (int) tris.size();
	mdlAlloc(m, vcnt, num_tri * 3);
	for (int i = 0; i < vcnt; i++) {
		mixvec2MdlVec(m, i, mixvecs[i]);
	}
	for (int t = 0; t < num_tri; t++) {
		copy3short(m->idxs, t * 3, tris[t].mixvidxs);
	}
	return m;
}

void cObjLoadBuffer::makeFaceModels(std::vector<cG3DefModel *> &facemdls) {
	int fcnt = num_facegroup;
	facemdls.reserve(facemdls.size() + fcnt);

	int icnt = (int) mixvecs.size();
	std::vector<short> vecmap(icnt);
	int tcnt = (int) tris.size();

	for (int g = 0; g < fcnt; g++) {
		//scan tris to build copy list
		for (int i = 0; i < icnt; i++) {
			vecmap[i] = -1;
		}
		int trifound = 0;
		int mvfound = 0;
		for (int t = 0; t < tcnt; t++) {
			objtri_t &tri = tris[t];
			if (tri.facegroup != g)
				continue;
			trifound++;
			for(int m = 0; m < 3; m++){
				int mvi = tri.mixvidxs[m];
				if (vecmap[mvi] == -1){
					vecmap[mvi] = mvfound;
					mvfound++;
				}
			}
		}
		assert((mvfound > 0) && (mvfound <= icnt));
		assert((trifound > 0) && (trifound <= tcnt));

		//copy to a new model
		cG3DefModel *mdl = new cG3DefModel("facemodel");
		mdlAlloc(mdl, mvfound, trifound * 3);
		for (int i = 0; i < icnt; i++) {
			int mapv = vecmap[i];
			if(mapv != -1)
				mixvec2MdlVec(mdl, mapv, mixvecs[i]);
		}

		int cpt = 0;
		const int num_tri = (int) tris.size();
		for (int t = 0; t < num_tri; t++) {
			objtri_t &tri = tris[t];
			if (g != tri.facegroup)
				continue;
			for(int ti = 0; ti < 3; ti++)
				mdl->idxs[cpt + ti] = vecmap[tri.mixvidxs[ti]];
			cpt += 3;
		}
		facemdls.push_back(mdl);
	}
}

void cObjLoadBuffer::makeCornerModels(std::vector<cG3DefModel *> &cornmdls, std::vector<Mat4> &m4s, float size) {
	int ccnt = (int) hardcorners.size();
	cornmdls.reserve(cornmdls.size() + ccnt);
	m4s.reserve(m4s.size() + ccnt);

	Mat4 mat4;
	for (int c = 0; c < ccnt; c++) {
		cornmdls.push_back(new cG3DefModelGenSphere("corner", 8, 4, size, 0));
		lookatMatrix(corners[hardcorners[c]], Vec3(1, 0, 0), Vec3(0, 0, 1), mat4);
		m4s.push_back(mat4);
	}
}

void cObjLoadBuffer::makeEdgeModels(std::vector<cG3DefModel *> &edgemdls, std::vector<Mat4> &m4s, float size) {
	int ecnt = (int) hardedges.size();
	edgemdls.reserve(edgemdls.size() + ecnt);
	m4s.reserve(m4s.size() + ecnt);

	Vec3 vup;
	Mat4 mat4;
	for (int e = 0; e < ecnt; e++) {
		objedge_t &ed = edges[hardedges[e]];
		const Vec3 &veye = corners[ed.vidx1];
		const Vec3 &vspot = corners[ed.vidx2];
		const Vec3 vdir = vspot - veye;
		if ((0 == vdir.x) && (0 == vdir.y)) {
			vup = Vec3(0, vdir.z, 0);
		}else{
			vup = Vec3(0, 0, 1);
		}
		edgemdls.push_back(new cG3DefModelGen("egdes", 6,
			sqrtf(vectorLensq(vdir)), 0, size, 1.0f, 0.0f, true));
		lookatMatrix(veye, vspot, vup, mat4);
		m4s.push_back(mat4);
	}
}


int cObjLoadBuffer::hittestfaces(const Vec3 &vnear, const Vec3 &vdir) {
	const int tcnt = (int) tris.size();

	int hitgroup = -1;
	float distsq = MAXFLOAT;
	for (int t = 0; t < tcnt; t++) {
		objtri_t &tri = tris[t];
		bool hit1 = false;
		float dist1 = MAXFLOAT;
		hittestlinetri(vnear, vdir, corners[tri.vidxs[0]], corners[tri.vidxs[1]], corners[tri.vidxs[2]],
			&hit1, &dist1);
		//NSLog(@"do_hittestfaces() i=%d, hit=%d, distsq=%f", i, hit1, dist1);
		if (hit1) {
			if (dist1 < distsq) {
				distsq = dist1;
				hitgroup = tri.facegroup;
			}
		}
	}
	return hitgroup;
}

int cObjLoadBuffer::hittestedges(const Vec3 &vnear, const Vec3 &vfar, float &distsq) {
	distsq = MAXFLOAT;
	// Edge Test
	int edgecnt = (int) hardedges.size();
	if (0 >= edgecnt) {
		return -1;
	}
	int nearedge = -1;

	Vec3 out1, out2;
	for (int i = 0; i < edgecnt; i++) {
		objedge_t &eg = edges[hardedges[i]];
		float edistsq = hittestlineline2(vnear, vfar, corners[eg.vidx1], corners[eg.vidx2], out1, out2);
		if (edistsq < distsq) {
			distsq = edistsq;
			nearedge = i;
		}
	}
	//NSLog(@"do_hittestedges() Ecnt=%d C=%d(%f)", edgecnt, nearedge, distsq);
	return nearedge;
}

void cObjLoadBuffer::logdebug() {
	LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBuffer::logdebug()");
	int i = 0;
	for (const auto &t : mixvecs)
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBuffer::mixvecs[%d] = (%f, %f, %f)", i++, t.vec.x, t.vec.y, t.vec.z);
	i = 0;
	for (const auto &t : tris)
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBuffer::tris[%d] group = %d, idx = %d-%d-%d", i++, t.facegroup, t.vidxs[0], t.vidxs[1], t.vidxs[2]);
	i = 0;
	for (const auto &v : corners)
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBuffer::corners[%d] = (%f, %f, %f)", i++, v.x, v.y, v.z);
	i = 0;
	for (const auto h : hardcorners)
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBuffer::hardcorners[%d] = %d", i++, h);
	i = 0;
	for (const auto &e : edges)
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBuffer::edges[%d] group = %d, idx = %d-%d", i++, e.edgegroup, e.vidx1, e.vidx2);
	i = 0;
	for (const auto &h : hardedges)
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBuffer::hardedges[%d] = %d", i++, h);
}

void OBJ_LoadTopo(cObjLoadBuffer &b, const char *data, int len, float loadscale) {
	cObjLoadBufferRaw r;
	r.loadObj(data, len, loadscale);
	//r.print();
	b.process_rawbuffer(r);
	r.reserve(0, 0, 0, 0, 0);
}

const int sizeoffloat = sizeof(float);
inline MeshVertexAttrib makeVerAtt(int att, int size) {
	MeshVertexAttrib attrib;
	attrib.size = size;
	attrib.type = GL_FLOAT;
	attrib.vertexAttrib = att;
	attrib.attribSizeBytes = attrib.size * sizeoffloat;
	return attrib;
}

inline void pushvec3(std::vector<float> &vdest, const Vec3 &v) {
	vdest.push_back(v.x);
	vdest.push_back(v.y);
	vdest.push_back(v.z);
}
inline void pushvec2(std::vector<float> &vdest, const Vec2 &v) {
	vdest.push_back(v.x);
	vdest.push_back(v.y);
}


inline cocos2d::AABB calculateAABB(const std::vector<float>& vertex, int stride, const std::vector<unsigned short>& index) {
	AABB aabb;
	stride /= 4;
	for (const auto& it : index) {
		Vec3 point(vertex[it * stride], vertex[it * stride + 1], vertex[it * stride + 2]);
		aabb.updateMinMax(&point, 1);
	}
	return aabb;
}

Sprite3D *DefModel2Sprite3D(cG3DefModel *mdl) {
	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"DefModel2Sprite3D() vecs=%d, idxs=%d", mdl->num_verts, mdl->num_idx);
	MeshDatas meshdatas;
	MaterialDatas materialdatas;
	NodeDatas nodedatas;

	MeshData* meshdata = new (std::nothrow) MeshData();
	meshdata->attribs.push_back(makeVerAtt(GLProgram::VERTEX_ATTRIB_POSITION, 3));
	meshdata->attribs.push_back(makeVerAtt(GLProgram::VERTEX_ATTRIB_NORMAL, 3));
	meshdata->attribs.push_back(makeVerAtt(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2));
	
	int vcnt = mdl->num_verts;
	/*
	meshdata->vertex.clear();
	meshdata->vertex.reserve(vcnt * 8);
	for (int v = 0; v < vcnt; v++) {
		pushvec3(meshdata->vertex, mdl->vers[v]);
		pushvec3(meshdata->vertex, mdl->nors[v]);
		pushvec2(meshdata->vertex, mdl->uvs[v]);
		//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"mdl v[%d] = v(%f,%f,%f)n(%f,%f,%f)u(%f,%f)", v,
		//	mdl->vers[v].x, mdl->vers[v].y, mdl->vers[v].z,
		//	mdl->nors[v].x, mdl->nors[v].y, mdl->nors[v].z,
		//	mdl->uvs[v].x, mdl->uvs[v].y);
	}
	*/
	int fcnt = vcnt * 8;
	meshdata->vertex.resize(fcnt);
	float *out = meshdata->vertex.data();
	const Vec3 *inv = mdl->vers.data();
	const Vec3 *inn = mdl->nors.data();
	const Vec2 *inu = mdl->uvs.data();
	float minx = mdl->vers[0].x;
	float miny = mdl->vers[0].y;
	float minz = mdl->vers[0].z;
	float maxx = minx, maxy = miny, maxz = minz;
	int i = 0;
	for (int f = 0; f < fcnt; f += 8) {
		const Vec3 &v = inv[i];
		const Vec3 &n = inn[i];
		const Vec2 &u = inu[i];
		if (v.x < minx)
			minx = v.x;
		else if (v.x > maxx)
			maxx = v.x;
		if (v.y < miny)
			miny = v.y;
		else if (v.y > maxz)
			maxz = v.y;
		if (v.z < minz)
			minz = v.z;
		else if (v.z > maxz)
			maxz = v.z;

		out[f] = v.x;
		out[f + 1] = v.y;
		out[f + 2] = v.z;
		out[f + 3] = n.x;
		out[f + 4] = n.y;
		out[f + 5] = n.z;
		out[f + 6] = u.x;
		out[f + 7] = u.y;
		i++;
	}

	std::string mdlname = "mdl2sprite3d";
	int icnt = mdl->num_idx;
	meshdata->subMeshIndices.push_back(std::vector<ushort>());
	std::vector<ushort> &mis = meshdata->subMeshIndices.back();
	mis.resize(icnt);
	ushort *iout = mis.data();
	unsigned short *iin = mdl->idxs.data();
	for (int i = 0; i < icnt; i++) {
		iout[i] = iin[i];
	}
	//for (int i = 0; i < icnt; i+=3)
	//	LOGUTIL(LEVEL_INFO, tagtag::TAG(),"mdl fi[%d] = %d-%d-%d", i / 3, mdl->idxs[i], mdl->idxs[i + 1], mdl->idxs[i + 2]);
	meshdata->subMeshAABB.push_back(AABB(Vec3(minx, miny, minz), Vec3(maxx, maxy, maxz)));
	meshdata->subMeshIds.push_back(mdlname);

	auto modelnode = new (std::nothrow) ModelData();
	//modelnode->matrialId = ""; //materialId = "";
	modelnode->subMeshId = mdlname;

	auto node = new (std::nothrow) NodeData();
	node->id = mdlname;
	node->modelNodeDatas.push_back(modelnode);
	
	nodedatas.nodes.push_back(node);
	meshdatas.meshDatas.push_back(meshdata);

	Sprite3D *sp3d = Sprite3D::create();
	sp3d->initFrom(nodedatas, meshdatas, materialdatas);
	
	meshdatas.resetData();
	materialdatas.resetData();
	nodedatas.resetData();

	return sp3d;
}

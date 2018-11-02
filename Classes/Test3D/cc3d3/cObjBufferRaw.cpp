//#include "CC2D3Util.h"
#include "cObjBufferRaw.h"
#include "cStrWalker.h"
#include "LogUtil.h"

void cObjLoadBufferRaw::reserveperfilesize(int fsize) {
	//Minimum but sure enough memory reserve;
	ushort maxv = fsize / sizeof("v 0 0 0 ");
	ushort maxu = fsize / sizeof("vt 0 0 ");
	ushort maxo = fsize / sizeof("  f 0 0 0");
	ushort maxi = maxo * 4;
	ushort maxtri = maxi / 3;
	reserve(maxv, maxu, maxi, maxo, maxtri);
}

void cObjLoadBufferRaw::reserve(ushort maxv, ushort maxu, ushort maxi, ushort maxo, ushort maxtri) {
	nors.clear();
	nors.reserve(maxv);
	vecs.clear();
	vecs.reserve(maxv);
	uvs.clear();
	uvs.reserve(maxu);

	idx_n.clear();
	idx_n.reserve(maxi);
	idx_v.clear();
	idx_v.reserve(maxi);
	idx_u.clear();
	idx_u.reserve(maxi);

	idx_of1.clear();
	idx_of1.reserve(maxo);
	num_of.clear();
	num_of.reserve(maxo);

	idx_fv1.clear();
	idx_fv1.reserve(maxtri);
	num_fv.clear();
	num_fv.reserve(maxtri);
}


void cObjLoadBufferRaw::loadObj(const char *data, int len, float loadscale) {
	reserveperfilesize(len);

	cStrWalker filewalker(data, len);
	const char *delimit = " \t\n";
	const int buflen = 4096;
	char buf[buflen];

	int linenum = 0;
	num_tri = 0;
	num_face = 0;
	num_of.push_back(0);//at least 1 object start with 000 face
	idx_of1.push_back(0);//first face or first object is 0;
	while (0 <= filewalker.nextnonempty('\n')) {
		linenum++;
		cStrWalker linewalker(filewalker.curtoken, filewalker.curlen);
		linewalker.nextnonempty(delimit);
		linewalker.cpcurtoken(buf, buflen);
		//printf("ln:%3d l:%3d '%s'\n", linenum, linewalker.curlen, buf);

		if (!strcmp("v", buf)) {
			float x = linewalker.nextnonemptyfloat(delimit, 0.0f) * loadscale;
			float y = linewalker.nextnonemptyfloat(delimit, 0.0f) * loadscale;
			float z = linewalker.nextnonemptyfloat(delimit, 0.0f) * loadscale;
			vecs.push_back(Vec3(x, y, z));
			//printf("v[%d] (%f, %f, %f)\n", b.num_vec,
			//       b.Vecs[b.num_vec][0], b.Vecs[b.num_vec][1], b.Vecs[b.num_vec][2]);
		}
		else if (!strcmp("vt", buf)) {
			float x = linewalker.nextnonemptyfloat(delimit, 0.0f);
			float y = 1 - linewalker.nextnonemptyfloat(delimit, 0.0f);
			uvs.push_back(Vec2(x, y));
			//printf("vt[%d] (%f, %f)\n", b.num_uv,
			//       b.UVs[b.num_uv][0], b.UVs[b.num_uv][1]);
		}
		else if (!strcmp("vn", buf)) {
			float x = linewalker.nextnonemptyfloat(delimit, 0.0f);
			float y = linewalker.nextnonemptyfloat(delimit, 0.0f);
			float z = linewalker.nextnonemptyfloat(delimit, 0.0f);
			nors.push_back(Vec3(x, y, z));
		//printf("vn[%d] (%f, %f, %f)\n", b.num_nor,
			//       b.Nors[b.num_nor][0], b.Nors[b.num_nor][1], b.Nors[b.num_nor][2]);
		}
		else if (!strcmp("f", buf)) {
			idx_fv1.push_back(idx_v.size());
			num_fv.push_back(0);
			while (0 < linewalker.nextnonempty(delimit)) {
				cStrWalker walker1(linewalker.curtoken, linewalker.curlen);
				idx_v.push_back((short)walker1.nextfloat('/', 0.0f) - 1);
				idx_u.push_back((short)walker1.nextfloat('/', 0.0f) - 1);
				idx_n.push_back((short)walker1.nextfloat('/', 0.0f) - 1);
				//printf("o[%d]-face[%d]-idx[%d]: (%d / %d / %d)\n",
				//       b.num_obj, b.num_face, b.num_idx,
				//       b.idx_v[b.num_idx], b.idx_u[b.num_idx], b.idx_n[b.num_idx]);
				num_fv.back()++;
			}
			num_face++;
			num_tri += (num_fv.back() - 2);
			num_of.back()++;
		}
		else if (!strcmp("o", buf)) {
			//close old, and open New Object only when current object has face
			if (0 < num_of.back()) {
				num_of.push_back(0);
				idx_of1.push_back(num_face);
			}
		}
		else if (!strcmp("usemtl", buf)) {
			//skip material
		}
		else {
			//skip comment etc...
		}
	}
	//remove last object if empty
	if (0 >= num_of.back()) {
		num_of.pop_back();
		idx_of1.pop_back();
	}
	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBufferRaw::loadObj o:%d f:%d t:%d\n", num_of.size(), num_face, num_tri);
}

void cObjLoadBufferRaw::print() {
	for (int i = 0; i < (int)vecs.size(); i++)
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBufferRaw::print() vecs[%d] = (%f, %f, %f)", i, vecs[i].x, vecs[i].y, vecs[i].z);
	for (int i = 0; i < (int)nors.size(); i++)
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBufferRaw::print() nors[%d] = (%f, %f, %f)", i, nors[i].x, nors[i].y, nors[i].z);
	for (int i = 0; i < (int)uvs.size(); i++)
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBufferRaw::print() uvs[%d] = (%f, %f)", i, uvs[i].x, uvs[i].y);

	for (int i = 0; i < (int)idx_v.size(); i++)
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBufferRaw::print() idx vnu[%d] = (%d / %d / %d)", i, idx_v[i], idx_n[i], idx_u[i]);

	for (int i = 0; i < (int)idx_of1.size(); i++) {
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBufferRaw::print() o[%d] idx_of1=%d, num_of=%d", i, idx_of1[i], num_of[i]);
	}
	for (int i = 0; i < (int)idx_fv1.size(); i++) {
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBufferRaw::print() f[%d] idx_fv1=%d,num_fv=%d", i, idx_fv1[i], num_fv[i]);
	}
	LOGUTIL(LEVEL_INFO, tagtag::TAG(),"cObjLoadBufferRaw::print() numtri=%d, numface=%d", num_tri, num_face);
}

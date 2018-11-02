#include "Fold3DLoader.h"

#include "json/rapidjson.h"     // rapidjson's DOM-style API
#include "json/document.h"     // rapidjson's DOM-style API
#include "LogUtil.h"

//#include "CC3DMath.h"


/*
struct sFolderObj {

	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(path);
	std::string FileUtils::getStringFromFile(const std::string& filename)

		Vec3 startAngle;
	Vec3 endAngle;
	float startTime;
	float endTime;
};
*/

USING_NS_CC;
//using namespace rapidjson;
//using namespace std;


Sprite3D* addSprite3D(Node *pnode, const char *objname, const char *pngname, const float scale, const Vec3 &pos, const Vec3 &rot) {
	log("------------%s %s %f, (%f,%f,%f) (%f,%f,%f)", objname, pngname, scale, pos.x, pos.y, pos.z, rot.x, rot.y, rot.z);
	Sprite3D *sp3d = Sprite3D::create(objname);
	sp3d->setTexture(pngname);
	sp3d->setScale(scale);
	sp3d->setPosition3D(pos);
	sp3d->setRotationQuat(Eul2Quat(rot));
	if (pnode) {
		//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"mask: %i", (int)pnode->getCameraMask());
		pnode->addChild(sp3d);
		sp3d->setCameraMask(pnode->getCameraMask());
	}
	return sp3d;
}

Vec3 ja2rot(const rapidjson::Value &jrot) {
    return Vec3(
                CC_DEGREES_TO_RADIANS((float)jrot[(rapidjson::SizeType)0].GetDouble()),
                CC_DEGREES_TO_RADIANS((float)jrot[(rapidjson::SizeType)1].GetDouble()),
                CC_DEGREES_TO_RADIANS((float)jrot[(rapidjson::SizeType)2].GetDouble())
	);
}
Vec3 ja2pos(const rapidjson::Value &jpos) {
    const rapidjson::Value &x = jpos[(rapidjson::SizeType)0],
    &y = jpos[(rapidjson::SizeType)1],
    &z = jpos[(rapidjson::SizeType)2];
    return Vec3(x.GetDouble(), y.GetDouble(), z.GetDouble());
//	return Vec3(jpos[0].GetDouble(), jpos[1].GetDouble(), jpos[2].GetDouble());
}


std::vector<cocos2d::Sprite3D*> ObjList_Load(Node *rootnode, const char *json, const std::string &basepath) {
	std::vector<cocos2d::Sprite3D*> ret;
	rapidjson::Document doc;
	if (doc.Parse<0>(json).HasParseError()) {
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"ERROR LOADING SCENE JSON at char %d", (int)doc.GetErrorOffset());
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"%s", json);
		return ret;
	}

	const rapidjson::Value &ja = doc["scene"];
	for (int i = 0; i < (int)ja.Size(); i++) { // rapidjson uses SizeType instead of size_t.
		const rapidjson::Value &o = ja[i];
		std::string obj = o["obj"].GetString();
		std::string png = o["png"].GetString();
		float scale = o["scale"].GetDouble();
		Vec3 pos = ja2pos(o["pos"]);
		Vec3 rot = ja2rot(o["rot"]);

		int pidx = o["parent"].GetInt();
		Node *pnode = ((pidx <= (int)ret.size()) && (pidx >= 0)) ? ret[pidx] : rootnode;
		//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"Use Root %d, size=%d idx=%d", (pnode==rootnode), ret.size(), pidx);
		Sprite3D *sp3d = addSprite3D(pnode, (basepath + obj).c_str(), (basepath + png).c_str(), scale, pos, rot);
		ret.push_back(sp3d);
	}
	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"ret count %i", ret.size());
	return ret;
}

template <typename T>
bool hastrack(int i, std::map<int, T> &map) {
	return map.find(i) != map.end();
}

template <typename T>
T &gettrack(int i, std::map<int, T> &map) {
	return map[i];
}

void ObjList_AniApply(const std::vector<cocos2d::Sprite3D*> &objs, const Sprite3DAni &ani, float time){
	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"apply ani %f", time);
	const Sprite3DTrackScaleMap &scaletracks = ani.scaletracks;
	for (Sprite3DTrackScaleMap::const_iterator itr = scaletracks.begin(); itr != scaletracks.end(); itr++) {
		//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"apply scale %d", itr->first);
		Sprite3DAniScale(objs[itr->first], itr->second, time);
	}
	const Sprite3DTrackRotMap &rottracks = ani.rottracks;
	for (Sprite3DTrackRotMap::const_iterator itr = rottracks.begin(); itr != rottracks.end(); itr++) {
		//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"apply rot %d", itr->first);
		Sprite3DAniRot(objs[itr->first], itr->second, time);
	}
	const Sprite3DTrackPosMap &postracks = ani.postracks;
	for (Sprite3DTrackPosMap::const_iterator itr = postracks.begin(); itr != postracks.end(); itr++) {
		//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"apply pos %d", itr->first);
		Sprite3DAniPos(objs[itr->first], itr->second, time);
	}
}


void ObjList_AniLoad(Sprite3DAni &ani, const char *json) {
	rapidjson::Document doc;
	if (doc.Parse<0>(json).HasParseError()) {
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"ERROR LOADING Ani JSON at char %d", (int)doc.GetErrorOffset());
		LOGUTIL(LEVEL_INFO, tagtag::TAG(),"%s", json);
		return;
	}

	const rapidjson::Value &ja = doc["tracks"];
	for (int i = 0; i < (int)ja.Size(); i++) { // rapidjson uses SizeType instead of size_t.
		const rapidjson::Value &track = ja[i];
		const int objidx = track["objidx"].GetInt();
		const rapidjson::Value &keys = track["keys"];
		for (int j = 0; j < (int)keys.Size(); j++) { // rapidjson uses SizeType instead of size_t.
			const rapidjson::Value &key = keys[j];
			const float time = key["time"].GetDouble();
			ani.maxtime = fmaxf(time, ani.maxtime);
			if (key.HasMember("rot")) {
				//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"add rot key o=%d t=%f", objidx, time);
				ani.rottracks[objidx][time] = ja2rot(key["rot"]);
			}
			else if (key.HasMember("pos")) {
				//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"add pos key o=%d t=%f", objidx, time);
				ani.postracks[objidx][time] = ja2pos(key["pos"]);
			}
			else if (key.HasMember("scale")) {
				//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"add scale key o=%d t=%f", objidx, time);
				ani.scaletracks[objidx][time] = key["scale"].GetDouble();
			}
		}
	}
}

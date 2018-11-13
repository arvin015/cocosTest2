//
// Created by mac_arvin on 2018/11/8.
//

#include "FoldPaperFoldLayer.h"
#include "CC3DMath.h"
#include "BasicData.h"

USING_NS_CC;
using namespace std;

namespace FoldPaper {

    const float FOLD_SPEED = 1.2f;
    const Color4F BACK_FACE_COLOR = Color4F(0.8f, 0.8f, 0.8f, 1.0f);
    const Color4F DIVIDE_LINE_COLOR = Color4F(229 / 255.0, 229 / 255.0, 229 / 255.0, 1.0f);

    //根据父层数量排序
    class SortNodeByLevel {
    public:
        bool operator()(Polygon3D* n0, Polygon3D* n1) const;
    };
    bool SortNodeByLevel::operator()(Polygon3D* n0, Polygon3D* n1) const {
        return n0->getDepth() < n1->getDepth() ;
    }

    FoldPaperFoldLayer::FoldPaperFoldLayer()
    : touchListener(nullptr),
      cc3dLayer(nullptr),
      camtarget(0, 0, 0),
      camoffset(0, 0, 8),
      isFolding(false),
      isFoldOver(false),
      rootPolygon3D(nullptr),
      foldingPolygon3D(nullptr),
      show3DContainer(nullptr) {

    }

    FoldPaperFoldLayer::~FoldPaperFoldLayer() {

    }

    void FoldPaperFoldLayer::onEnter() {
        Layer::onEnter();

        touchListener = EventListenerTouchOneByOne::create();
        touchListener->onTouchBegan = CC_CALLBACK_2(FoldPaperFoldLayer::onTouchBegan, this);
        touchListener->onTouchMoved = CC_CALLBACK_2(FoldPaperFoldLayer::onTouchMoved, this);
        touchListener->onTouchEnded = CC_CALLBACK_2(FoldPaperFoldLayer::onTouchEnded, this);
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);

        scheduleUpdate();
    }

    void FoldPaperFoldLayer::onExit() {

        if (touchListener != nullptr) {
            Director::getInstance()->getEventDispatcher()->removeEventListener(touchListener);
        }

        Layer::onExit();
    }

    bool FoldPaperFoldLayer::init() {
        if (!Layer::init()) {
            return false;
        }

        cc3dLayer = CC3DLayer::create();
        cc3dLayer->userCameraFlag(CameraFlag::USER1);
        addChild(cc3dLayer);
        cc3dLayer->addChild(AmbientLight::create(Color3B(160, 160, 160)));
        cc3dLayer->_camera->addChild(DirectionLight::create(Vec3(1, -3, -5).getNormalized(), Color3B(100, 100, 100)));
        cc3dLayer->setCamLoc(camtarget, camquat, camoffset);

        show3DContainer = Node::create();
        show3DContainer->setContentSize(Size(1024, 768));
        show3DContainer->setCameraMask(cc3dLayer->getCameraMask());
        cc3dLayer->addChild(show3DContainer);

        return true;
    }

    void FoldPaperFoldLayer::update(float dt) {

        if (!isVisible()) return;

        fold(dt); //折叠
    }

    void FoldPaperFoldLayer::fold(float dt) {

        if (!isFolding || isFoldOver || polygon3DList.size() < 2 || foldingPolygon3D == nullptr) return;

        int id0 = foldingPolygon3D->foldAxisIds[0];
        int id1 = foldingPolygon3D->foldAxisIds[1];

        if (id0 >= 0 && id1 >= 0) {
            Vec3 p0 = foldingPolygon3D->vertexList[id0].position;
            Vec3 p1 = foldingPolygon3D->vertexList[id1].position;
            float rotate_angle = -FOLD_SPEED * dt;
            float rotatedTotal = foldingPolygon3D->rotatedTotal;
            if (rotatedTotal + rotate_angle > foldingPolygon3D->targetAngle) {
                foldingPolygon3D->recursiveRotate(p0, p1, rotate_angle);
                foldingPolygon3D->rotatedTotal += rotate_angle;
            } else {

                //密封
                float deltaAngle = foldingPolygon3D->targetAngle - foldingPolygon3D->rotatedTotal;
                foldingPolygon3D->recursiveRotate(p0, p1, deltaAngle);
                foldingPolygon3D->rotatedTotal = foldingPolygon3D->targetAngle;

                mIter++;
                if (mIter != polygon3DList.end()) {
                    foldingPolygon3D = *mIter;

                    if (!canFoldThisPolygon(foldingPolygon3D)) {
                        bool hasFoldableNode = false;

                        std::vector<Polygon3D*>::iterator it;
                        for (it = mIter++; it != polygon3DList.end(); it++) {
                            if (canFoldThisPolygon(*it)) {
                                foldingPolygon3D = *it;
                                hasFoldableNode = true;
                                mIter = it;
                                break;
                            }
                        }

                        if (!hasFoldableNode) {
                            isFolding = false;
                            foldingPolygon3D = nullptr;
                        }
                    }

                } else {
                    isFoldOver = true;
                }
            }
        }

        draw3D();
    }

    void FoldPaperFoldLayer::draw3D() {

        show3DContainer->removeAllChildren();

        vector<Polygon3D*> backFaces;
        vector<Polygon3D*> frontFaces;

        for (Polygon3D* polygon3D : polygon3DList) {

            Vec3 n = polygon3D->getNormal();
            Vec3 viewDir = polygon3D->getCenter() - cc3dLayer->_camera->getPosition3D();
            viewDir.normalize();
            viewDir = viewDir.getNormalized();

            if (n.dot(viewDir) > 0.0f) {
                frontFaces.push_back(polygon3D);
            } else {
                backFaces.push_back(polygon3D);
            }
        }

        //先绘制后面
        for (Polygon3D* backPolygon3D : backFaces) {
            draw3DObj(backPolygon3D, false);
        }

        //再绘制前面
        for (Polygon3D* frontPolygon3D : frontFaces) {
            draw3DObj(frontPolygon3D, true);
        }
    }

    bool FoldPaperFoldLayer::canFoldThisPolygon(Polygon3D* polygon) {
        if (polygon == nullptr) return false;

        if (polygon->foldAxisIds.size() < 1) return false;

        bool canFold = true;

        polygon->backup(true); //备份

        int id0 = polygon->foldAxisIds[0];
        int id1 = polygon->foldAxisIds[1];

        vector<Polygon3D*> aboutToFoldPolygons;
        collect(polygon, aboutToFoldPolygons);

        int step = 60;
        float targetAngle = polygon->targetAngle;
        float angle_inc = targetAngle / (float)step;

        Vec3 p0 = polygon->vertexList[id0].position;
        Vec3 p1 = polygon->vertexList[id1].position;

        for (int s = 0; s <= 1 + step; s++) {
            polygon->recursiveRotate(p0, p1, targetAngle);
            for (int i = 0; i < (int)polygon3DList.size(); i++) {
                Polygon3D* p = polygon3DList[i];

                if (polygon->parent == p) continue;

                vector<Polygon3D*>::iterator it = find(aboutToFoldPolygons.begin(), aboutToFoldPolygons.end(), p);
                if (it != aboutToFoldPolygons.end()) continue;

                for (int j = 0; j < (int)aboutToFoldPolygons.size(); j++) {
                    Polygon3D* pp = aboutToFoldPolygons[j];
                    if (p->isPolygonsIntersected(pp)) {
                        canFold = false;
                        break;
                    }
                }
            }
            targetAngle -= angle_inc;
        }
        polygon->restore(true); //还原
        return canFold;
    }

    void FoldPaperFoldLayer::collect(Polygon3D* parent, vector<Polygon3D*> &childs) {
        childs.clear();

        std::queue<Polygon3D*> q;
        q.push(parent);
        while (!q.empty()) {
            Polygon3D* p = q.front();
            q.pop();
            childs.push_back(p);
            for (int i = 0; i < p->child.size(); i++) {
                q.push(p->child[i]);
            }
        }
    }

    void FoldPaperFoldLayer::draw3DObj(Polygon3D* polygon3D, bool isFront) {
        auto draw3d = DrawNode3D::create(3);
        draw3d->setCameraMask(cc3dLayer->getCameraMask());
        show3DContainer->addChild(draw3d);

        //绘制颜色面
        Vec3* vertexs = new Vec3[polygon3D->vertexList.size()];
        for (int i = 0; i < polygon3D->vertexList.size(); i++) {
            vertexs[i] = polygon3D->vertexList[i].position;
        }
        draw3d->drawPolygon(vertexs, polygon3D->vertexList.size(), isFront ? polygon3D->polygonColor : BACK_FACE_COLOR);

        //绘制分割线
        if (isFront) {
            for (int i = 0; i < polygon3D->vertexList.size(); i++) {
                Vec3 p0 = polygon3D->vertexList[i].position;
                Vec3 p1 = polygon3D->vertexList[(i + 1) % polygon3D->vertexList.size()].position;
                draw3d->drawLine(p0, p1, DIVIDE_LINE_COLOR);
            }
        } else {
            if (polygon3D->foldAxisIds.size() > 0) {
                draw3d->drawLine(polygon3D->vertexList[polygon3D->foldAxisIds[0]].position,
                                 polygon3D->vertexList[polygon3D->foldAxisIds[1]].position, DIVIDE_LINE_COLOR);
            }
        }

        //绘制贴图
        if (isFront) {

            //纹理坐标
            Vec2 uvs[] = {
                    Vec2(0, 0),
                    Vec2(1, 0),
                    Vec2(1, 1),
                    Vec2(0, 1)
            };

            draw3d->set3DTextture(Director::getInstance()->getTextureCache()->addImage("paper_img_animal_tortoise.png")->getName());
            draw3d->drawPolygonForTexture(vertexs, uvs, polygon3D->vertexList.size());
        }

        delete[] vertexs;
    }

    bool FoldPaperFoldLayer::onTouchBegan(Touch* touch, Event* event) {
        if (!isVisible()) return false;
        multitouch.OnTouchDown(touch->getID(), convertTouchToNodeSpaceAR(touch));
        return true;
    }

    void FoldPaperFoldLayer::onTouchMoved(Touch* touch, Event* event) {
        multitouch.OnTouchMove(touch->getID(), convertTouchToNodeSpaceAR(touch));
        if (multitouch.active) {
            if (multitouch.wasactive) {
                Quaternion qz = Z2Quat(multitouch.mtLast.rad - multitouch.mt.rad);
                camquat = camquat * qz;
            }
        } else {
            Vec2 t1 = GLNormalized(convertToNodeSpaceAR(touch->getLocation()));

            Vec2 delta = GLNormalized(touch->getDelta());
            Quaternion qx = X2Quat(delta.y * 2.0f * (1 - (t1.x * t1.x)));
            Quaternion qy = Y2Quat(-delta.x * 2.0f * (1 - (t1.y * t1.y)));

            Vec2 t2 = GLNormalized(convertToNodeSpaceAR(touch->getPreviousLocation()));
            float r1 = atan2f(t1.y, t1.x);
            float r2 = atan2f(t2.y, t2.x);
            camquat = camquat * (qx * qy) * Z2Quat(RadWrap(r2 - r1) * t1.lengthSquared());
        }
        cc3dLayer->camquat = camquat;
        cc3dLayer->updateCamLoc();

        draw3D(); //移动Camera时需要重新绘制3D来区别正方面，假3D
    }

    void FoldPaperFoldLayer::onTouchEnded(Touch* touch, Event* event) {
        multitouch.OnTouchUp(touch->getID(), convertTouchToNodeSpaceAR(touch));
    }

    void FoldPaperFoldLayer::responseFoldClick(bool isChecked) {
        if (!isChecked) { //退出
            isFolding = false;
            rootPolygon3D = nullptr;
            foldingPolygon3D = nullptr;

            show3DContainer->removeAllChildren();
        } else { //进入
            //重置摄像头
            camquat = Quaternion();
            camtarget = Vec3::ZERO;
            camoffset = Vec3(0, 0, 8);
            cc3dLayer->setCamLoc(camtarget, camquat, camoffset);
        }
    }

    void FoldPaperFoldLayer::setData(std::vector<Polygon3D*> polygon3DList) {
        this->polygon3DList.clear();
        this->polygon3DList = polygon3DList;

        alignToOrigin();
        buildRelation();
        setFoldPolygon();

        isFolding = true;
        isFoldOver = false;
    }

    void FoldPaperFoldLayer::alignToOrigin() {
        Vec3 maxVertex = polygon3DList.at(0)->vertexList.at(0).position;
        Vec3 minVertex = polygon3DList.at(0)->vertexList.at(0).position;
        for (Polygon3D* p : polygon3DList) {
            for (Vertex vertex : p->vertexList) {
                Vec3 v = vertex.position;
                if (v.x < minVertex.x) {
                    minVertex.x = v.x;
                }
                if (v.y < minVertex.y) {
                    minVertex.y = v.y;
                }

                if (v.x > maxVertex.x) {
                    maxVertex.x = v.x;
                }
                if (v.y > maxVertex.y) {
                    maxVertex.y = v.y;
                }
            }
        }
        Vec3 center = (maxVertex + minVertex) / 2.0;
        for (Polygon3D* p : polygon3DList) {
            for (int i = 0; i < p->vertexList.size(); i++) {
                p->vertexList[i].position = (p->vertexList[i].position - center) / (120 / 2);
            }
        }
    }

    void FoldPaperFoldLayer::buildRelation() {

        map<Polygon3D*, vector<Polygon3D*>> relationMap;
        for (Polygon3D* polygon3D : polygon3DList) {
            relationMap.insert(make_pair(polygon3D, vector<Polygon3D*>()));
        }

        //建立相互吸附关系
        for (int i = 0; i < polygon3DList.size(); i++) {
            Polygon3D* polygon3D = polygon3DList.at(i);
            for (int j = i + 1; j < polygon3DList.size(); j++) {
                Polygon3D* polygon3D1 = polygon3DList.at(j);
                if (polygon3D->isStickTogether(polygon3D1, nullptr)) {
                    relationMap[polygon3D].push_back(polygon3D1);
                    relationMap[polygon3D1].push_back(polygon3D);
                }
            }
        }

        //找出被吸附最多的多边形
        int max = 0;
        map<Polygon3D*, vector<Polygon3D*>>::iterator it;
        for (it = relationMap.begin(); it != relationMap.end(); it++) {
            Polygon3D* cp = it->first;
            if (it->second.size() > max) {
                rootPolygon3D = cp;
                max = it->second.size();
            }
        }

        if (rootPolygon3D == nullptr) return;

        //确定上下面的边数
        float baseSideCount = 3;
        map<Polygon3D*, vector<Polygon3D*>>::iterator it1;
        for (it1 = relationMap.begin(); it1 != relationMap.end(); it1++) {
            if (it1->first->faceType == FaceType::FaceTypeBase) {
                baseSideCount = it1->first->getVertexCount();
            }
        }

        //建立父子关系
        map<Polygon3D*, bool> visited;

        queue<Polygon3D*> q;
        q.push(rootPolygon3D);

        visited.insert(make_pair(rootPolygon3D, true));

        while (!q.empty()) {
            Polygon3D* parent = q.front();
            q.pop();

            map<Polygon3D*, vector<Polygon3D*>>::iterator it = relationMap.find(parent);
            if (it != relationMap.end()) {
                vector<Polygon3D*> pList = it->second;
                for (Polygon3D* child : pList) {
                    if (visited.find(child) != visited.end()) continue;

                    int* axisIds = new int[4];
                    if (child->isStickTogether(parent, axisIds)) {

                        //确定多边形需旋转的角度
                        int f0 = parent->faceType;
                        int f1 = child->faceType;

                        float targetAngle = 3.14 / 2;
                        if ((f0 == FaceType::FaceTypeBase && f1 == FaceType::FaceTypeLateralFace) ||
                            (f1 == FaceType::FaceTypeBase && f0 == FaceType::FaceTypeLateralFace) ||
                            (f1 == FaceType::FaceTypeBase && f0 == FaceType::FaceTypeBase)) {
                            targetAngle = 3.14 / 2;
                        } else if (f0 == FaceType::FaceTypeLateralFace && f1 == FaceType::FaceTypeLateralFace) {
                            int n = baseSideCount;
                            targetAngle = 3.14 * 2 / (float)n;
                        }
                        child->targetAngle = -targetAngle;

                        //确定子多边形旋转边（轴）
                        child->foldAxisIds.clear();
                        child->foldAxisIds.push_back(axisIds[0]);
                        child->foldAxisIds.push_back(axisIds[1]);

                        child->parent = parent;
                        parent->child.push_back(child);

                        visited.insert(make_pair(child, true));
                        q.push(child);
                    }
                }
            }
        }
    }

    void FoldPaperFoldLayer::setFoldPolygon() {
        if (rootPolygon3D == nullptr) return;

        std::sort(polygon3DList.begin(), polygon3DList.end(), SortNodeByLevel());

        mIter = polygon3DList.begin();

        foldingPolygon3D = nullptr;
        if (polygon3DList.size() >= 2) {
            foldingPolygon3D = polygon3DList[1];
        }
    }
}
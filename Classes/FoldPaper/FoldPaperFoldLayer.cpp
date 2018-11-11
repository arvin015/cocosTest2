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
      drawPolygon3D(nullptr) {

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

        drawPolygon3D = DrawNode3D::create();
        drawPolygon3D->setCameraMask(cc3dLayer->getCameraMask());
        cc3dLayer->addChild(drawPolygon3D);

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
                } else {
                    isFoldOver = true;
                }
            }
        }

        draw3D();
    }

    void FoldPaperFoldLayer::draw3D() {

        drawPolygon3D->clear();

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
            Vec3* vertexs = new Vec3[backPolygon3D->vertexList.size()];
            for (int i = 0; i < backPolygon3D->vertexList.size(); i++) {
                vertexs[i] = backPolygon3D->vertexList[i].position;
            }
            drawPolygon3D->drawPolygon(vertexs, backPolygon3D->vertexList.size(), Color4F::GRAY);
            delete[](vertexs);
        }

        //再绘制前面
        for (Polygon3D* frontPolygon3D : frontFaces) {
            Vec3* vertexs = new Vec3[frontPolygon3D->vertexList.size()];
            for (int i = 0; i < frontPolygon3D->vertexList.size(); i++) {
                vertexs[i] = frontPolygon3D->vertexList[i].position;
            }
            drawPolygon3D->drawPolygon(vertexs, frontPolygon3D->vertexList.size(), frontPolygon3D->polygonColor);
            delete[](vertexs);

            //绘制分割线
            if (frontPolygon3D->foldAxisIds.size() > 0) {
                drawPolygon3D->drawLine(frontPolygon3D->getVertexById(frontPolygon3D->foldAxisIds[0]).position,
                                     frontPolygon3D->getVertexById(frontPolygon3D->foldAxisIds[1]).position, Color4F::GRAY);
            }
        }
    }

    bool FoldPaperFoldLayer::onTouchBegan(Touch* touch, Event* event) {
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

            drawPolygon3D->clear();
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
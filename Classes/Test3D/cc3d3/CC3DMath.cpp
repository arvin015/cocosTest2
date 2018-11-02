#include "CC3DMath.h"

USING_NS_CC;




/* project an x,y pair onto a sphere of radius r or a
hyperbolic sheet if we are away from the center of the sphere. */
float projectToSphere(float r, float x, float y){
	float d, t, z;
	d = sqrt(x*x + y*y);
	if (d < r * 0.70710678118654752440){//inside sphere
		z = sqrt(r*r - d*d);
	}else{ //on hyperbola
		t = r / 1.41421356237309504880;
		z = t*t / d;
	}
	return z;
}

void calculateArcBall(Quaternion _quat, float _radius, cocos2d::Vec3 & axis, float & angle, float p1x, float p1y, float p2x, float p2y){
	Mat4 rotation_matrix;
	Mat4::createRotation(_quat, &rotation_matrix);

	Vec3 uv = rotation_matrix * Vec3(0.0f, 1.0f, 0.0f); //rotation y
	Vec3 sv = rotation_matrix * Vec3(1.0f, 0.0f, 0.0f); //rotation x
	Vec3 lv = rotation_matrix * Vec3(0.0f, 0.0f, -1.0f);//rotation z

	Vec3 p1 = sv * p1x + uv * p1y - lv * projectToSphere(_radius, p1x, p1y); //start point screen transform to 3d
	Vec3 p2 = sv * p2x + uv * p2y - lv * projectToSphere(_radius, p2x, p2y); //end point screen transform to 3d

	Vec3::cross(p2, p1, &axis);  //calculate rotation axis
	axis.normalize();

	float t = (p2 - p1).length() / (2.0 * _radius);
	//clamp -1 to 1
	if (t > 1.0) t = 1.0;
	if (t < -1.0) t = -1.0;
	angle = asin(t);           //rotation angle
}

void updateCameraTransform(Camera *camera, const Vec3 &orig, float distanceZ, Quaternion quat){
	Mat4 trans, rot, center;
	Mat4::createTranslation(Vec3(0.0f, 10.0f, distanceZ), &trans);
	Mat4::createRotation(quat, &rot);
	Mat4::createTranslation(orig, &center);
	Mat4 result = center * rot * trans;
	camera->setNodeToParentTransform(result);
}
/*
void Node::setRotation3D(const Vec3& rotation)
{
	if (_rotationX == rotation.x &&
		_rotationY == rotation.y &&
		_rotationZ_X == rotation.z)
		return;

	_transformUpdated = _transformDirty = _inverseDirty = true;

	_rotationX = rotation.x;
	_rotationY = rotation.y;

	// rotation Z is decomposed in 2 to simulate Skew for Flash animations
	_rotationZ_Y = _rotationZ_X = rotation.z;

	updateRotationQuat();
}
*/

Quaternion Eul2QuatGL(const Vec3 &rot) {
	// Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
	// and multiply those together.
	// the calculation below does the same, just shorter

	float yaw = rot.z;
	float pitch = rot.x;
	float roll = rot.y;
	float r = pitch * 0.5f;//float r = roll * PIOVER180 / 2.0;
	float p = roll * 0.5f;//float p = pitch * PIOVER180 / 2.0;
	float y = yaw * 0.5f;//float y = yaw * PIOVER180 / 2.0;

	float sinp = sin(p);
	float siny = sin(y);
	float sinr = sin(r);
	float cosp = cos(p);
	float cosy = cos(y);
	float cosr = cos(r);

	Quaternion q(
		sinr * cosp * cosy - cosr * sinp * siny,
		cosr * sinp * cosy + sinr * cosp * siny,
		cosr * cosp * siny - sinr * sinp * cosy,
		cosr * cosp * cosy + sinr * sinp * siny
		);
	q.normalize();
	return q;
}

Quaternion Eul2QuatCC(const Vec3 &rot){
	Quaternion _rotationQuat;
	// _rotationQuat = RotationZ * RotationY * RotationX
	const float 
		halfRadx = CC_DEGREES_TO_RADIANS(rot.x * 0.5f),
		halfRady = CC_DEGREES_TO_RADIANS(rot.y / 0.5f), 
		halfRadz = -CC_DEGREES_TO_RADIANS(rot.z / 0.5f),
		coshalfRadx = cosf(halfRadx), sinhalfRadx = sinf(halfRadx), 
		coshalfRady = cosf(halfRady), sinhalfRady = sinf(halfRady), 
		coshalfRadz = cosf(halfRadz), sinhalfRadz = sinf(halfRadz);
	_rotationQuat.x = sinhalfRadx * coshalfRady * coshalfRadz - coshalfRadx * sinhalfRady * sinhalfRadz;
	_rotationQuat.y = coshalfRadx * sinhalfRady * coshalfRadz + sinhalfRadx * coshalfRady * sinhalfRadz;
	_rotationQuat.z = coshalfRadx * coshalfRady * sinhalfRadz - sinhalfRadx * sinhalfRady * coshalfRadz;
	_rotationQuat.w = coshalfRadx * coshalfRady * coshalfRadz + sinhalfRadx * sinhalfRady * sinhalfRadz;
	return _rotationQuat;
}

Quaternion lookatQuat(Vec3 eye, Vec3 lookat, Vec3 up) {
	const Vec3 vdir = lookat - eye;
	if (vdir.lengthSquared() == 0) {
		lookat = eye + Vec3(up.z, up.y, up.x);
	} else if (vdir == up) {
		up = Vec3(up.z, up.y, up.x);
	}
	Mat4 mat4;
	lookatMatrix(eye, lookat, up, mat4);
	
	Quaternion q;
	mat4.getRotation(&q);
	return q;
}
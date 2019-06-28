#include "CC2DMath.h"

//2D Line Functions
bool lineCrossed(const Vec2 &line1a, const Vec2 &line1b, const Vec2 &line2a, const Vec2 &line2b) {
	Vec2 dir1 = line1b - line1a;
	float side2a = dir1.cross(line2a - line1a);
	float side2b = dir1.cross(line2b - line1a);
	if ((side2a * side2b) > 0)
		return false;//2a & 2b are at same side of line1
	Vec2 dir2 = line2b - line2a;
	float side1a = dir2.cross(line1a - line2a);
	float side1b = dir2.cross(line1b - line2a);
	if ((side1a * side1b) > 0)
		return false;//2a & 2b are at same side of line1

	//LOGUTIL(LEVEL_INFO, tagtag::TAG(),"line crossed: %d, %d - %d, %d <> %d, %d ~ %d, %d side:%.3f,%.3f,%.3f,%.3f",
	//	(int)line1a.x, (int)line1a.y, (int)line1b.x, (int)line1b.y, (int)line2a.x, (int)line2a.y, (int)line2b.x, (int)line2b.y,
	//	side2a, side2b, side1a, side1b);
	return true;
}

//2D Polygon Shape Functions
float shapeAreaSigned2(const std::vector<Vec2> &pts) { //for Close Shape only (last Vec overlap first)
	int count = (int)pts.size();
	if (count < 4)
		return 0.0f;
	float a = 0.0f;
	for (int i = 1; i < count; ++i) {
		a += pts[i-1].x * pts[i].y - pts[i-1].y * pts[i].x;
	}
	return a;
}

/*
std::vector<Vec2> shapeClip(const std::vector<Vec2>& subject, const std::vector<Vec2>& clipper) {
std::vector<Vec2> polygonVertices = subject;

for (size_t i = 0; i < clipper.size(); i++) {
Vec2 A = clipper[i];
Vec2 B = clipper[(i + 1) % clipper.size()];

std::vector<Vec2> temp;

for (size_t j = 0; j < polygonVertices.size(); j++) {
Vec2 polygonEdgeStart = polygonVertices[j];
Vec2 polygonEdgeEnd = polygonVertices[(j + 1) % polygonVertices.size()];

if (IsLeftOf(A, B, polygonEdgeStart)) {
if (IsLeftOf(A, B, polygonEdgeEnd)) {
temp.push_back(polygonEdgeEnd);
} else //Right
{
temp.push_back(Intersection(A, B, polygonEdgeStart, polygonEdgeEnd));
}
} else //Right
{
if (IsLeftOf(A, B, polygonEdgeEnd)) {
temp.push_back(Intersection(A, B, polygonEdgeStart, polygonEdgeEnd));
temp.push_back(polygonEdgeEnd);
} else //Right
{
// nothing to do: outside of the window
}
}
}
polygonVertices = temp;
}

polygonVertices;
}

void GeometryUtils::ClipPolygon(const std::vector<Vector2>& subject, const std::vector<Vector2>& clipper, std::vector<Vector2>& result) {
	std::vector<Vector2> polygonVertices = subject;

	for (size_t i = 0; i < clipper.size(); i++) {
		Vector2 A = clipper[i];
		Vector2 B = clipper[(i + 1) % clipper.size()];

		std::vector<Vector2> temp;

		for (size_t j = 0; j < polygonVertices.size(); j++) {
			Vector2 polygonEdgeStart = polygonVertices[j];
			Vector2 polygonEdgeEnd = polygonVertices[(j + 1) % polygonVertices.size()];

			if (IsLeftOf(A, B, polygonEdgeStart)) {
				if (IsLeftOf(A, B, polygonEdgeEnd)) {
					temp.push_back(polygonEdgeEnd);
				} else //Right
				{
					temp.push_back(Intersection(A, B, polygonEdgeStart, polygonEdgeEnd));
				}
			} else //Right
			{
				if (IsLeftOf(A, B, polygonEdgeEnd)) {
					temp.push_back(Intersection(A, B, polygonEdgeStart, polygonEdgeEnd));
					temp.push_back(polygonEdgeEnd);
				} else //Right
				{
					// nothing to do: outside of the window
				}
			}
		}
		polygonVertices = temp;
	}

	result = polygonVertices;
}

bool GeometryUtils::IsLeftOf(const Vector2& a, const Vector2& b, const Vector2& c) {
	Vector2 A = b - a;
	Vector2 B = c - a;
	return A.x * B.y - B.x * A.y < 0.0f;
}

Vector2 GeometryUtils::Intersection(const Vector2& a, const Vector2& b, const Vector2& p, const Vector2& q) {
	double A1 = b.y - a.y;
	double B1 = a.x - b.x;
	double C1 = A1 * a.x + B1 * a.y;

	double A2 = q.y - p.y;
	double B2 = p.x - q.x;
	double C2 = A2 * p.x + B2 * p.y;

	double det = A1 * B2 - A2 * B1;
	double x = (B2 * C1 - B1 * C2) / det;
	double y = (A1 * C2 - A2 * C1) / det;

	return Vector2(x, y);
}

bool GeometryUtils::IsOnLine(const Vector2& testPt, const Vector2& pt0, const Vector2& pt1, float thickness, bool infinity) {
	Vector2 U = (pt1 - pt0).normalisedCopy();
	Vector2 V = Vector2(-U.y, U.x);

	Basis2 basis2(0.5f*(pt0 + pt1), U, V);

	Vector2 localPt = basis2.ToLocal(testPt);

	if (fabs(localPt.y) > thickness)
		return false;

	return infinity ? true : fabs(localPt.x) < 0.5f * (pt1 - pt0).length();
}
*/

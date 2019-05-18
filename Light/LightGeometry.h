#pragma once
#include"LibZJ.h"
#include"LightMath.h"

class LBoundingBox
{
public:
	LVec v0, v1;
	LBoundingBox();
	LBoundingBox(LVec vec);
	bool islegal() const;
	bool isInside(LVec vec) const;
	void Update(LVec vec);
	double RayDetect(LRay ray) const;
	LBoundingBox operator + (const LBoundingBox& b) const;
	LBoundingBox& operator += (const LBoundingBox& b);
};

class LTriangle
{
public:
	LVec v0, v1, v2;
	LPlane pl;
	LTriangle();
	LTriangle(LVec _v0, LVec _v1, LVec _v2);
	bool islegal() const;
	void GetIntersection(const LRay& ray, bool* pbSuccess = NULL, LVec* pvResult = NULL, double* pdScale = NULL) const;
	bool OnTriangle(LVec v) const;
	LBoundingBox GetBoundingBox() const;
	LVec ScaleVec(LVec v) const;
	LVec CenterPoint() const;
};

class LSphere
{
public:
	LVec o;
	double radius;
	LSphere();
	LSphere(LVec _o, double _radius);
	bool islegal() const;
	void GetIntersection(const LRay& ray, bool* pbSuccess = NULL, LVec* pvResult = NULL, double* pdScale = NULL) const;
	LBoundingBox GetBoundingBox() const;
};

class LCoordinate
{
public:
	LVec x, y, z, o;
	LCoordinate();
	LCoordinate(double rx, double ry, double rz);
	void Reset();
	void Move(LVec dirta);
	void MoveRelative(LVec dirta);
	void Rotate(double rx, double ry, double rz);
	void Zoom(double scale);
	LVec PutIn(LVec v) const;
	LVec GetOut(LVec v) const;
	LRay PutIn(LRay ray) const;
	LRay GetOut(LRay ray) const;
};
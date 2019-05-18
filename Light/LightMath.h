#pragma once
#include<iostream>
#include<cstdio>
#include<cmath>
#include<assert.h>
#include <algorithm>
#include"LibZJ.h"
#include"LightLowDiscrepancySequence.h"
using namespace std;

constexpr auto L_PI = (3.14159265358979323846264338327950288419716939937510);
constexpr auto L_EPS = 1e-8;
constexpr auto L_INF = 1e100;
class LVec;
class LPlane;
class LRay;
class LLine;

class LVec
{
public:
	double x, y, z;
	LVec();
	LVec(double _x, double _y,double _z);
	LVec operator + (const LVec& b) const;
	LVec operator - () const;
	LVec operator - (const LVec& b) const;
	double operator * (const LVec& b) const;
	friend LVec operator * (const LVec& v, const double& d);
	friend LVec operator * (const double& d, const LVec& v);
	LVec operator / (const double& b) const;
	LVec operator ^ (const LVec& b) const;
	double mod() const;
	double mod2() const;
	LVec zoom(double l) const;
	LVec rotate(double rx = 0.0, double ry = 0.0, double rz = 0.0) const;
	bool iszero() const;
	bool isnan() const;
	bool isnormal() const;
	bool isfinite() const;
	bool isinf() const;
	bool operator == (const LVec& b) const;
	bool operator != (const LVec& b) const;
	bool operator < (const double& b) const;
	bool operator <= (const double& b) const;
	bool operator > (const double& b) const;
	bool operator >= (const double& b) const;
	LVec& operator += (const LVec& b);
	LVec& operator -= (const LVec& b);
	LVec& operator *= (const double& b);
	LVec& operator /= (const double& b);
	LVec& operator ^= (const LVec& b);
	bool operator || (const LVec& b) const;
	bool isvertical(const LVec& b) const;
	bool isparallel(const LVec& b) const;
	void Print() const;
};

class LPlane
{
public:
	LVec n;
	double d;
	LPlane();
	LPlane(LVec _n, double _d);
	LPlane(LVec v0, LVec v1, LVec v2);
	bool islegal() const;
	double verify(LVec v) const;
	friend LVec operator & (const LPlane& pl, const LVec& v);
	friend LVec operator & (const LVec& v, const LPlane& pl);
	friend LPlane operator | (const LPlane& pl, const LVec& v);
	friend LPlane operator | (const LVec& v, const LPlane& pl);
	bool operator || (const LPlane& b) const;
	bool operator && (const LPlane& b) const;
	friend bool operator && (const LVec& b, const LPlane& pl);
	friend bool operator && (const LPlane& pl, const LVec& b);
	friend bool operator || (const LVec& b, const LPlane& pl);
	friend bool operator || (const LPlane& pl, const LVec& b);
	bool isvertical(const LPlane& pl) const;
	bool isparallel(const LPlane& pl) const;
	bool iscross(const LPlane& pl) const;
	bool isvertical(const LVec& v) const;
	bool isparallel(const LVec& v) const;
	bool iscross(const LVec& v) const;
	void Print() const;
};

class LRay
{
public:
	LVec o, v;
	LRay();
	LRay(LVec _o, LVec _v);
	bool islegal() const;
	friend bool operator || (const LPlane& pl, const LRay& ray);
	friend bool operator || (const LRay& ray, const LPlane& pl);
	friend bool operator && (const LPlane& pl, const LRay& ray);
	friend bool operator && (const LRay& ray, const LPlane& pl);
	friend LVec operator & (const LPlane& pl, const LRay& ray);
	friend LVec operator & (const LRay& ray, const LPlane& pl);
	friend LVec operator & (const LRay& ray, const LVec& vec);
	friend LVec operator & (const LVec& vec, const LRay& ray);
	bool isvertical(const LPlane& pl) const;
	bool isparallel(const LPlane& pl) const;
	bool iscross(const LPlane& pl) const;
	LVec _vec() const;
	LLine _line() const;
};

class LLine
{
public:
	LVec v0, v1;
	LLine();
	LLine(LVec _v0, LVec _v1);
	LLine(LRay ray);
	bool islegal() const;
	friend bool operator || (const LPlane& pl, const LLine& line);
	friend bool operator || (const LLine& line, const LPlane& pl);
	friend bool operator && (const LPlane& pl, const LLine& line);
	friend bool operator && (const LLine& line, const LPlane& pl);
	friend LVec operator & (const LPlane& pl, const LLine& line);
	friend LVec operator & (const LLine& line, const LPlane& pl);
	bool isvertical(const LPlane& pl) const;
	bool isparallel(const LPlane& pl) const;
	bool iscross(const LPlane& pl) const;
	LVec _vec() const;
	LRay _ray() const;
};

double diff(double a, double b);

double iszero(double d);

void rotate(double& x, double& y, double rad);

double angle(LVec v0, LVec v1);

void GetIntersection(const LPlane& pl, const LRay& ray, bool* pbSuccess = NULL, LVec* pvResult = NULL, double* pdScale = NULL);

LVec RandInSphere(RandomDevice& randdevice);

LVec RandOnSphere(RandomDevice& randdevice);

LVec RandOnSphere(unsigned& index);

LVec RandOnSphereCrown(RandomDevice& randdevice, double costheta);

LVec RandOnSphereCrown(RandomDevice& randdevice, double costheta, LVec centual);

LVec RandOnSphereCrown(unsigned& index, double costheta);

LVec RandOnSphereCrown(unsigned& index, double costheta, LVec centual);

LVec BindVec(LVec centual, LVec v);


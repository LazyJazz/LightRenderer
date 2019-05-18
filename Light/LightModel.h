#pragma once
#include"LightMaterial.h"
#include"LightMedium.h"
#include<vector>
using namespace std;


class LModel
{
public:
	LMedium* medium = NULL;
	virtual bool GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const = 0;
	void SetMedium(LMedium* pMedium);
	virtual void GetMedium(LRay iray, double time, LMedium** pMedium) const;
};

class LModelTriangle :public LModel
{
public:
	LTriangle triangle;
	LMaterial* material;
	LModelTriangle();
	LModelTriangle(const LTriangle& tri, LMaterial* pMaterial);
	bool GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const;
};

class LModelSphere :public LModel
{
public:
	LSphere sphere;
	LMaterial* material;
	LModelSphere();
	LModelSphere(const LSphere& sph, LMaterial* pMaterial);
	bool GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const;
	void GetMedium(LRay iray, double time, LMedium** pMedium) const;
};

class LModelSet :public LModel
{
public:
	vector<LModel*> vecModels;
	LModelSet();
	bool GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const;
	void AddModel(LModel* pModel);
	void GetMedium(LRay iray, double time, LMedium** pMedium) const;
};

class LModelShift :public LModel
{
public:
	LVec v0, v1;
	LModel* model;
	LModelShift();
	LModelShift(LVec _v0, LVec _v1, LModel* _model);
	bool GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const;
	void GetMedium(LRay iray, double time, LMedium** pMedium) const;
};

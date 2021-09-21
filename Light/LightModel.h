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
	virtual LBoundingBox GetBoundingBox() const = 0;
};

class LModelTriangle :public LModel
{
public:
	LTriangle triangle;
	LMaterial* material;
	LModelTriangle();
	LModelTriangle(const LTriangle& tri, LMaterial* pMaterial);
	bool GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const;
	virtual LBoundingBox GetBoundingBox() const;
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
	virtual LBoundingBox GetBoundingBox() const;
};

class LModelSet :public LModel
{
public:
	vector<LModel*> vecModels;
	LBoundingBox bbTotal;
	LModelSet();
	bool GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const;
	void AddModel(LModel* pModel);
	void GetMedium(LRay iray, double time, LMedium** pMedium) const;
	virtual LBoundingBox GetBoundingBox() const;
};

class LModelKDTreeNode
{
public:
	LModel* pModel = NULL;
	LVec pos = LVec();
	LBoundingBox bounding_box = LBoundingBox();
	size_t ch[8] = {};
};

class LModelKDTree : public LModel
{
public:
	size_t uRoot = 0, uModelCnt = 0;
	vector<LModelKDTreeNode> vecModelKDTree;
	bool GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const;
	void AddModel(LModel* pModel);
	//void GetMedium(LRay iray, double time, LMedium** pMedium) const;
	virtual LBoundingBox GetBoundingBox() const;
	void Build();
private:
	void BuildKDTree(size_t& x, vector<size_t> vecFaceNode);
	bool GetIntersection(size_t x, const LRay& iray, const double& time, LSurfaceInfo* pSurfaceInfo) const;
	size_t RelativeIndex(LVec o, LVec p) const;
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
	virtual LBoundingBox GetBoundingBox() const;
};

class LModelTransform :public LModel
{
public:
	LCoordinate coordinate;
	LModel* model;
	LModelTransform();
	LModelTransform(LCoordinate cdn, LModel* pModel);
	bool GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const;
	void GetMedium(LRay iray, double time, LMedium** pMedium) const;
	virtual LBoundingBox GetBoundingBox() const;
};
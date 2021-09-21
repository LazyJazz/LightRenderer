#pragma once
#include"LightGeometry.h"
#include"LightModel.h"
#include<vector>
#include<sstream>
#include<fstream>
#include<sstream>
#include<algorithm>

class LMeshFace
{
public:
	UINT index_vertex[3];
	UINT index_normal[3];
	UINT index_texture[3];
	LTriangle triangle;
	LVec pos;
	LBoundingBox bounding_box;
	size_t ch[8];
	LMeshFace();
	LMeshFace(string index0, string index1, string index2);
	void TranslateIndex(string index, UINT& vertex, UINT& normal, UINT& texture);
};

class LIntersectionPack
{
public:
	bool success = false;
	double distance = 0.0;
	LVec intersection = LVec();
	LVec normal = LVec();
	LVec model_normal = LVec();
	LVec texture = LVec();
};
//#define INTERSECTION_COUNTER
class LMesh
{
public:
	size_t uRoot = 0, uVertexCnt = 0, uNormalCnt = 0, uTextureCnt = 0, uFaceCnt = 0;
	vector<LVec> vecMeshVertex;
	vector<LVec> vecMeshNormal;
	vector<LVec> vecMeshTexture;
	vector<LMeshFace> vecMeshFace;
	LBoundingBox bounding_box;
	void LoadObjFile(const char* filename, size_t no = 0);
	void GetIntersection(const LRay &iray, LIntersectionPack* pResult) const;
	void GetIntersectionCnt(const LRay& iray, UINT* cnt) const;
	void Move(LVec dirta);
	void Rotate(double rx, double ry, double rz);
	void Zoom(double scale);
	void Normalize(double size = 1.0);
	LVec CenterPoint() const;
	void Build();
private:
	void BuildKDTree(size_t& x, vector<size_t> vecFaceNode);
	void GetIntersection(size_t x, const LRay& iray, LIntersectionPack* pResult) const;
	void GetIntersectionCnt(size_t x, const LRay& iray, UINT* cnt) const;
	size_t RelativeIndex(LVec o, LVec p) const;
};

class LModelMesh :public LModel
{
public:
	LMesh* mesh;
	LMaterial* material;
	LModelMesh();
	LModelMesh(LMesh* pMesh, LMaterial* pMaterial);
	bool GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const;
	void GetMedium(LRay iray, double time, LMedium** pMedium) const;
	virtual LBoundingBox GetBoundingBox() const;
};

UINT GetIntersectionCounter();

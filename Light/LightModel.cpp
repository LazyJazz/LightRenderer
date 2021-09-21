#include "LightModel.h"
#include "LightMaterial.h"
#include "LightMedium.h"

LModelTriangle::LModelTriangle()
{
	triangle = LTriangle();
	material = NULL;
}

LModelTriangle::LModelTriangle(const LTriangle &tri, LMaterial* pMaterial)
{
	triangle = tri;
	material = pMaterial;
}

bool LModelTriangle::GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const
{
	bool res;
	triangle.GetIntersection(iray, &res, &pSurfaceInfo->intersection, &pSurfaceInfo->distance);
	if (res)
	{
		pSurfaceInfo->material = material;
		pSurfaceInfo->normal = triangle.pl.n;
		if (pSurfaceInfo->normal * iray.v > 0)pSurfaceInfo->normal = -pSurfaceInfo->normal;
		pSurfaceInfo->model_normal = pSurfaceInfo->normal;
		pSurfaceInfo->ray = iray;
		pSurfaceInfo->transformed_pos = LVec();
	}
	return res;
}

LBoundingBox LModelTriangle::GetBoundingBox() const
{
	return triangle.GetBoundingBox();
}

LModelSet::LModelSet()
{
	vecModels.clear();
	bbTotal = LBoundingBox();
}

bool LModelSet::GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const
{
	bool res = 0;
	if (pSurfaceInfo->material)res = true;
	LSurfaceInfo surface_info;
	for (LModel* pModel : vecModels)
	{
		bool tres = pModel->GetIntersection(iray, time, &surface_info);
		if (surface_info.distance < L_EPS)continue;
		if (tres && (!res || surface_info.distance < pSurfaceInfo->distance))
			res = true,
			*pSurfaceInfo = surface_info;
	}
	return res;
}

void LModelSet::AddModel(LModel* pModel)
{
	vecModels.push_back(pModel);
	bbTotal += pModel->GetBoundingBox();
}

void LModelSet::GetMedium(LRay iray, double time, LMedium** pMedium) const
{
	*pMedium = medium;
	for (LModel* pModel : vecModels)
	{
		LMedium* tmedium = NULL;
		pModel->GetMedium(iray, time, &tmedium);
		if (tmedium)*pMedium = tmedium;
	}
}

LBoundingBox LModelSet::GetBoundingBox() const
{
	return bbTotal;
}

LModelSphere::LModelSphere()
{
	sphere = LSphere();
	material = NULL;
}

LModelSphere::LModelSphere(const LSphere& sph, LMaterial* pMaterial)
{
	sphere = sph;
	material = pMaterial;
}

bool LModelSphere::GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const
{
	bool res;
	sphere.GetIntersection(iray, &res, &pSurfaceInfo->intersection, &pSurfaceInfo->distance);
	if (res)
	{
		pSurfaceInfo->material = material;
		pSurfaceInfo->normal = (pSurfaceInfo->intersection - sphere.o) / sphere.radius;
		if (pSurfaceInfo->normal * iray.v > 0)pSurfaceInfo->normal = -pSurfaceInfo->normal;
		pSurfaceInfo->model_normal = pSurfaceInfo->normal;
		pSurfaceInfo->ray = iray;
		pSurfaceInfo->transformed_pos = LVec();
	}
	return res;
}

void LModelSphere::GetMedium(LRay iray, double time, LMedium** pMedium) const
{
	*pMedium = NULL;
	if ((sphere.o - iray.o) < sphere.radius - L_EPS) * pMedium = medium;
	else if ((sphere.o - iray.o) < sphere.radius + L_EPS)
		if (iray.v * (iray.o - sphere.o) < 0.0) * pMedium = medium;
}

LBoundingBox LModelSphere::GetBoundingBox() const
{
	return sphere.GetBoundingBox();
}

void LModel::SetMedium(LMedium* pMedium)
{
	medium = pMedium;
}

void LModel::GetMedium(LRay iray, double time, LMedium** pMedium) const
{
	*pMedium = NULL;
}

LModelShift::LModelShift()
{
	v0 = LVec();
	v1 = LVec();
	model = NULL;
}

LModelShift::LModelShift(LVec _v0, LVec _v1, LModel* _model)
{
	v0 = _v0;
	v1 = _v1;
	model = _model;
}

bool LModelShift::GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const
{
	LVec v = v0 * (1.0 - time) + v1 * time;
	iray.o -= v;
	bool ret = model->GetIntersection(iray, time, pSurfaceInfo);
	pSurfaceInfo->ray.o += v;
	pSurfaceInfo->intersection += v;
	return ret;
}

void LModelShift::GetMedium(LRay iray, double time, LMedium** pMedium) const
{
	LVec v = v0 * (1.0 - time) + v1 * time;
	iray.o -= v;
	model->GetMedium(iray, time, pMedium);
}

LBoundingBox LModelShift::GetBoundingBox() const
{
	LBoundingBox bb = model->GetBoundingBox();
	return LBoundingBox(bb.v0 + v0) + LBoundingBox(bb.v1 + v0) + LBoundingBox(bb.v0 + v1) + LBoundingBox(bb.v1 + v1);
}

bool LModelKDTree::GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const
{
	bool res = false;
	pSurfaceInfo->distance = L_INF * 0.5;
	res = GetIntersection(uRoot, iray, time, pSurfaceInfo);
	return res;
}

void LModelKDTree::AddModel(LModel* pModel)
{
	if (vecModelKDTree.empty())
	{
		vecModelKDTree.push_back(LModelKDTreeNode());
	}
	LModelKDTreeNode node;
	node.pModel = pModel;
	node.bounding_box = pModel->GetBoundingBox();
	node.pos = (node.bounding_box.v0 + node.bounding_box.v1) * 0.5;
	vecModelKDTree.push_back(node);
	uModelCnt++;
}

LBoundingBox LModelKDTree::GetBoundingBox() const
{
	if (!uRoot)return LBoundingBox();
	return vecModelKDTree[uRoot].bounding_box;
}

void LModelKDTree::Build()
{
	uRoot = 0;
	for (auto& model : vecModelKDTree)
	{
		memset(model.ch, 0, sizeof model.ch);
	}
	vector<size_t> face_nodes;
	face_nodes.resize(uModelCnt);
	for (size_t i = 0; i < uModelCnt; i++)face_nodes[i] = i + 1;
	BuildKDTree(uRoot, face_nodes);
}

void LModelKDTree::BuildKDTree(size_t& x, vector<size_t> vecFaceNode)
{
	x = 0;
	if (vecFaceNode.empty())return;
	vector<size_t> divide[8];
	vector<pair<size_t, size_t>> score;
	score.resize(vecFaceNode.size());
	for (size_t i = 0; i < vecFaceNode.size(); i++)score[i] = make_pair(0, vecFaceNode[i]);
	int center = vecFaceNode.size() / 2;
	//sort(vecFaceNode.begin(), vecFaceNode.end(), [this](size_t u, size_t v)->bool {return this->vecMeshFace[u].pos.x < this->vecMeshFace[v].pos.x; });
	sort(score.begin(), score.end(), [this](pair<size_t, size_t> u, pair<size_t, size_t> v)->bool {return this->vecModelKDTree[u.second].pos.x < this->vecModelKDTree[v.second].pos.x; });
	for (int i = 0; i < vecFaceNode.size(); i++)score[i].first += abs(i - center);
	//sort(vecFaceNode.begin(), vecFaceNode.end(), [this](size_t u, size_t v)->bool {return this->vecMeshFace[u].pos.y < this->vecMeshFace[v].pos.y; });
	sort(score.begin(), score.end(), [this](pair<size_t, size_t> u, pair<size_t, size_t> v)->bool {return this->vecModelKDTree[u.second].pos.y < this->vecModelKDTree[v.second].pos.y; });
	for (int i = 0; i < vecFaceNode.size(); i++)score[i].first += abs(i - center);
	//sort(vecFaceNode.begin(), vecFaceNode.end(), [this](size_t u, size_t v)->bool {return this->vecMeshFace[u].pos.z < this->vecMeshFace[v].pos.z; });
	sort(score.begin(), score.end(), [this](pair<size_t, size_t> u, pair<size_t, size_t> v)->bool {return this->vecModelKDTree[u.second].pos.z < this->vecModelKDTree[v.second].pos.z; });
	for (int i = 0; i < vecFaceNode.size(); i++)score[i].first += abs(i - center);
	sort(score.begin(), score.end());
	x = score[0].second;
	for (size_t i = 1; i < score.size(); i++)
	{
		size_t node = score[i].second;
		divide[RelativeIndex(vecModelKDTree[x].pos, vecModelKDTree[node].pos)].push_back(node);
	}
	for (size_t direct = 0; direct < 8; direct++)
	{
		BuildKDTree(vecModelKDTree[x].ch[direct], divide[direct]);
		if (vecModelKDTree[x].ch[direct])vecModelKDTree[x].bounding_box += vecModelKDTree[vecModelKDTree[x].ch[direct]].bounding_box;
	}
}

bool LModelKDTree::GetIntersection(size_t x, const LRay& iray, const double& time, LSurfaceInfo* pSurfaceInfo) const
{
	if (!x)return false;
	if (vecModelKDTree[x].bounding_box.RayDetect(iray) >= pSurfaceInfo->distance)return false;
	bool res = 0;
	LSurfaceInfo surface_info;
	bool tres = vecModelKDTree[x].pModel->GetIntersection(iray, time, &surface_info);
	if (tres)
	{
		if (surface_info.distance >= L_EPS)
		{
			if (surface_info.distance < pSurfaceInfo->distance)
			{
				*pSurfaceInfo = surface_info;
				res = true;
			}
		}
	}
	size_t direct = RelativeIndex(vecModelKDTree[x].pos, iray.o);
	res |= GetIntersection(vecModelKDTree[x].ch[direct], iray, time, pSurfaceInfo);
	res |= GetIntersection(vecModelKDTree[x].ch[direct ^ 1], iray, time, pSurfaceInfo);
	res |= GetIntersection(vecModelKDTree[x].ch[direct ^ 2], iray, time, pSurfaceInfo);
	res |= GetIntersection(vecModelKDTree[x].ch[direct ^ 4], iray, time, pSurfaceInfo);
	res |= GetIntersection(vecModelKDTree[x].ch[direct ^ 6], iray, time, pSurfaceInfo);
	res |= GetIntersection(vecModelKDTree[x].ch[direct ^ 5], iray, time, pSurfaceInfo);
	res |= GetIntersection(vecModelKDTree[x].ch[direct ^ 3], iray, time, pSurfaceInfo);
	res |= GetIntersection(vecModelKDTree[x].ch[direct ^ 7], iray, time, pSurfaceInfo);
	return res;
}

size_t LModelKDTree::RelativeIndex(LVec o, LVec p) const
{
	size_t res = 0;
	if (p.x > o.x)res |= 1;
	if (p.y > o.y)res |= 2;
	if (p.z > o.z)res |= 4;
	return res;
}

LModelTransform::LModelTransform()
{
	coordinate = LCoordinate();
	model = NULL;
}

LModelTransform::LModelTransform(LCoordinate cdn, LModel* pModel)
{
	coordinate = cdn;
	model = pModel;
}

bool LModelTransform::GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const
{
	//LRay tray = LRay(coordinate.PutIn(iray.o), coordinate.PutIn(iray.o + iray.v) - coordinate.PutIn(iray.o));
	bool res = model->GetIntersection(coordinate.GetOut(iray), time, pSurfaceInfo);
	if (res)
	{
		pSurfaceInfo->normal = coordinate.PutIn(pSurfaceInfo->intersection + pSurfaceInfo->normal);
		pSurfaceInfo->model_normal = coordinate.PutIn(pSurfaceInfo->intersection + pSurfaceInfo->model_normal);
		pSurfaceInfo->intersection = coordinate.PutIn(pSurfaceInfo->intersection);
		pSurfaceInfo->normal -= pSurfaceInfo->intersection; pSurfaceInfo->normal = pSurfaceInfo->normal.zoom(1.0);
		pSurfaceInfo->model_normal -= pSurfaceInfo->intersection; pSurfaceInfo->model_normal = pSurfaceInfo->model_normal.zoom(1.0);
		pSurfaceInfo->distance = (pSurfaceInfo->intersection - iray.o).mod();
		pSurfaceInfo->ray = coordinate.PutIn(pSurfaceInfo->ray);
	}
	return res;
}

void LModelTransform::GetMedium(LRay iray, double time, LMedium** pMedium) const
{
	model->GetMedium(coordinate.PutIn(iray), time, pMedium);
}

LBoundingBox LModelTransform::GetBoundingBox() const
{
	LBoundingBox res = model->GetBoundingBox();
	LVec v[2] = { res.v0,res.v1 };
	res = LBoundingBox();
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				/*LVec tv = LVec(v[i].x, v[j].y, v[k].z);
				tv = coordinate.PutIn(tv);//*/
				res.Update(coordinate.PutIn(LVec(v[i].x, v[j].y, v[k].z)));
			}
	return res;
}

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

LModelSet::LModelSet()
{
	vecModels.clear();
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

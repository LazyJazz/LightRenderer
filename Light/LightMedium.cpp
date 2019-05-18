#include "LightModel.h"
#include "LightMaterial.h"
#include "LightMedium.h"

double LMedium::GetRefractionIndex() const
{
	return 1.0;
}

bool LMedium::GetParticipation(LRay iray, double time, RandomDevice& randdevice, unsigned** ppIndexSeq, LSurfaceInfo* pSurfaceInfo) const
{
	return false;
}

void LMedium::GetFilter(const LSurfaceInfo* pSurfaceInfo, LColor* pFilter, double* pScale)const
{
	;
}

void LMediumNonParticipating::SetRefractionIndex(double ri)
{
	refraction_index = ri;
}

void LMediumNonParticipating::SetAttenuation(LColor att)
{
	attenuation = att;
}

double LMediumNonParticipating::GetRefractionIndex() const
{
	return refraction_index;
}

void LMediumNonParticipating::GetFilter(const LSurfaceInfo* pSurfaceInfo, LColor* pFilter, double* pScale)const
{
	pFilter->r *= pow(attenuation.r, pSurfaceInfo->distance);
	pFilter->g *= pow(attenuation.g, pSurfaceInfo->distance);
	pFilter->b *= pow(attenuation.b, pSurfaceInfo->distance);
}

LMedium* g_pMedium = new LMedium();

void SetDefault(LMedium* pMedium)
{
	g_pMedium = pMedium;
}

void GetDefaultMedium(LMedium** ppMedium)
{
	*ppMedium = g_pMedium;
}

LMediumParticipating::LMediumParticipating()
{
	absorb = 0.0;
	mat_impurity = LMaterialSpecularSurfacePureColor(LColor(0.0, 0.0, 0.0));
}

LMediumParticipating::LMediumParticipating(double _absorb, LColor _color)
{
	absorb = _absorb;
	mat_impurity = LMaterialSpecularSurfacePureColor(_color);
}

bool LMediumParticipating::GetParticipation(LRay iray, double time, RandomDevice& randdevice, unsigned** ppIndexSeq, LSurfaceInfo* pSurfaceInfo) const
{
	LVec v;
	double prob;
	v.y = 0.0;
	do
	{
		//if (*ppIndexSeq)(*ppIndexSeq)[31]++,prob = Halton(3, (*ppIndexSeq)[31]), v.x = 1.0 - 2.0 * Halton(4, (*ppIndexSeq)[31]), v.z = 1.0 - 2.0 * Halton(5, (*ppIndexSeq)[31]);
		//else 
		prob = randdevice.rand_double(0.0, 1.0), v.x = randdevice.rand_double(-1.0, 1.0), v.z = randdevice.rand_double(-1.0, 1.0);
	} while (v.x* v.x + v.z * v.z > 1.0 || prob == 0.0);
	v.y = sqrt(1.0 - v.x * v.x - v.z * v.z);
	if (prob > 0.0)
	{
		pSurfaceInfo->distance = log(prob) / log(1.0 - absorb);
		pSurfaceInfo->intersection = iray.o + iray.v * pSurfaceInfo->distance;
		pSurfaceInfo->ray = iray;
		pSurfaceInfo->time = time;
		pSurfaceInfo->transformed_pos = LVec();
		pSurfaceInfo->normal = BindVec(-iray.v, v);
		pSurfaceInfo->material = &mat_impurity;
		pSurfaceInfo->model_normal = pSurfaceInfo->normal;
	}
	else return false;
	return true;
}

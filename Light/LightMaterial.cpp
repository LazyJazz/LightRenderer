#include "LightModel.h"
#include "LightMaterial.h"
#include "LightMedium.h"

void LMaterialReflectAny::Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const
{
	InteractionReflectAny(surface, oray, randdevice, ppIndexSeq, Floor);
	Interaction(surface, *oray, Scale, pFilter, randdevice);
	if (oray->v * surface.model_normal < 0.0)
		*pFilter = LColor();
}

void LMaterialLambertian::Interaction(const LSurfaceInfo& surface, LRay outray, double* Scale, LColor* pFilter, RandomDevice& randdevice) const
{
	*Scale = 1.0;
	*pFilter = GetColor(surface);
	*pFilter = *pFilter * (surface.normal * outray.v);
}

LMaterialLambertianPureColor::LMaterialLambertianPureColor()
{
	color = LColor();
}

LMaterialLambertianPureColor::LMaterialLambertianPureColor(LColor color)
{
	this->color = color;
}

void LMaterialLambertianPureColor::SetColor(LColor color)
{
	this->color = color;
}

LColor LMaterialLambertianPureColor::GetColor(const LSurfaceInfo& surface) const
{
	return color;
}

void LMaterialLightSource::Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const
{
	InteractionLightSource(surface, oray);
	*Scale = 1.0;
	*pFilter = GetColor(surface);
}

LMaterialLightSourcePureColor::LMaterialLightSourcePureColor()
{
	color = LColor();
}

LMaterialLightSourcePureColor::LMaterialLightSourcePureColor(LColor color)
{
	this->color = color;
}

void LMaterialLightSourcePureColor::SetColor(LColor color)
{
	this->color = color;
}

LColor LMaterialLightSourcePureColor::GetColor(const LSurfaceInfo& surface) const
{
	return color;
}

void LMaterialSpecularSurface::Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const
{
	InteractionSpecular(surface, surface.normal, oray);
	/*oray->v = surface.intersection + surface.normal * (surface.normal * (surface.ray.o - surface.intersection));
	oray->v = oray->v * 2 - surface.ray.o;
	oray->v -= surface.intersection;
	oray->v = oray->v.zoom(1.0);//*/
	*Scale = 1.0;
	if (oray->v * surface.model_normal < 0.0)
		* pFilter = LColor();
	else *pFilter = GetColor(surface);
}

LMaterialSpecularSurfacePureColor::LMaterialSpecularSurfacePureColor()
{
	color = LColor();
}

LMaterialSpecularSurfacePureColor::LMaterialSpecularSurfacePureColor(LColor color)
{
	this->color = color;
}

void LMaterialSpecularSurfacePureColor::SetColor(LColor color)
{
	this->color = color;
}

LColor LMaterialSpecularSurfacePureColor::GetColor(const LSurfaceInfo& surface) const
{
	return color;
}

void InteractionSpecular(const LSurfaceInfo& surface, LVec real_normal, LRay* oray)
{
	oray->o = surface.intersection;
	oray->v = surface.ray.v + real_normal * ((surface.ray.v * real_normal) * -2);
}

void InteractionRefraction(const LSurfaceInfo& surface, LVec real_normal, LRay* oray, LColor* pFilter, RandomDevice& randdevice, UINT Floor)
{
	LRay ray;
	double ri0, ri1;
	ray.o = surface.intersection;
	ray.v = real_normal;
	LMedium* pMed[2];
	surface.model->GetMedium(ray, surface.time, &pMed[0]);
	if (!pMed[0])GetDefaultMedium(&pMed[0]);
	ray.v = -ray.v;
	surface.model->GetMedium(ray, surface.time, &pMed[1]);
	if (!pMed[1])GetDefaultMedium(&pMed[1]);
	ri0 = pMed[0]->GetRefractionIndex();
	ri1 = pMed[1]->GetRefractionIndex();
	LVec mid = ray & surface.ray.o;
	double sin0 = (mid - surface.ray.o).mod() / (surface.ray.o - surface.intersection).mod(), sin1 = sin0 * ri0 / ri1;
	double i0 = asin(sin0), i1;
	double ref_possib;
	*pFilter = LColor(1.0, 1.0, 1.0);
	/*if (!Floor)
	{
		printf("%lf %.100lf\n", ri1, surface.intersection.z);
		real_normal.Print(); puts("");
		if (ri1 == 1.0)
		{
			printf("%lf %.100lf\n", ri1, surface.intersection.z);
			printf("Intersection:"); surface.intersection.Print(); puts("");
			printf("Normal:"); surface.normal.Print(); puts("");
			printf("Model Normal:"); surface.model_normal.Print(); puts("");
		}
	}//*/
	if (sin1 > 1.0)
	{
		ref_possib = 1.0;
	}
	else
	{
		i1 = asin(sin1);
		double rs = -sin(i0 - i1) / sin(i0 + i1), rp = tan(i0 - i1) / tan(i0 + i1);
		ref_possib = (rs * rs + rp * rp) * 0.5;
	}
	oray->o = surface.intersection;
	if (randdevice.rand_double() <= ref_possib)
	{
		oray->v = surface.ray.v + real_normal * ((surface.ray.v * real_normal) * -2);
	}
	else
	{
		double cos1 = cos(i1), tan1 = sin1 / cos1;
		oray->v = -real_normal * cos1 + (mid - surface.ray.o).zoom(sin1);
	}
}

void InteractionReflectAny(const LSurfaceInfo& surface, LRay* oray, RandomDevice& randdevice, unsigned** ppIndexSeq, UINT Floor)
{
	oray->o = surface.intersection;
	if (*ppIndexSeq)
	{
		do
		{
			oray->v = RandOnSphere((*ppIndexSeq)[Floor]);
		} while (oray->v* surface.normal < 0.0);
		(*ppIndexSeq) = 0;
	}
	else
	{
		do
		{
			oray->v = RandOnSphere(randdevice);
		} while (oray->v * surface.normal < 0.0);
	}
}

void InteractionLightSource(const LSurfaceInfo& surface, LRay* oray)
{
	oray->o = LVec();
	oray->v = LVec();
}

LMaterialLambertianTexture::LMaterialLambertianTexture()
{
	scale = 1.0;
}

void LMaterialLambertianTexture::SetTexture(PCWSTR filepath, double scale)
{
	img.LoadFromFile(filepath);
	img.Lock();
	this->scale = scale;
}

LColor LMaterialLambertianTexture::GetColor(const LSurfaceInfo& surface) const
{
	long long x = img.GetWidth(), y = img.GetHeight();
	x = max(min((long long)floor(x * surface.transformed_pos.x), x - 1), 0);
	y = max(min((long long)floor(y * (1.0 - surface.transformed_pos.y)), y - 1), 0);
	DWORD color = img.GetPixel(x, y);
	return LColor(((color >> 16) & 255) * (1.0 / 255.0) * scale, ((color >> 8) & 255) * (1.0 / 255.0) * scale, (color & 255) * (1.0 / 255.0) * scale);
}

LMaterialSpecularSurfaceTexture::LMaterialSpecularSurfaceTexture()
{
	scale = 1.0;
}

void LMaterialSpecularSurfaceTexture::SetTexture(PCWSTR filepath, double scale)
{
	img.LoadFromFile(filepath);
	img.Lock();
	this->scale = scale;
}

LColor LMaterialSpecularSurfaceTexture::GetColor(const LSurfaceInfo& surface) const
{
	long long x = img.GetWidth(), y = img.GetHeight();
	x = max(min((long long)floor(x * surface.transformed_pos.x), x - 1), 0);
	y = max(min((long long)floor(y * (1.0 - surface.transformed_pos.y)), y - 1), 0);
	DWORD color = img.GetPixel(x, y);
	return LColor(((color >> 16) & 255) * (1.0 / 255.0) * scale, ((color >> 8) & 255) * (1.0 / 255.0) * scale, (color & 255) * (1.0 / 255.0) * scale);
}

LMaterialLightSourceTexture::LMaterialLightSourceTexture()
{
	scale = 1.0;
}

void LMaterialLightSourceTexture::SetTexture(PCWSTR filepath, double scale)
{
	img.LoadFromFile(filepath);
	img.Lock();
	this->scale = scale;
}

LColor LMaterialLightSourceTexture::GetColor(const LSurfaceInfo& surface) const
{
	long long x = img.GetWidth(), y = img.GetHeight();
	x = max(min((long long)floor(x * surface.transformed_pos.x), x - 1), 0);
	y = max(min((long long)floor(y * (1.0 - surface.transformed_pos.y)), y - 1), 0);
	DWORD color = img.GetPixel(x, y);
	return LColor(((color >> 16) & 255) * (1.0 / 255.0) * scale, ((color >> 8) & 255) * (1.0 / 255.0) * scale, (color & 255) * (1.0 / 255.0) * scale);
}

LMaterialMixture::LMaterialMixture()
{
	mat[0] = mat[1] = NULL;
	scale[0] = scale[1] = 0.0;
}

LMaterialMixture::LMaterialMixture(LMaterial* pMat0, double scale0, LMaterial* pMat1, double scale1)
{
	mat[0] = pMat0;
	mat[1] = pMat1;
	scale[0] = scale0;
	scale[1] = scale1;
}

void LMaterialMixture::Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const
{
	if (randdevice.rand_double(0.0, scale[0] + scale[1]) < scale[0])
		mat[0]->Interaction(surface, oray, Scale, pFilter, randdevice, ppIndexSeq, pReserved, Floor);
	else
		mat[1]->Interaction(surface, oray, Scale, pFilter, randdevice, ppIndexSeq, pReserved, Floor);
	*pFilter = *pFilter * (scale[0] + scale[1]);
}

void LMaterialRefractionSurface::Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const
{
	//InteractionSpecular(surface, surface.normal, oray);
	InteractionRefraction(surface, surface.normal, oray, pFilter, randdevice, Floor);
	*Scale = 1.0;
	*pFilter = *pFilter * GetColor(surface);
}

LMaterialRefractionSurfacePureColor::LMaterialRefractionSurfacePureColor()
{
	color = LColor();
}

LMaterialRefractionSurfacePureColor::LMaterialRefractionSurfacePureColor(LColor color)
{
	this->color = color;
}

void LMaterialRefractionSurfacePureColor::SetColor(LColor color)
{
	this->color = color;
}

LColor LMaterialRefractionSurfacePureColor::GetColor(const LSurfaceInfo& surface) const
{
	return color;
}

LMaterialGlossySurface::LMaterialGlossySurface()
{
	color = LColor();
	glossy = 0.0;
}

LMaterialGlossySurface::LMaterialGlossySurface(LColor color, double glossy)
{
	this->color = color;
	this->glossy = glossy;
}

void LMaterialGlossySurface::SetColor(LColor color)
{
	this->color = color;
}

void LMaterialGlossySurface::Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const
{
	InteractionSpecular(surface, RandOnSphereCrown(randdevice, 1.0 - glossy, surface.normal), oray);
	/*oray->v = surface.intersection + surface.normal * (surface.normal * (surface.ray.o - surface.intersection));
	oray->v = oray->v * 2 - surface.ray.o;
	oray->v -= surface.intersection;
	oray->v = oray->v.zoom(1.0);//*/
	*Scale = 1.0;
	if (oray->v * surface.model_normal < 0.0)
		* pFilter = LColor();
	else *pFilter = color;
}

void LMaterialDisneyDiffuse::Interaction(const LSurfaceInfo& surface, LRay outray, double* Scale, LColor* pFilter, RandomDevice& randdevice) const
{
	*pFilter = color;
	*pFilter = *pFilter * (surface.normal * outray.v);

	double oneMinusCosL = 1.0 - abs(surface.ray.v.z);
	double oneMinusCosLSqr = oneMinusCosL * oneMinusCosL;
	double oneMinusCosV = 1.0 - abs(outray.v.z);
	double oneMinusCosVSqr = oneMinusCosV * oneMinusCosV;

	// Roughness是粗糙度，IDotH的意思会在下一篇讲Microfacet模型时提到
	LVec normal = -surface.ray.v + outray.v;
	if (normal.iszero())normal = surface.normal;
	else normal = normal.zoom(1.0);
	double IDotH = -surface.ray.v * normal;
	//if(IDotH)
	double F_D90 = 0.5 + 2.0 * IDotH * IDotH * Roughness;

	*Scale = (1.0 + (F_D90 - 1.0) * oneMinusCosLSqr * oneMinusCosLSqr * oneMinusCosL) *
		(1.0 + (F_D90 - 1.0) * oneMinusCosVSqr * oneMinusCosVSqr * oneMinusCosV);
}

void LMaterialDisneyDiffuse::SetColor(LColor color)
{
	this->color = color;
}

void LMaterialDisneyDiffuse::SetRoughness(double Roughness)
{
	this->Roughness = Roughness;
}

void LMaterialPaper::Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const
{
	oray->o = surface.intersection;
	if (*ppIndexSeq)
	{
		do
		{
			oray->v = RandOnSphere((*ppIndexSeq)[Floor]);
		} while (oray->v * surface.normal == 0.0);
		(*ppIndexSeq) = 0;
	}
	else
	{
		do
		{
			oray->v = RandOnSphere(randdevice);
		} while (oray->v * surface.normal == 0.0);
	}
	*Scale = 1.0;
	if(oray->v * surface.normal>0.0)*pFilter = color * abs(oray->v * surface.normal);
	else
	{
		double p = acos(surface.ray.v * oray->v)*2;
		*pFilter = color * abs(oray->v * surface.normal) * LColor(pow(color.r, p), pow(color.g, p), pow(color.b, p));
	}
}

void LMaterialPaper::SetColor(LColor color)
{
	this->color = color;
}

void LMaterialCombine::Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const
{
	if (randdevice.rand_double() < scale)mat[0]->Interaction(surface, oray, Scale, pFilter, randdevice, ppIndexSeq, pReserved, Floor);
	else mat[1]->Interaction(surface, oray, Scale, pFilter, randdevice, ppIndexSeq, pReserved, Floor);
}

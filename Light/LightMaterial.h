#pragma once
#include"LightGeometry.h"
#include"LightFilm.h"

extern class LModel;

extern class LMedium;

class LMaterial;


class LSurfaceInfo
{
public:
	LRay ray;
	LVec intersection;
	LVec normal;
	LVec model_normal;
	LVec transformed_pos;
	double distance = 0.0;
	double time = 0.0;
	const LMaterial* material = NULL;
	LModel* model = NULL;
};

class LMaterial
{
public:
	virtual void Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const = 0;
};

class LMaterialReflectAny:public LMaterial
{
public:
	void Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const;
	virtual void Interaction(const LSurfaceInfo& surface, LRay outray, double* Scale, LColor* pFilter, RandomDevice& randdevice) const = 0;
};

class LMaterialLambertian :public LMaterialReflectAny
{
public:
	void Interaction(const LSurfaceInfo& surface, LRay outray, double* Scale, LColor* pFilter, RandomDevice& randdevice) const;
	virtual LColor GetColor(const LSurfaceInfo& surface)const = 0;
};

class LMaterialLambertianPureColor :public LMaterialLambertian
{
public:
	LColor color;
	LMaterialLambertianPureColor();
	LMaterialLambertianPureColor(LColor color);
	void SetColor(LColor color);
	LColor GetColor(const LSurfaceInfo& surface) const;
};

class LMaterialLambertianTexture :public LMaterialLambertian
{
public:
	Image img;
	double scale;
	LMaterialLambertianTexture();
	void SetTexture(PCWSTR filepath, double scale = 1.0);
	LColor GetColor(const LSurfaceInfo& surface) const;
};

class LMaterialLightSource:public LMaterial
{
public:
	void Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const;
	virtual LColor GetColor(const LSurfaceInfo& surface) const = 0;
};

class LMaterialLightSourcePureColor :public LMaterialLightSource
{
public:
	LColor color;
	LMaterialLightSourcePureColor();
	LMaterialLightSourcePureColor(LColor color);
	void SetColor(LColor color);
	LColor GetColor(const LSurfaceInfo& surface) const;
};

class LMaterialLightSourceTexture :public LMaterialLightSource
{
public:
	Image img;
	double scale;
	LMaterialLightSourceTexture();
	void SetTexture(PCWSTR filepath, double scale = 1.0);
	LColor GetColor(const LSurfaceInfo& surface) const;
};

class LMaterialSpecularSurface :public LMaterial
{
public:
	void Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const;
	virtual LColor GetColor(const LSurfaceInfo& surface) const = 0;
};

class LMaterialSpecularSurfacePureColor :public LMaterialSpecularSurface
{
public:
	LColor color;
	LMaterialSpecularSurfacePureColor();
	LMaterialSpecularSurfacePureColor(LColor color);
	void SetColor(LColor color);
	LColor GetColor(const LSurfaceInfo& surface) const;
};

class LMaterialSpecularSurfaceTexture :public LMaterialSpecularSurface
{
public:
	Image img;
	double scale;
	LMaterialSpecularSurfaceTexture();
	void SetTexture(PCWSTR filepath, double scale = 1.0);
	LColor GetColor(const LSurfaceInfo& surface) const;
};

class LMaterialMixture :public LMaterial
{
public:
	LMaterial* mat[2];
	double scale[2];
	LMaterialMixture();
	LMaterialMixture(LMaterial* pMat0, double scale0, LMaterial* pMat1, double scale1);
	void Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const;
};

class LMaterialRefractionSurface :public LMaterial
{
public:
	void Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const;
	virtual LColor GetColor(const LSurfaceInfo& surface) const = 0;
};

class LMaterialRefractionSurfacePureColor :public LMaterialRefractionSurface
{
public:
	LColor color;
	LMaterialRefractionSurfacePureColor();
	LMaterialRefractionSurfacePureColor(LColor color);
	void SetColor(LColor color);
	LColor GetColor(const LSurfaceInfo& surface) const;
};

class LMaterialGlossySurface :public LMaterial
{
public:
	LColor color;
	double glossy;
	LMaterialGlossySurface();
	LMaterialGlossySurface(LColor color, double glossy);
	void SetColor(LColor color);
	void Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const;
};


void InteractionSpecular(const LSurfaceInfo& surface, LVec real_normal, LRay* oray);

void InteractionRefraction(const LSurfaceInfo& surface, LVec real_normal, LRay* oray,LColor *pFilter, RandomDevice& randdevice);

void InteractionReflectAny(const LSurfaceInfo& surface, LRay* oray, RandomDevice& randdevice, unsigned** ppIndexSeq, UINT Floor);

void InteractionLightSource(const LSurfaceInfo& surface, LRay* oray);



#pragma once
#include"LightMaterial.h"

extern class LModel;

class LMedium
{
public:
	virtual double GetRefractionIndex() const;
	virtual bool GetParticipation(LRay iray, double time, RandomDevice& randdevice, unsigned** ppIndexSeq, LSurfaceInfo* pSurfaceInfo) const;
	virtual void GetFilter(const LSurfaceInfo* pSurfaceInfo, LColor* pFilter, double* pScale) const;
};

class LMediumNonParticipating :public LMedium
{
public:
	double refraction_index = 1.0;
	LColor attenuation = LColor(1.0, 1.0, 1.0);
	void SetRefractionIndex(double ri);
	void SetAttenuation(LColor att);
	double GetRefractionIndex() const;
	void GetFilter(const LSurfaceInfo* pSurfaceInfo, LColor* pFilter, double* pScale)const;
};

class LMediumParticipating :public LMediumNonParticipating
{
public:
	double absorb;
	LMaterialSpecularSurfacePureColor mat_impurity;
	LMediumParticipating();
	LMediumParticipating(double _absorb, LColor _color);
	bool GetParticipation(LRay iray, double time, RandomDevice& randdevice, unsigned** ppIndexSeq, LSurfaceInfo* pSurfaceInfo) const;
};

void SetDefault(LMedium* pMedium);

void GetDefaultMedium(LMedium** ppMedium);


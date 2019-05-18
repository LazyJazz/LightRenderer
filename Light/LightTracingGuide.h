#pragma once
#include"LightGeometry.h"
class LTracingGuideUnit
{
public:
	LVec keyo = LVec();
	LVec keyv = LVec();
	double weight = 0.0;
	LBoundingBox bb = LBoundingBox();
	UINT childs[8] = {};
};
class LTracingGuide
{
public:
	UINT uSize, uCnt;
	LTracingGuideUnit* pUnits;
	LTracingGuide();
	void SetSize(UINT size);
};
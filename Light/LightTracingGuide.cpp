#include "LightTracingGuide.h"

LTracingGuide::LTracingGuide()
{
	uSize = 0;
	uCnt = 0;
	pUnits = NULL;
}

void LTracingGuide::SetSize(UINT size)
{
	if (size <= uCnt)return;
	LTracingGuideUnit* pnewUnits = new LTracingGuideUnit[size];
	uSize = size;
	if (uCnt)
	{
		memcpy(pnewUnits, pUnits, sizeof(LTracingGuideUnit) * uCnt);
	}
	if (pUnits)delete[]pUnits;
	pUnits = pnewUnits;
}

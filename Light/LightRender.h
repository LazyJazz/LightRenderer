#pragma once
#include"LightModel.h"
#include"LightCamara.h"
#include"LightFilm.h"
#include"LightMaterial.h"
#include<vector>
#include<queue>
using namespace std;

class RenderMission
{
public:
	UINT x = 0, y = 0, times = 0;
	LColor color = LColor();
	double scale = 0.0;
};

class LRender
{
public:
	LModel* pModel = NULL;
	LCamara* pCamara = NULL;
	LFilm* pFilm = NULL;
	//virtual void StartRender(int nThread) = 0;
};

class LRenderPathTracing:public LRender
{
public:
	HANDLE hMutex;
	queue<RenderMission> MissionSequence;
	queue<RenderMission> ResultSequence;
	unsigned** ppIndexSeq;
	wstring project_name = L"";
	int sample_per_round = 2000;
	LRenderPathTracing();
	void PathTracingCore(LRay iray, double time, RandomDevice& randdevice, LColor* pColor, double* pScale, UINT Floor = 0, unsigned* index_seq = NULL);
	static DWORD WINAPI RenderThread(LPVOID pReserved);
	void StartRender(int nThread);
};
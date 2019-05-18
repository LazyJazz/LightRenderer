#include "LightRender.h"

LRenderPathTracing::LRenderPathTracing()
{
	pCamara = NULL;
	pFilm = NULL;
	pModel = NULL;
	hMutex = NULL;
	ppIndexSeq = NULL;
}

void LRenderPathTracing::PathTracingCore(LRay iray, double time, RandomDevice& randdevice, LColor* pColor, double* pScale, UINT Floor, unsigned* index_seq)
{
	if (Floor == 12)
	{
		*pColor = LColor();
		*pScale = 1.0;
		return;
	}
	LSurfaceInfo SurfaceInfo;
	LMedium* medium;
	pModel->GetMedium(iray, time, &medium);
	if (!medium)GetDefaultMedium(&medium);
	

	//printf_s("Dist:%lf\n", SurfaceInfo.distance);
	bool res = medium->GetParticipation(iray, time, randdevice, ppIndexSeq, &SurfaceInfo);
	res |= pModel->GetIntersection(iray, time, &SurfaceInfo);
	if (!res)
	{
		*pColor = LColor();
		*pScale = 1.0;
		return;
	}
	SurfaceInfo.model = pModel;
	SurfaceInfo.time = time;
	LRay nextRay;
	double resScale;
	LColor resColor;
	/*nextRay.o = SurfaceInfo.intersection;
	if (Floor)
	{
		do
		{
			nextRay.v = RandOnSphere(randdevice);
		} while (nextRay.v * SurfaceInfo.normal < 0.0);
	}
	else
	{
		do
		{
			nextRay.v = RandOnSphere(index_seq[Floor]);
		} while (nextRay.v * SurfaceInfo.normal < 0.0);
	}//*/
	//nextRay.v = RandOnSphereCrown(randdevice, 0.0, SurfaceInfo.normal);
	SurfaceInfo.material->Interaction(SurfaceInfo, &nextRay, &resScale, &resColor, randdevice, &index_seq, NULL, Floor);
	if (nextRay.v.iszero())* pColor = resColor, * pScale = resScale;
	else
	{
		PathTracingCore(nextRay, time, randdevice, pColor, pScale, Floor + 1, index_seq);
		*pColor = *pColor * resColor;
		*pScale = *pScale * resScale;
	}
	medium->GetFilter(&SurfaceInfo, pColor, pScale);
}

DWORD __stdcall LRenderPathTracing::RenderThread(LPVOID pReserved)
{
	RandomDevice randdevice;
	LRenderPathTracing* pThis = static_cast<LRenderPathTracing*>(pReserved);
	puts("RenderThread Activated!");
	//unsigned index_seq[32];
	//for (int i = 0; i < 32; i++)index_seq[i] = randdevice.rand_int(0, 1048575);
	while (true)
	{
		WaitForSingleObject(pThis->hMutex, INFINITE);
		if (pThis->MissionSequence.empty())break;
		RenderMission render_mission = pThis->MissionSequence.front();
		if(render_mission.times) pThis->MissionSequence.pop(), ReleaseMutex(pThis->hMutex);
		else { ReleaseMutex(pThis->hMutex); continue; }
		render_mission.color = LColor();
		render_mission.scale = 0.0;
		for (UINT i = 0; i < render_mission.times; i++)
		{
			double time = sqrt(randdevice.rand_double());
			LRay first_ray;
			first_ray = pThis->pCamara->GenerateRay(render_mission.x, render_mission.y, time, randdevice);
			LColor resColor;
			double resScale;
			pThis->PathTracingCore(first_ray, time, randdevice, &resColor, &resScale, 0, pThis->ppIndexSeq[render_mission.y * pThis->pFilm->width + render_mission.x]);
			render_mission.color = render_mission.color + resColor * resScale;
			render_mission.scale = render_mission.scale + resScale;
		}
		WaitForSingleObject(pThis->hMutex, INFINITE);
		pThis->ResultSequence.push(render_mission);
		ReleaseMutex(pThis->hMutex);
	}
	ReleaseMutex(pThis->hMutex);
	return 0;
}

void LRenderPathTracing::StartRender(int nThread)
{
	Image img;
	ShowWindow(GetHWnd(), SW_SHOW);
	hMutex = CreateMutex(NULL, FALSE, L"PathTracingRenderer");
	if (!hMutex)return;
	static_cast<LCamaraFlat*>(pCamara)->SetFilmSize(pFilm->width, pFilm->height);
	vector<RenderMission> vecMission;
	vecMission.resize((size_t)pFilm->width * pFilm->height);
	ppIndexSeq = new unsigned* [(size_t)pFilm->width * pFilm->height];
	RandomDevice rnddev;
	for (size_t i = 0; i < (size_t)pFilm->width * pFilm->height; i++)
	{
		ppIndexSeq[i] = new unsigned[32];
		for (size_t j = 0; j < 32; j++)
		{
			ppIndexSeq[i][j] = rnddev.rand_int(0, 1048575);
		}
	}
	for (UINT x = 0; x < pFilm->width; x++)
		for (UINT y = 0; y < pFilm->height; y++)
		{
			RenderMission render_mission;
			render_mission.x = x;
			render_mission.y = y;
			render_mission.times = sample_per_round;
			vecMission[(size_t)y * pFilm->width + x] = render_mission;
		}
	MissionSequence.push(RenderMission());
	for (int id = 0; id < nThread; id++)
		CreateThread(NULL, NULL, RenderThread, this, NULL, NULL);
	Timer timer;
	int SPP = 0;
	while (!GetAsyncKeyState(VK_ESCAPE))
	{
		WaitForSingleObject(hMutex, INFINITE);
		MissionSequence.pop();
		random_shuffle(vecMission.begin(), vecMission.end());
		for (RenderMission rm : vecMission)MissionSequence.push(rm);
		MissionSequence.push(RenderMission());
		ReleaseMutex(hMutex);
		UINT nResult = 0, lastnResult = 0;
		while (true)
		{
			WaitForSingleObject(hMutex, INFINITE);
			if (nResult == pFilm->width * pFilm->height)break;
			while (!ResultSequence.empty())
			{
				RenderMission rm = ResultSequence.front(); ResultSequence.pop();
				pFilm->AddColor(rm.x, rm.y, rm.color, rm.scale);
				//printf("R:%lf G:%lf B:%lf scale:%lf\n", rm.color.r, rm.color.g, rm.color.b, rm.scale);
				nResult++;
			}
			ReleaseMutex(hMutex);
			pFilm->GetImage(&img);
			SetFrameImage(&img);
			timer.Sleep(1000.0);
			printf("Total:%d Done:%d Remain:%d ", pFilm->width * pFilm->height, nResult, pFilm->width * pFilm->height - nResult);
			printf("\n");
			/*if (lastnResult == nResult)printf("\n");
			else printf("Time Remain:%ds\n", (int)(pFilm->width * pFilm->height - nResult) / (int)(nResult - lastnResult));//*/
			lastnResult = nResult;
		}
		ReleaseMutex(hMutex);
		SPP += sample_per_round;
		pFilm->GetImage(&img);
		TCHAR filename[128];
		swprintf_s(filename, L"%ws-%dspp.png", project_name.c_str(), SPP);
		img.SaveToFile(filename);
		printf("%d SPP Done...%d s used\n", SPP, clock() / CLOCKS_PER_SEC);
	}
	WaitForSingleObject(hMutex, INFINITE);
	MissionSequence.pop();
	ReleaseMutex(hMutex);
}

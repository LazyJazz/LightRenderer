#include"LibZJ.h"
#include"Light.h"
#include <random>
#include<iostream>
#include <iomanip>
#include<map>
#include<fstream>

using namespace std;

#include"my_rasterize_lib.h"

void PrintGuid(GUID guid)
{
	printf_s("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n",
		guid.Data1, guid.Data2,
		guid.Data3, guid.Data4[0],
		guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4],
		guid.Data4[5], guid.Data4[6],
		guid.Data4[7]);
}

std::string GenerateGuid()
{
	GUID guid;
	HRESULT hr = CoCreateGuid(&guid);
	if (FAILED(hr))return string("");
	char cBuffer[64] = { 0 };
	sprintf_s(cBuffer, sizeof(cBuffer),
		"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2,
		guid.Data3, guid.Data4[0],
		guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4],
		guid.Data4[5], guid.Data4[6],
		guid.Data4[7]);
	return std::string(cBuffer);
}
LModelTriangle triangles[12];
LModelSphere spheres[18];
LModelSet models;
LMaterialLightSourcePureColor mat_light;
LMaterialSpecularSurfacePureColor mat_specular;
LMaterialLambertianPureColor mat_diffuse;
LMaterialLambertianPureColor mat_diffuse_red;
LMaterialLambertianPureColor mat_diffuse_green;
LMaterialLambertianPureColor mat_diffuse_blue;
LMaterialLambertianPureColor mat_diffuse_yellow;
LMaterialLambertianPureColor mat_diffuse_cyan;
LMaterialLambertianPureColor mat_diffuse_magenta;
LMaterialLambertianPureColor mat_diffuse_jadegreen;
LMaterialRefractionSurfacePureColor mat_ref;
LMaterialGlossySurface mat_glossy_gold;
LMediumNonParticipating med_glass;
LMediumParticipating med_jade;
LMediumParticipating med_amber;
LMediumParticipating med_default;
LCoordinate coordinate;
LCamaraFlat camara;
LFilm film;
LRenderPathTracing render;
LMesh mesh, mesh2;
LModelMesh model_mesh, model_mesh2;

int main1()
{
	//	HRESULT hr;
	InitWindow(600, 600, L"Light Engine", false, SW_HIDE);
	InitD2DFactory();
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "r", stdin);
	/*RandomDevice rd;
	double sumv = 0.0, alpha = 1.0 - 1e-30;
	int cnt = 0;
	cout << 1.0 - alpha << endl;
	for (auto i = 0; i < 1000000; i++)
		if(log(rd.rand_double()) / log(1e-8)<0.01)
			cnt++;
	cout << cnt / 1000000.0;
	system("pause");
	return 0;//*/

	//mesh.LoadObjFile("bunny.obj");
	//mesh.LoadObjFile("frostmourn/sword.obj", 1);
	mesh.LoadObjFile("fixed.perfect.dragon.100K.0.07.obj");
	mesh2.LoadObjFile("fixed.perfect.dragon.100K.0.07.obj");
	//mesh.LoadObjFile("TingYuan.obj");
	//mesh.LoadObjFile("cube.obj");
	mesh.Move(-mesh.CenterPoint());
	mesh2.Move(-mesh2.CenterPoint());
	//mesh.Rotate(90.0 / 180.0 * L_PI, 0.0, 0.0);
	//mesh.Rotate(0.0, 90.0 / 180.0 * L_PI, 0.0);
	mesh.Move(LVec(0.0, -mesh.bounding_box.v0.y, 0.0));
	mesh2.Move(LVec(0.0, -mesh2.bounding_box.v0.y, 0.0));
	mesh.Normalize(1.0);
	mesh2.Normalize(1.0);
	mesh.Move(LVec(-0.4, -1.0, 0.25));
	mesh2.Move(LVec(0.4, -1.0, -0.25));
	//mesh.Rotate(0.0, L_PI, 0.0);
	//mesh2.Rotate(0.0, L_PI, 0.0);
	mesh.Build();
	mesh2.Build();

	mat_ref.SetColor(LColor(1.0, 1.0, 1.0));
	mat_light.SetColor(LColor(40, 40, 40));
	mat_diffuse.SetColor(LColor(0.8, 0.8, 0.8));
	mat_diffuse_red.SetColor(LColor(0.8, 0.0, 0.0));
	mat_diffuse_green.SetColor(LColor(0.0, 0.8, 0.0));
	mat_diffuse_blue.SetColor(LColor(0.0, 0.0, 0.8));
	mat_diffuse_yellow.SetColor(LColor(0.8, 0.8, 0.0));
	mat_diffuse_cyan.SetColor(LColor(0.0, 0.8, 0.8));
	mat_diffuse_magenta.SetColor(LColor(0.8, 0.0, 0.8));
	mat_diffuse_jadegreen.SetColor(LColor(0.034, 0.177, 0.076) * 4.0);
	mat_specular.SetColor(LColor(0.8, 0.8, 0.8));
	mat_glossy_gold = LMaterialGlossySurface(LColor(1.0, 0.94, 0.0) * 0.5, 0.0);

	med_glass.SetRefractionIndex(1.5);
	med_glass.SetAttenuation(LColor(1.0, 1.0, 1.0));

	med_default = LMediumParticipating(0.4, LColor(0.8, 0.8, 0.8));

	/*triangles[0] = LModelTriangle(LTriangle(LVec(-1.0, 0.0, -1.0), LVec(-1.0, 0.0, 1.0), LVec(1.0, 0.0, -1.0)), &mat_diffuse);
	triangles[1] = LModelTriangle(LTriangle(LVec(1.0, 0.0, 1.0), LVec(-1.0, 0.0, 1.0), LVec(1.0, 0.0, -1.0)), &mat_diffuse);
	triangles[2] = LModelTriangle(LTriangle(LVec(-2.0, -0.5, -2.0), LVec(-2.0, -0.5, 2.0), LVec(2.0, -0.5, -2.0)), &mat_diffuse);
	triangles[3] = LModelTriangle(LTriangle(LVec(2.0, -0.5, 2.0), LVec(-2.0, -0.5, 2.0), LVec(2.0, -0.5, -2.0)), &mat_diffuse);
	triangles[4] = LModelTriangle(LTriangle(LVec(-0.3, 1.0, -0.3), LVec(-0.3, 1.0, 0.3), LVec(0.3, 1.0, -0.3)), &mat_light);
	triangles[5] = LModelTriangle(LTriangle(LVec(0.3, 1.0, 0.3), LVec(-0.3, 1.0, 0.3), LVec(0.3, 1.0, -0.3)), &mat_light);//*/
	/*
	triangles[0] = LModelTriangle(LTriangle(LVec(-10.0, 10.0, -10.0), LVec(10.0, 10.0, 10.0), LVec(-10.0, 10.0, 10.0)), &mat_light);
	triangles[1] = LModelTriangle(LTriangle(LVec(-10.0, 10.0, -10.0), LVec(10.0, 10.0, 10.0), LVec(10.0, 10.0, -10.0)), &mat_light);
	//triangles[0] = LModelTriangle(LTriangle(LVec(-5.0, 10.0, 5.0), LVec(5.0, 0.0, 5.0), LVec(-5.0, 0.0, 5.0)), &mat_light);
	//triangles[1] = LModelTriangle(LTriangle(LVec(-5.0, 10.0, 5.0), LVec(5.0, 0.0, 5.0), LVec(5.0, 10.0, 5.0)), &mat_light);
	triangles[2] = LModelTriangle(LTriangle(LVec(-1e10, 0.0, -1e10), LVec(1e10, 0.0, 1e10), LVec(-1e10, 0.0, 1e10)), &mat_diffuse);
	triangles[3] = LModelTriangle(LTriangle(LVec(-1e10, 0.0, -1e10), LVec(1e10, 0.0, 1e10), LVec(1e10, 0.0, -1e10)), &mat_diffuse);//*/

	triangles[0] = LModelTriangle(LTriangle(LVec(-0.3, 1.0 - L_EPS, -0.3), LVec(0.3, 1.0 - L_EPS, 0.3), LVec(-0.3, 1.0 - L_EPS, 0.3)), &mat_light);
	triangles[1] = LModelTriangle(LTriangle(LVec(-0.3, 1.0 - L_EPS, -0.3), LVec(0.3, 1.0 - L_EPS, 0.3), LVec(0.3, 1.0 - L_EPS, -0.3)), &mat_light);
	triangles[2] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(1.0, -1.0, 1.0), LVec(-1.0, -1.0, 1.0)), &mat_diffuse);
	triangles[3] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(1.0, -1.0, 1.0), LVec(1.0, -1.0, -1.0)), &mat_diffuse);
	triangles[4] = LModelTriangle(LTriangle(LVec(1.0, -1.0, -1.0), LVec(1.0, 1.0, 1.0), LVec(1.0, -1.0, 1.0)), &mat_diffuse_yellow);
	triangles[5] = LModelTriangle(LTriangle(LVec(1.0, -1.0, -1.0), LVec(1.0, 1.0, 1.0), LVec(1.0, 1.0, -1.0)), &mat_diffuse_yellow);
	triangles[6] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, 1.0, -1.0), LVec(1.0, 1.0, -1.0)), &mat_diffuse);
	triangles[7] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, 1.0, -1.0), LVec(-1.0, 1.0, 1.0)), &mat_diffuse);
	//triangles[8] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0), LVec(1.0, -1.0, 1.0)), &mat_diffuse_magenta);
	//triangles[9] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0), LVec(-1.0, 1.0, 1.0)), &mat_diffuse_magenta);
	triangles[8] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0), LVec(1.0, -1.0, 1.0)), &mat_diffuse);
	triangles[9] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0), LVec(-1.0, 1.0, 1.0)), &mat_diffuse);
	triangles[10] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(-1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0)), &mat_diffuse_cyan);
	triangles[11] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(-1.0, 1.0, 1.0), LVec(-1.0, 1.0, -1.0)), &mat_diffuse_cyan);//*/
	spheres[0] = LModelSphere(LSphere(LVec(0.0, -0.0, 0.0), 0.3), &mat_ref);
	spheres[0].SetMedium(&med_glass);

	med_jade = LMediumParticipating(1.0 - 1e-8, LColor(0.8, 0.8, 0.8));
	med_jade.SetAttenuation(LColor(0.034, 0.177, 0.076) * 4.0);
	med_jade.SetRefractionIndex(1.3);

	med_amber = LMediumParticipating(1.0 - 1e-8, LColor(0.8, 0.8, 0.8));
	med_amber.SetAttenuation(LColor(0.5, 0.05, 0.05));
	med_amber.SetRefractionIndex(1.3);

	model_mesh = LModelMesh(&mesh, &mat_diffuse_jadegreen);
	//model_mesh.SetMedium(&med_jade);
	model_mesh2 = LModelMesh(&mesh2, &mat_diffuse_jadegreen);
	//model_mesh2.SetMedium(&med_jade);

	/*for (int i = 0; i < 12; i++)
	{
		puts(triangles[i].triangle.pl.islegal() ? "YES" : "NO");
	}//*/
	for (int i = 0; i < 12; i++)
		models.AddModel(&triangles[i]);
	models.AddModel(&model_mesh);
	models.AddModel(&model_mesh2);
	//models.AddModel(&spheres[0]);
	//models.SetMedium(&med_default);
	coordinate.Reset();
	coordinate.o = LVec(0.0, 0.0, -3.0);
	coordinate.Rotate(L_PI * -0.03, 0.0, 0.0);
	camara.SetCoordinate(coordinate);
	camara.SetViewSize(0.7, 0.7);
	film.Resize(256, 256);
	render.pCamara = &camara;
	render.pFilm = &film;
	render.pModel = &models;
	render.sample_per_round = 2000;
	render.project_name = L"SubsurfaceScatteringFakeJade";
	render.StartRender(64);
	Image img;
	film.GetImage(&img);
	SetFrameImage(&img);
	//img.SaveToFile(L"save.png");
	system("pause");
	return 0;
}
int main2()
{
	//	HRESULT hr;
	InitWindow(1024, 1024, L"Light Engine", false, SW_HIDE);
	InitD2DFactory();
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "r", stdin);
	/*RandomDevice rd;
	double sumv = 0.0, alpha = 1.0 - 1e-30;
	int cnt = 0;
	cout << 1.0 - alpha << endl;
	for (auto i = 0; i < 1000000; i++)
		if(log(rd.rand_double()) / log(1e-8)<0.01)
			cnt++;
	cout << cnt / 1000000.0;
	system("pause");
	return 0;//*/

	mat_ref.SetColor(LColor(1.0, 1.0, 1.0));
	mat_light.SetColor(LColor(1, 1, 1)*40);
	mat_diffuse.SetColor(LColor(0.8, 0.8, 0.8));
	mat_diffuse_red.SetColor(LColor(0.8, 0.0, 0.0));
	mat_diffuse_green.SetColor(LColor(0.0, 0.8, 0.0));
	mat_diffuse_blue.SetColor(LColor(0.0, 0.0, 0.8));
	mat_diffuse_yellow.SetColor(LColor(0.8, 0.8, 0.0));
	mat_diffuse_cyan.SetColor(LColor(0.0, 0.8, 0.8));
	mat_diffuse_magenta.SetColor(LColor(0.8, 0.0, 0.8));
	mat_diffuse_jadegreen.SetColor(LColor(0.034, 0.177, 0.076) * 4.0);
	mat_specular.SetColor(LColor(0.8, 0.8, 0.8));
	mat_glossy_gold = LMaterialGlossySurface(LColor(1.0, 1.0, 1.0) * 0.5, 0.01);

	med_glass.SetRefractionIndex(1.5);
	med_glass.SetAttenuation(LColor(1.0, 1.0, 1.0));

	med_default = LMediumParticipating(0.4, LColor(0.8, 0.8, 0.8));

	triangles[0] = LModelTriangle(LTriangle(LVec(-0.3, 1.0 - L_EPS, -0.3), LVec(0.3, 1.0 - L_EPS, 0.3), LVec(-0.3, 1.0 - L_EPS, 0.3)), &mat_light);
	triangles[1] = LModelTriangle(LTriangle(LVec(-0.3, 1.0 - L_EPS, -0.3), LVec(0.3, 1.0 - L_EPS, 0.3), LVec(0.3, 1.0 - L_EPS, -0.3)), &mat_light);
	triangles[2] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(1.0, -1.0, 1.0), LVec(-1.0, -1.0, 1.0)), &mat_glossy_gold);
	triangles[3] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(1.0, -1.0, 1.0), LVec(1.0, -1.0, -1.0)), &mat_glossy_gold);
	triangles[4] = LModelTriangle(LTriangle(LVec(1.0, -1.0, -1.0), LVec(1.0, 1.0, 1.0), LVec(1.0, -1.0, 1.0)), &mat_diffuse_yellow);
	triangles[5] = LModelTriangle(LTriangle(LVec(1.0, -1.0, -1.0), LVec(1.0, 1.0, 1.0), LVec(1.0, 1.0, -1.0)), &mat_diffuse_yellow);
	triangles[6] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, 1.0, -1.0), LVec(1.0, 1.0, -1.0)), &mat_diffuse);
	triangles[7] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, 1.0, -1.0), LVec(-1.0, 1.0, 1.0)), &mat_diffuse);
	//triangles[8] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0), LVec(1.0, -1.0, 1.0)), &mat_diffuse_magenta);
	//triangles[9] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0), LVec(-1.0, 1.0, 1.0)), &mat_diffuse_magenta);
	triangles[8] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0), LVec(1.0, -1.0, 1.0)), &mat_diffuse);
	triangles[9] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0), LVec(-1.0, 1.0, 1.0)), &mat_diffuse);
	triangles[10] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(-1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0)), &mat_diffuse_cyan);
	triangles[11] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(-1.0, 1.0, 1.0), LVec(-1.0, 1.0, -1.0)), &mat_diffuse_cyan);//*/

	LModelShift shifts[18];

	for (int i = 0; i < 2; i++)
	{
		spheres[i] = LModelSphere(LSphere(LVec(), 0.2), &mat_specular);
	}
	shifts[0] = LModelShift(LVec(-0.5, -0.8, 0.0), LVec(-0.2, -0.8, 0), &spheres[0]);
	models.AddModel(&shifts[0]);
	shifts[1] = LModelShift(LVec(0.5, -0.8, 0.0), LVec(0.2, -0.8, 0), &spheres[1]);
	models.AddModel(&shifts[1]);

	med_jade = LMediumParticipating(1.0 - 1e-8, LColor(0.8, 0.8, 0.8));
	med_jade.SetAttenuation(LColor(0.034, 0.177, 0.076) * 4.0);
	med_jade.SetRefractionIndex(1.3);

	med_amber = LMediumParticipating(1.0 - 1e-8, LColor(0.8, 0.8, 0.8));
	med_amber.SetAttenuation(LColor(0.5, 0.05, 0.05));
	med_amber.SetRefractionIndex(1.3);

	model_mesh = LModelMesh(&mesh, &mat_diffuse_jadegreen);
	//model_mesh.SetMedium(&med_jade);
	model_mesh2 = LModelMesh(&mesh2, &mat_diffuse_jadegreen);
	//model_mesh2.SetMedium(&med_jade);

	/*for (int i = 0; i < 12; i++)
	{
		puts(triangles[i].triangle.pl.islegal() ? "YES" : "NO");
	}//*/
	for (int i = 0; i < 12; i++)
		models.AddModel(&triangles[i]);
	//models.AddModel(&spheres[0]);
	coordinate.Reset();
	coordinate.o = LVec(0.0, 0.0, -3.0);
	coordinate.Rotate(0.0, 0.0, 0.0);
	camara.SetCoordinate(coordinate);
	camara.SetViewSize(1.0, 1.0);
	film.Resize(1024, 1024);
	render.pCamara = &camara;
	render.pFilm = &film;
	render.pModel = &models;
	render.sample_per_round = 2000;
	render.project_name = L"MotionBlur";
	render.StartRender(64);
	Image img;
	film.GetImage(&img);
	SetFrameImage(&img);
	//img.SaveToFile(L"save.png");
	system("pause");
	return 0;
}

int main3()
{
	//	HRESULT hr;
	InitWindow(600, 600, L"Light Engine", false, SW_HIDE);
	InitD2DFactory();
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "r", stdin);
	/*RandomDevice rd;
	double sumv = 0.0, alpha = 1.0 - 1e-30;
	int cnt = 0;
	cout << 1.0 - alpha << endl;
	for (auto i = 0; i < 1000000; i++)
		if(log(rd.rand_double()) / log(1e-8)<0.01)
			cnt++;
	cout << cnt / 1000000.0;
	system("pause");
	return 0;//*/

	//mesh.LoadObjFile("bunny.obj");
	//mesh.LoadObjFile("frostmourn/sword.obj", 1);
	mesh.LoadObjFile("bunny.obj");
	mesh2.LoadObjFile("bunny.obj");
	//mesh.LoadObjFile("TingYuan.obj");
	//mesh.LoadObjFile("cube.obj");
	mesh.Move(-mesh.CenterPoint());
	mesh2.Move(-mesh2.CenterPoint());
	//mesh.Rotate(90.0 / 180.0 * L_PI, 0.0, 0.0);
	//mesh.Rotate(0.0, 90.0 / 180.0 * L_PI, 0.0);
	mesh.Move(LVec(0.0, -mesh.bounding_box.v0.y, 0.0));
	mesh2.Move(LVec(0.0, -mesh2.bounding_box.v0.y, 0.0));
	mesh.Normalize(0.66);
	mesh2.Normalize(0.66);
	mesh.Move(LVec(-0.4, -1.0, 0.25));
	mesh2.Move(LVec(0.4, -1.0, -0.25));
	mesh.Build();
	mesh2.Build();

	mat_ref.SetColor(LColor(1.0, 1.0, 1.0));
	mat_light.SetColor(LColor(40, 40, 40));
	mat_diffuse.SetColor(LColor(0.8, 0.8, 0.8));
	mat_diffuse_red.SetColor(LColor(0.8, 0.0, 0.0));
	mat_diffuse_green.SetColor(LColor(0.0, 0.8, 0.0));
	mat_diffuse_blue.SetColor(LColor(0.0, 0.0, 0.8));
	mat_diffuse_yellow.SetColor(LColor(0.8, 0.8, 0.0));
	mat_diffuse_cyan.SetColor(LColor(0.0, 0.8, 0.8));
	mat_diffuse_magenta.SetColor(LColor(0.8, 0.0, 0.8));
	mat_diffuse_jadegreen.SetColor(LColor(0.034, 0.177, 0.076) * 4.0);
	mat_specular.SetColor(LColor(0.8, 0.8, 0.8));
	mat_glossy_gold = LMaterialGlossySurface(LColor(1.0, 0.94, 0.0) * 0.5, 0.0);

	med_glass.SetRefractionIndex(1.5);
	med_glass.SetAttenuation(LColor(0.5, 0.05, 0.05));

	med_default = LMediumParticipating(0.4, LColor(0.8, 0.8, 0.8));

	triangles[0] = LModelTriangle(LTriangle(LVec(-0.3, 1.0 - L_EPS, -0.3), LVec(0.3, 1.0 - L_EPS, 0.3), LVec(-0.3, 1.0 - L_EPS, 0.3)), &mat_light);
	triangles[1] = LModelTriangle(LTriangle(LVec(-0.3, 1.0 - L_EPS, -0.3), LVec(0.3, 1.0 - L_EPS, 0.3), LVec(0.3, 1.0 - L_EPS, -0.3)), &mat_light);
	triangles[2] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(1.0, -1.0, 1.0), LVec(-1.0, -1.0, 1.0)), &mat_diffuse);
	triangles[3] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(1.0, -1.0, 1.0), LVec(1.0, -1.0, -1.0)), &mat_diffuse);
	triangles[4] = LModelTriangle(LTriangle(LVec(1.0, -1.0, -1.0), LVec(1.0, 1.0, 1.0), LVec(1.0, -1.0, 1.0)), &mat_diffuse_yellow);
	triangles[5] = LModelTriangle(LTriangle(LVec(1.0, -1.0, -1.0), LVec(1.0, 1.0, 1.0), LVec(1.0, 1.0, -1.0)), &mat_diffuse_yellow);
	triangles[6] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, 1.0, -1.0), LVec(1.0, 1.0, -1.0)), &mat_diffuse);
	triangles[7] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, 1.0, -1.0), LVec(-1.0, 1.0, 1.0)), &mat_diffuse);
	triangles[8] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0), LVec(1.0, -1.0, 1.0)), &mat_diffuse);
	triangles[9] = LModelTriangle(LTriangle(LVec(1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0), LVec(-1.0, 1.0, 1.0)), &mat_diffuse);
	triangles[10] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(-1.0, 1.0, 1.0), LVec(-1.0, -1.0, 1.0)), &mat_diffuse_cyan);
	triangles[11] = LModelTriangle(LTriangle(LVec(-1.0, -1.0, -1.0), LVec(-1.0, 1.0, 1.0), LVec(-1.0, 1.0, -1.0)), &mat_diffuse_cyan);//*/
	spheres[0] = LModelSphere(LSphere(LVec(-0.4, -0.7, 0.25), 0.3), &mat_specular);
	spheres[1] = LModelSphere(LSphere(LVec(0.4, -0.7, -0.25), 0.3), &mat_specular);
	//spheres[0].SetMedium(&med_glass);

	med_jade = LMediumParticipating(1.0 - 1e-8, LColor(0.8, 0.8, 0.8));
	med_jade.SetAttenuation(LColor(0.034, 0.177, 0.076) * 4.0);
	med_jade.SetRefractionIndex(1.3);

	med_amber = LMediumParticipating(1.0 - 1e-8, LColor(0.8, 0.8, 0.8));
	med_amber.SetAttenuation(LColor(0.5, 0.05, 0.05));
	med_amber.SetRefractionIndex(1.3);

	model_mesh = LModelMesh(&mesh, &mat_ref);
	model_mesh.SetMedium(&med_glass);
	model_mesh2 = LModelMesh(&mesh2, &mat_ref);
	model_mesh2.SetMedium(&med_glass);
	for (int i = 0; i < 12; i++)
		models.AddModel(&triangles[i]);
	//models.AddModel(&model_mesh);
	//models.AddModel(&model_mesh2);
	models.AddModel(&spheres[0]);
	models.AddModel(&spheres[1]);
	//models.SetMedium(&med_default);
	coordinate.Reset();
	coordinate.o = LVec(0.0, 0.0, -3.0);
	coordinate.Rotate(L_PI * -0.03, 0.0, 0.0);
	LCamaraAperture camara;
	camara.SetCoordinate(coordinate);
	camara.SetViewSize(0.7, 0.7);
	camara.SetAperture(0.0);
	camara.SetFocalLength(3.25);
	film.Resize(1024, 1024);
	render.pCamara = &camara;
	render.pFilm = &film;
	render.pModel = &models;
	render.sample_per_round = 2000;
	render.project_name = L"DepthOfFieldX";
	render.StartRender(64);
	Image img;
	film.GetImage(&img);
	SetFrameImage(&img);
	//img.SaveToFile(L"save.png");
	system("pause");
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nShowCmd)
{
	return main3();
}
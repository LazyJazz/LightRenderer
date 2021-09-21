#include"LibZJ.h"
#include"Light.h"
#include <random>
#include<iostream>
#include <iomanip>
#include<map>
#include<fstream>

using namespace std;

LModelTriangle triangles[2];
LModelSphere spheres[1];
LModelSet models;
LModelKDTree models_kd;
LMaterialLightSourcePureColor mat_light;
LMaterialLambertianPureColor mat_diffuse;
LMaterialRefractionSurfacePureColor mat_ref;
LMaterialPaper mat_paper;

LCoordinate coordinate;
LCameraFlat camera;
LFilm film;
LRenderPathTracing render;
LMesh mesh, mesh2;
LModelMesh model_mesh, model_mesh2;
LModelSet lantern_set;
LModelTransform lanterns[21][21][21];
LModelTransform lantern[1024];
LVec position[1024];

LMediumParticipating med_jade;
int LucyDemo()
{
	//	HRESULT hr;
	InitWindow(600, 600, L"Light Engine", false, SW_HIDE);
	InitD2DFactory();
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "r", stdin);

	LMaterialLightSourcePureColor mat_lantern_light;
	mat_lantern_light.SetColor(LColor(1, 1, 1) * 40.0);
	mat_paper.SetColor(LColor(0.988, 0.792, 0.380));
	mat_light.SetColor(LColor(1, 1, 1) * 20.0);
	mat_ref.SetColor(LColor(1, 1, 1));

	med_jade = LMediumParticipating(1.0 - 1e-8, LColor(0.8, 0.8, 0.8));
	med_jade.SetAttenuation(LColor(LColor(34, 253, 76) * (1.0 / 255.0)));
	med_jade.SetRefractionIndex(1.3);

	mesh.LoadObjFile("lucy.obj");
	mesh.Normalize();
	mesh.Move(-mesh.CenterPoint());
	mesh.Rotate(0.0, L_PI * 0.7, 0.0);
	mesh.Move(LVec(0.0, -mesh.bounding_box.v0.y - 1.0, 0.0));
	mesh.Build();
	model_mesh.mesh = &mesh;
	model_mesh.material = &mat_ref;
	model_mesh.SetMedium(&med_jade);
	LMaterialDisneyDiffuse mat_ground;
	mat_ground.SetColor(LColor(1, 1, 1));


	triangles[0] = LModelTriangle(LTriangle(LVec(-1e4, -1.0, -1e4), LVec(1e4, -1.0, 1e4), LVec(-1e4, -1.0, 1e4)), &mat_ground);
	triangles[1] = LModelTriangle(LTriangle(LVec(-1e4, -1.0, -1e4), LVec(1e4, -1.0, 1e4), LVec(1e4, -1.0, -1e4)), &mat_ground);

	mesh2.LoadObjFile("lantern.obj");
	mesh2.Normalize(0.04);
	mesh2.Build();
	model_mesh2.mesh = &mesh2;
	model_mesh2.material = &mat_paper;
	model_mesh2.medium = NULL;

	spheres[0] = LModelSphere(LSphere(LVec(0.0, -0.01, 0.0), 0.01), &mat_lantern_light);

	lantern_set.AddModel(&model_mesh2);
	lantern_set.AddModel(&spheres[0]);
	
	RandomDevice rd(20162503);
	for (int i = 0; i < 1024; i++)
	{
		LVec pos;
		bool pass = false;
		while (!pass)
		{
			pass = true;
			pos = RandInSphere(rd) * sqrt(2);
			if (pos.z < -0.2)pass = false;
			for (int j = 0; j < i && pass; j++)
				if ((pos - position[j]) < 0.1)
					pass = false;
			if (abs(pos.y + 1) < 0.04)pass = false;
			if (pass)
			{
				for (LVec v : mesh.vecMeshVertex)
				{
					if ((pos - v) < 0.1)
					{
						pass = false;
						break;
					}
				}
			}
		}
		LCoordinate cdn;
		cdn.Move(pos);
		//cdn.Zoom(1.0);
		cdn.Rotate(0.0, rd.rand_double(-L_PI, L_PI), 0.0);
		lantern[i] = LModelTransform(cdn, &lantern_set);
		position[i] = pos;
		models_kd.AddModel(&lantern[i]);
	}
	models_kd.Build();


	models.AddModel(&models_kd);

	models.AddModel(&model_mesh);

	models.AddModel(&triangles[0]);
	models.AddModel(&triangles[1]);
	coordinate.Reset();
	coordinate.o = LVec(0.0, 0.0, -6.0);
	coordinate.Rotate(-L_PI*0.028, 0.0, 0.0);
	camera.SetCoordinate(coordinate);
	camera.SetViewSize(0.2, 0.2);
	film.Resize(256, 256);
	render.pCamera = &camera;
	render.pFilm = &film;
	render.pModel = &models;
	render.sample_per_round = 200;
	render.project_name = L"lucy-darker";
	render.StartRender(60);
	Image img;
	film.GetImage(&img);
	SetFrameImage(&img);
	system("pause");
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nShowCmd)
{
	return LucyDemo();
}


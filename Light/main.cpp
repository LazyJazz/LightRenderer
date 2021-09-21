#include"LibZJ.h"
#include"Light.h"
#include <random>
#include<iostream>
#include <iomanip>
#include<map>
#include<fstream>

using namespace std;

class LMaterialSkyBox :public LMaterial
{
public:
	void Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const;
};

LModelTriangle triangles[131072];
LModelSphere spheres[131072];
LModelSphere skybox;
LModelSet models;
LModelKDTree models_kd;
LMaterialLightSourcePureColor mat_light;
LMaterialSpecularSurfacePureColor mat_specular;
LMaterialLambertianPureColor mat_diffuse;
LMaterialDisneyDiffuse mat_disney_diffuse;
LMaterialDisneyDiffuse mat_wall;
LMaterialSkyBox mat_skybox;
LMaterialRefractionSurfacePureColor mat_ref;

LMediumNonParticipating med_water;
LMediumNonParticipating med_diamond;

LCoordinate coordinate;
LCamaraFlat camara;
LFilm film;
LRenderPathTracing render;
LMesh mesh, mesh2;
LModelMesh model_mesh, model_mesh2;

const int ModelSample = 100;
int index[2][ModelSample+1][ModelSample+1];
int outputwave()
{
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "r", stdin);
	RandomDevice rd(20162503);
	LBezier bezier(101, 101);
	for (int x = 0; x <= 100; x++)
		for (int y = 0; y <= 100; y++)
		{
			bool edge = true;
			if (x && x < 100 && y && y < 100)edge = false;
			if (edge)bezier.GetVec(x, y) = LVec(x * 0.02 - 1.0, rd.rand_double(-0.5, -0.0), y * 0.02 - 1.0);
			else bezier.GetVec(x, y) = LVec(x * 0.02 - 1.0, rd.rand_double(-1.0 , 0.5), y * 0.02 - 1.0);
		}
	puts("Initialized");
	fopen_s(&file, "wave.obj", "w");
	if (!file)return -1;
	int cnt = 1;
	for (int i = 0; i <= ModelSample; i++)
	{
		for (int j = 0; j <= ModelSample; j++)
		{
			index[0][i][j] = cnt++;
			LVec v = bezier.GetPos(i * (0.9 / ModelSample) + 0.05, j * (0.9 / ModelSample) + 0.05) , vn = bezier.GetNormal(i * (0.9 / ModelSample) + 0.05, j * (0.9 / ModelSample) + 0.05);
			//
			v.x /= 0.9;
			v.z /= 0.9;
			v.y += 0.5;
			fprintf(file, "v %lf %lf %lf\n", v.x, v.y, -v.z);
			fprintf(file, "vn %lf %lf %lf\n", vn.x, vn.y, -vn.z);
		}
		printf("%d\n", i);
	}
	puts("Pass");
	for (int i = 0; i <= ModelSample; i++)
	{
		for (int j = 0; j <= ModelSample; j++)
		{
			index[1][i][j] = cnt++;
			LVec v = LVec(i * (2.0 / ModelSample) - 1.0, -1.0001, j * (2.0 / ModelSample) - 1.0);
			fprintf(file, "v %lf %lf %lf\n", v.x, v.y, -v.z);
		}
	}
	for (int i = 0; i < ModelSample; i++)
	{
		for (int j = 0; j < ModelSample; j++)
		{
			fprintf(file, "f %d//%d %d//%d %d//%d\n", index[0][i][j], index[0][i][j], index[0][i + 1][j], index[0][i + 1][j], index[0][i + 1][j + 1], index[0][i + 1][j + 1]);
			fprintf(file, "f %d//%d %d//%d %d//%d\n", index[0][i][j], index[0][i][j], index[0][i + 1][j + 1], index[0][i + 1][j + 1], index[0][i][j + 1], index[0][i][j + 1]);
			//fprintf(file, "f %d %d %d\n", index[0][i][j], index[0][i + 1][j], index[0][i + 1][j + 1]);
			//fprintf(file, "f %d %d %d\n", index[0][i][j], index[0][i + 1][j + 1], index[0][i][j + 1]);
		}
	}
	for (int i = 0; i < ModelSample; i++)
	{
		for (int j = 0; j < ModelSample; j++)
		{
			fprintf(file, "f %d %d %d\n", index[1][i][j], index[1][i + 1][j + 1], index[1][i + 1][j]);
			fprintf(file, "f %d %d %d\n", index[1][i][j], index[1][i][j + 1], index[1][i + 1][j + 1]);
		}
	}
	for (int i = 0; i < ModelSample; i++)
	{
		fprintf(file, "f %d %d %d\n", index[0][0][i], index[1][0][i + 1], index[1][0][i]);
		fprintf(file, "f %d %d %d\n", index[0][0][i], index[0][0][i + 1], index[1][0][i + 1]);
		fprintf(file, "f %d %d %d\n", index[0][ModelSample][i], index[1][ModelSample][i], index[1][ModelSample][i + 1]);
		fprintf(file, "f %d %d %d\n", index[0][ModelSample][i], index[1][ModelSample][i + 1], index[0][ModelSample][i + 1]);
		fprintf(file, "f %d %d %d\n", index[0][i][0], index[1][i][0], index[1][i + 1][0]);
		fprintf(file, "f %d %d %d\n", index[0][i][0], index[1][i + 1][0], index[0][i + 1][0]);
		fprintf(file, "f %d %d %d\n", index[0][i][ModelSample], index[1][i + 1][ModelSample], index[1][i][ModelSample]);
		fprintf(file, "f %d %d %d\n", index[0][i][ModelSample], index[0][i + 1][ModelSample], index[1][i + 1][ModelSample]);
	}
	puts("Done");
}

int main1()
{
	//	HRESULT hr;
	InitWindow(600, 600, L"Light Engine", false, SW_HIDE);
	InitD2DFactory();
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "r", stdin);
	mat_light.SetColor(LColor(1, 1, 1) * 40.0);
	mat_specular.SetColor(LColor(1.0, 1.0, 1.0));
	mat_diffuse.SetColor(LColor(1.0, 1.0, 1.0));
	mat_disney_diffuse.SetColor(LColor(1.0, 1.0, 1.0));
	mat_disney_diffuse.SetRoughness(0.0);
	mat_wall.SetColor(LColor(0.403, 0.282, 0.925));
	mat_ref.SetColor(LColor(1.0, 1.0, 1.0));

	med_water.SetAttenuation(LColor(1.0, 1.0, 1.0));
	med_water.SetRefractionIndex(1.333);
	med_diamond.SetAttenuation(LColor(1.0, 1.0, 1.0));
	med_diamond.SetRefractionIndex(2.417);
	
	mesh.LoadObjFile("wave.obj");
	mesh.Build();
	model_mesh.mesh = &mesh;
	model_mesh.material = &mat_ref;
	model_mesh.medium = &med_water;
	mesh2.LoadObjFile("diamond.obj");
	mesh2.Normalize();
	mesh2.Move(-(mesh2.bounding_box.v0 + mesh2.bounding_box.v1) * 0.5);
	mesh2.Move(LVec(0.0, -mesh2.bounding_box.v0.y - 1.0, 0.0));
	mesh2.Build();
	model_mesh2.mesh = &mesh2;
	model_mesh2.material = &mat_ref;
	model_mesh2.medium = &med_diamond;
	/*for (int i = 0; i < 101*101; i++)
	{
		int x = i / 101, y = i % 101;
		spheres[i] = LModelSphere(LSphere(LVec(x * 0.02 - 1.0, y * 0.02 - 1.0, 0.0), 0.005), &mat_light);
		models_kd.AddModel(&spheres[i]);
		models.AddModel(&spheres[i]);
	}//*/
	models.AddModel(&model_mesh);
	models.AddModel(&model_mesh2);
	skybox = LModelSphere(LSphere(LVec(0, 0, 0), 1e10), &mat_skybox);
	//spheres[0] = LModelSphere(LSphere(LVec(0.0, 1.8, 0.0), 0.1), &mat_light);
	triangles[0] = LModelTriangle(LTriangle(LVec(-1e3, -1.0, 1e3), LVec(-1e3, -1.0, -1e3), LVec(1e3, -1.0, 1e3)), &mat_diffuse);
	triangles[1] = LModelTriangle(LTriangle(LVec(1e3, -1.0, -1e3), LVec(-1e3, -1.0, -1e3), LVec(1e3, -1.0, 1e3)), &mat_diffuse);
	triangles[2] = LModelTriangle(LTriangle(LVec(-1.0 + 1e-4, -1.0, 1.0), LVec(-1.0 + 1e-4, -1.0, -1.0), LVec(-1.0 + 1e-4, 1.618, 1.0)), &mat_wall);
	triangles[3] = LModelTriangle(LTriangle(LVec(-1.0 + 1e-4, 1.618, -1.0), LVec(-1.0 + 1e-4, -1.0, -1.0), LVec(-1.0 + 1e-4, 1.618, 1.0)), &mat_wall);
	triangles[4] = LModelTriangle(LTriangle(LVec(1.0 - 1e-4, -1.0, 1.0), LVec(1.0 - 1e-4, -1.0, -1.0), LVec(1.0 - 1e-4, 1.618, 1.0)), &mat_wall);
	triangles[5] = LModelTriangle(LTriangle(LVec(1.0 - 1e-4, 1.618, -1.0), LVec(1.0 - 1e-4, -1.0, -1.0), LVec(1.0 - 1e-4, 1.618, 1.0)), &mat_wall);
	triangles[6] = LModelTriangle(LTriangle(LVec(-1.0, 1.618, 1.0 - 1e-4), LVec(-1.0, -1.0, 1.0 - 1e-4), LVec(1.0, 1.618, 1.0 - 1e-4)), &mat_wall);
	triangles[7] = LModelTriangle(LTriangle(LVec(1.0, -1.0, 1.0 - 1e-4), LVec(-1.0, -1.0, 1.0 - 1e-4), LVec(1.0, 1.618, 1.0 - 1e-4)), &mat_wall);
	triangles[8] = LModelTriangle(LTriangle(LVec(-0.3, 1.618, -0.3), LVec(0.3, 1.618, 0.3), LVec(-0.3, 1.618, 0.3)), &mat_light);
	triangles[9] = LModelTriangle(LTriangle(LVec(-0.3, 1.618, -0.3), LVec(0.3, 1.618, 0.3), LVec(0.3, 1.618, -0.3)), &mat_light);
	//triangles[4] = LModelTriangle(LTriangle(LVec(-1.0, 3.0, 1.0), LVec(-1.0, 3.0, -1.0), LVec(1.0, 3.0, 1.0)), &mat_wall);
	//triangles[5] = LModelTriangle(LTriangle(LVec(1.0, 3.0, -1.0), LVec(-1.0, 3.0, -1.0), LVec(1.0, 3.0, 1.0)), &mat_wall);
	//triangles[6] = LModelTriangle(LTriangle(LVec(-1.0, 3.0, 1.0), LVec(-1.0, 3.0, -1.0), LVec(1.0, 3.0, 1.0)), &mat_wall);
	//triangles[7] = LModelTriangle(LTriangle(LVec(1.0, 3.0, -1.0), LVec(-1.0, 3.0, -1.0), LVec(1.0, 3.0, 1.0)), &mat_wall);
	//models.AddModel(&skybox);
	models.AddModel(&spheres[0]);
	//models_kd.AddModel(&spheres[1]);
	models.AddModel(&triangles[0]);
	models.AddModel(&triangles[1]);
	models.AddModel(&triangles[2]);
	models.AddModel(&triangles[3]);
	models.AddModel(&triangles[4]);
	models.AddModel(&triangles[5]);
	models.AddModel(&triangles[6]);
	models.AddModel(&triangles[7]);
	models.AddModel(&triangles[8]);
	models.AddModel(&triangles[9]);

	models_kd.Build();
	models.AddModel(&models_kd);

	coordinate.Reset();
	coordinate.o = LVec(0.0, 1.0, -6.0);
	coordinate.Rotate(L_PI * -0.06, 0.0, 0.0);
	camara.SetCoordinate(coordinate);
	camara.SetViewSize(0.38, 0.38);
	film.Resize(1024, 1024);
	render.pCamara = &camara;
	render.pFilm = &film;
	render.pModel = &models;
	render.sample_per_round = 2000;
	render.project_name = L"WaterSmooth";
	render.StartRender(64);
	Image img;
	film.GetImage(&img);
	SetFrameImage(&img);
	//img.SaveToFile(L"save.png");
	system("pause");
	return 0;
}

LMaterialPaper mat_paper;

LModelSet lantern_set;
LModelTransform lanterns[21][21][21];
LModelTransform lantern[1024];
LVec position[1024];

int main2()
{
	//	HRESULT hr;
	InitWindow(1024, 1024, L"Light Engine", false, SW_HIDE);
	InitD2DFactory();
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "r", stdin);
	RandomDevice rd(20010518);
	mat_light.SetColor(LColor(1, 1, 1) * 5000.0);
	mat_paper.SetColor(LColor(0.988, 0.792, 0.380));

	mesh.LoadObjFile("lantern.obj");
	//mesh.Rotate(0.0, L_PI * 0.25, 0.0);
	mesh.Build();
	model_mesh.mesh = &mesh;
	model_mesh.material = &mat_paper;
	model_mesh.medium = NULL;
	
	spheres[0] = LModelSphere(LSphere(LVec(0.0, -0.5, 0.0), 0.1), &mat_light);

	lantern_set.AddModel(&model_mesh);
	lantern_set.AddModel(&spheres[0]);

	/*for (int i = -2; i <= 2; i++)
	{
		for (int j = -2; j <= 2; j++)
		{
			for (int k = -2; k <= 2; k++)
			{
				LCoordinate cdn;
				cdn.Move(LVec(i, j, k) * 10);
				cdn.Rotate(0.0, rd.rand_double(-L_PI, L_PI), 0.0);
				lanterns[i + 10][j + 10][k + 10] = LModelTransform(cdn, &lantern_set);

				models_kd.AddModel(&lanterns[i + 10][j + 10][k + 10]);
			}
		}
	}//*/
	/*LCoordinate cdn;
	cdn = LCoordinate(0.0, L_PI * 0.25, 0.0);
	cdn.Move(LVec(0.0, 0.0, 6.0));
	lantern[0] = LModelTransform(cdn, &lantern_set);

	models_kd.AddModel(&lantern[0]);//*/

	LCoordinate cdn;
	cdn.Move(LVec(0, 0, 0));
	cdn.Rotate(0.0, L_PI * 0.25, 0.0);
	lantern[0] = LModelTransform(cdn, &lantern_set);
	position[0] = LVec();
	models_kd.AddModel(&lantern[0]);
	for (int i = 1; i < 512; i++)
	{
		LVec pos;
		bool pass = false;
		while (!pass)
		{
			pass = true;
			pos = RandInSphere(rd)*200.0;
			if (pos.z < 0.0)pass = false;
			for (int j = 0; j < i && pass; j++)
				if ((pos - position[j]) < 10.0)
					pass = false;
		}
		LCoordinate cdn;
		cdn.Move(pos);
		cdn.Rotate(0.0, rd.rand_double(-L_PI, L_PI), 0.0);
		lantern[i] = LModelTransform(cdn, &lantern_set);
		models_kd.AddModel(&lantern[i]);
	}
	models_kd.Build();
	
	coordinate.Reset();
	coordinate.o = LVec(0.0, 1.6, -6.0);
	coordinate.Rotate(L_PI*-0.1, 0.0, 0.0);
	LCamaraAperture camara;
	camara.SetCoordinate(coordinate);
	camara.SetViewSize(1.0,1.0);
	camara.SetAperture(0.03);
	camara.SetFocalLength(sqrt(6.0 * 6.0 + 1.6 * 1.6));
	film.Resize(256, 256);
	render.pCamara = &camara;
	render.pFilm = &film;
	render.pModel = &models_kd;
	render.sample_per_round = 2000;
	render.project_name = L"Lanterns";
	render.StartRender(60);
	Image img;
	film.GetImage(&img);
	SetFrameImage(&img);
	//img.SaveToFile(L"save.png");
	system("pause");
	return 0;
}
//LMaterialLightSourceTexture
LMaterialLambertianTexture mat_j20_diffuse;
LMaterialSpecularSurfaceTexture mat_j20_specular;
LMaterialLambertianTexture mat_j20_2;
LMaterialCombine mat_j20;
LModelTransform trans_j20[16];
LModelShift shift_j20[16];

int main3()
{
	//	HRESULT hr;
	InitWindow(600, 600, L"Light Engine", false, SW_HIDE);
	InitD2DFactory();
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CON", "w", stdout);
	freopen_s(&file, "CON", "r", stdin);

	mesh.LoadObjFile("j20-2.obj");
	/*mesh.bounding_box.v0.Print(); puts("");
	mesh.bounding_box.v1.Print(); puts("");//*/
	//system("pause");
	//return 0;
	//mesh.Rotate(0.0, 0.0, L_PI);
	mat_light.SetColor(LColor(1, 1, 1) * 300.0);
	mesh.Normalize(1.0);
	//mesh.Move(-mesh.CenterPoint());
	mesh.Build();
	mat_j20_diffuse.SetTexture(L"J-20_Diffuse_Black.jpg");
	mat_j20_2.SetTexture(L"J-20_Diffuse.jpg");
	mat_j20_specular.SetTexture(L"J-20_Specular_Black.jpg");
	mat_j20.mat[0] = &mat_j20_diffuse;
	mat_j20.mat[1] = &mat_j20_specular;
	mat_j20.scale = 0.9;
	model_mesh = LModelMesh(&mesh, &mat_j20_diffuse);
	model_mesh2 = LModelMesh(&mesh, &mat_j20_2);
	LCoordinate cdn;
	//cdn.Move(LVec(0.0, 0.0, 100.0));
	cdn.Rotate(0.0, L_PI*0.5, 0.0);
	cdn.Rotate(-L_PI * 0.4, 0.0, 0.0);
	cdn.Rotate(0.0, L_PI*0.1, 0.0);
	trans_j20[0].model = &model_mesh;
	trans_j20[0].coordinate = cdn;
	trans_j20[0].medium = NULL;
	shift_j20[0].model = &trans_j20[0];
	shift_j20[0].v0 = LVec(0.0, 0.0, 100.0) + cdn.PutIn(LVec(0.0, 0.0, -0.05));
	shift_j20[0].v1 = LVec(0.0, 0.0, 100.0);


	cdn.Move(LVec(-0.4, 0.3, 100.0));
	/*cdn.Rotate(0.0, L_PI * 0.5, 0.0);
	cdn.Rotate(-L_PI * 0.4, 0.0, 0.0);
	cdn.Rotate(0.0, L_PI * 0.1, 0.0);//*/

	trans_j20[1].model = &model_mesh2;
	trans_j20[1].coordinate = cdn;
	trans_j20[1].medium = NULL;
	shift_j20[1].model = &trans_j20[1];
	shift_j20[1].v0 = LVec(-0.4, 0.3, 200.0) + cdn.PutIn(LVec(0.0, 0.0, -0.05));
	shift_j20[1].v1 = LVec(-0.4, 0.3, 200.0);

	models_kd.AddModel(&shift_j20[0]);
	models_kd.AddModel(&shift_j20[1]);
	models_kd.Build();
	
	models.AddModel(&models_kd);

	spheres[0] = LModelSphere(LSphere(LVec(0.0, 0.0, 0.0), 1e4), &mat_skybox);
	spheres[1] = LModelSphere(LSphere(LVec(0.0, 10000.0, -10000.0).zoom(10000), 1000), &mat_light);

	models.AddModel(&spheres[0]);
	models.AddModel(&spheres[1]);

	coordinate.Reset();
	coordinate.o = LVec(0.0, 0.0, 0.0);
	coordinate.Rotate(0.0, 0.0, 0.0);
	camara.SetCoordinate(coordinate);
	camara.SetViewSize(0.01, 0.01);
	film.Resize(512, 512);
	render.pCamara = &camara;
	render.pFilm = &film;
	render.pModel = &models;
	render.sample_per_round = 2000;
	render.project_name = L"J20";
	render.StartRender(64);
	Image img;
	film.GetImage(&img);
	SetFrameImage(&img);
	//img.SaveToFile(L"save.png");
	system("pause");
	return 0;
}

LMediumParticipating med_jade;
LMediumParticipating med_air;
int main4()
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
	mat_disney_diffuse.SetColor(LColor(0, 162, 232) * (1.0 / 255.0));
	//mat_disney_diffuse.SetColor(LColor(64,0,128) * (1.0 / 255.0));
	mat_ref.SetColor(LColor(1, 1, 1));


	/*med_jade.SetAttenuation(LColor(4, 253, 11) * (1.0 / 255.0));
	med_jade.SetRefractionIndex(1.5);
	med_jade.absorb = 1 - 1e-8;//*/
	med_jade = LMediumParticipating(1.0 - 1e-8, LColor(0.8, 0.8, 0.8));
	med_jade.SetAttenuation(LColor(LColor(34, 253, 76) * (1.0 / 255.0)));
	med_jade.SetRefractionIndex(1.3);
	//med_jade.;

	med_air = LMediumParticipating(0.01, LColor(1.0, 1.0, 1.0));
	med_air.SetAttenuation(LColor(1.0, 1.0, 1.0));
	med_air.SetRefractionIndex(1.0);

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


	triangles[0] = LModelTriangle(LTriangle(LVec(-1.0, 2.0, -1.0), LVec(1.0, 2.0, 1.0), LVec(-1.0, 2.0, 1.0)), &mat_light);
	triangles[1] = LModelTriangle(LTriangle(LVec(-1.0, 2.0, -1.0), LVec(1.0, 2.0, 1.0), LVec(1.0, 2.0, -1.0)), &mat_light);
	triangles[2] = LModelTriangle(LTriangle(LVec(-1e4, -1.0, -1e4), LVec(1e4, -1.0, 1e4), LVec(-1e4, -1.0, 1e4)), &mat_ground);
	triangles[3] = LModelTriangle(LTriangle(LVec(-1e4, -1.0, -1e4), LVec(1e4, -1.0, 1e4), LVec(1e4, -1.0, -1e4)), &mat_ground);

	double window_size = 0.3,window_height=0.5;
	triangles[4] = LModelTriangle(LTriangle(LVec(-window_size, window_height, -window_size), LVec(-window_size, window_height, 1e4), LVec(-1e4, 1.0, -window_size)), &mat_ground);
	triangles[5] = LModelTriangle(LTriangle(LVec(-window_size, window_height, window_size), LVec(-window_size, window_height, 1e4), LVec(1e4, 1.0, window_size)), &mat_ground);
	triangles[6] = LModelTriangle(LTriangle(LVec(window_size, window_height, window_size), LVec(window_size, window_height, -1e4), LVec(1e4, 1.0, window_size)), &mat_ground);
	triangles[7] = LModelTriangle(LTriangle(LVec(window_size, window_height, -window_size), LVec(window_size, window_height, -1e4), LVec(-1e4, 1.0, -window_size)), &mat_ground);
	LMaterialLightSourcePureColor mat_weak_light;
	mat_weak_light.SetColor(LColor(1, 1, 1) * 2);
	triangles[8] = LModelTriangle(LTriangle(LVec(-1.0, 1.0, 1.0), LVec(-1.0, -1.0, -1.0), LVec(-1.0, 1.0, -1.0)), &mat_weak_light);
	triangles[9] = LModelTriangle(LTriangle(LVec(-1.0, 1.0, 1.0), LVec(-1.0, -1.0, -1.0), LVec(-1.0, -1.0, 1.0)), &mat_weak_light);
	//triangles[1] = LModelTriangle(LTriangle(LVec(-1.0, 1.0, -1.0), LVec(1.0, 1.0, 1.0), LVec(1.0, 1.0, -1.0)), &mat_ground);

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


	models.AddModel(&models_kd);//*/

	models.AddModel(&model_mesh);

	//models.AddModel(&triangles[0]);
	//models.AddModel(&triangles[1]);
	models.AddModel(&triangles[2]);
	models.AddModel(&triangles[3]);
	//models.AddModel(&triangles[4]);
	//models.AddModel(&triangles[5]);
	//models.AddModel(&triangles[6]);
	//models.AddModel(&triangles[7]);
	//models.AddModel(&triangles[8]);
	//models.AddModel(&triangles[9]);
	//models.SetMedium(&med_air);
	coordinate.Reset();
	coordinate.o = LVec(0.0, 0.0, -6.0);
	coordinate.Rotate(-L_PI*0.028, 0.0, 0.0);
	camara.SetCoordinate(coordinate);
	camara.SetViewSize(0.2, 0.2);
	film.Resize(2048, 2048);
	render.pCamara = &camara;
	render.pFilm = &film;
	render.pModel = &models;
	render.sample_per_round = 200;
	render.project_name = L"lucy-darker";
	render.StartRender(60);
	Image img;
	film.GetImage(&img);
	SetFrameImage(&img);
	//img.SaveToFile(L"save.png");
	system("pause");
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nShowCmd)
{
	//return outputwave();
	return main2();
}

void LMaterialSkyBox::Interaction(const LSurfaceInfo& surface, LRay* oray, double* Scale, LColor* pFilter, RandomDevice& randdevice, unsigned** ppIndexSeq, void* pReserved, UINT Floor) const
{

	InteractionLightSource(surface, oray);
	*Scale = 1.0;
	double brightness = surface.normal * LVec(0, -1, 0);
	brightness += 1.0;
	brightness *= 0.5;
	brightness = max(brightness, 0.0);
	*pFilter = LColor(51, 141, 251) * (1.0 / 255);// *brightness * 5.0;
}

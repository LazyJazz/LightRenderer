#include "LightMesh.h"

void LMesh::LoadObjFile(const char* filename, size_t no)
{
	ifstream filein(filename);
	char str[1024];
	uRoot = 0;
	uVertexCnt = 0;
	uNormalCnt = 0;
	uTextureCnt = 0;
	uFaceCnt = 0;
	size_t present_no = 0;
	while (filein.getline(str, 1024))
	{
		stringstream ss(str);
		string type;
		if (ss >> type)
		{
			if (type == "v")
				uVertexCnt++;
			else if (type == "vn")
				uNormalCnt++;
			else if (type == "vt")
				uTextureCnt++;
			else if (type == "f")
			{
				if (present_no != no && no)continue;
				string str_ind;
				while (ss >> str_ind)
					uFaceCnt++;
				uFaceCnt -= 2;
			}
			else if (type == "g")present_no++;
		}
	}
	vecMeshVertex.reserve((size_t)uVertexCnt + 1u);
	vecMeshVertex.resize((size_t)uVertexCnt + 1u);
	vecMeshNormal.reserve((size_t)uNormalCnt + 1u);
	vecMeshNormal.resize((size_t)uNormalCnt + 1u);
	vecMeshTexture.reserve((size_t)uTextureCnt + 1u);
	vecMeshTexture.resize((size_t)uTextureCnt + 1u);
	vecMeshFace.reserve((size_t)uFaceCnt + 1u);
	vecMeshFace.resize((size_t)uFaceCnt + 1u);
	cout << "Vertex Count: " << uVertexCnt << " Face Count: " << uFaceCnt << endl;
	cout << vecMeshVertex.size() << ' ' << vecMeshFace.size() << endl;
	cout << vecMeshVertex.capacity() << ' ' << vecMeshFace.capacity() << endl;
	filein.close();
	filein.open(filename);
	UINT cVertex = 1, cFace = 1, cNormal = 1, cTexture = 1;
	present_no = 0;
	while (filein.getline(str, 1024))
	{
		stringstream ss(str);
		string type;
		if (ss >> type)
		{
			if (type == "v")
			{
				double x, y, z;
				ss >> x >> y >> z;
				vecMeshVertex[cVertex++] = LVec(x, y, -z);
				//bounding_box.Update(LVec(x, y, z));
			}
			else if (type == "vn")
			{
				double x, y, z;
				ss >> x >> y >> z;
				vecMeshNormal[cNormal++] = LVec(x, y, -z);
				//bounding_box.Update(LVec(x, y, z));
			}
			else if (type == "vt")
			{
				double x, y, z;
				ss >> x >> y >> z;
				vecMeshTexture[cTexture++] = LVec(x, y, z);
				//bounding_box.Update(LVec(x, y, z));
			}
			else if (type == "f")
			{
				if (present_no != no && no)continue;
				string index_first, index;
				vector<string> index_vec;
				//ss >> index[0] >> index[1] >> index[2];
				ss >> index_first;
				while (ss >> index)index_vec.push_back(index);
				for (size_t i = 1; i < index_vec.size(); i++)vecMeshFace[cFace++] = LMeshFace(index_first, index_vec[i - 1], index_vec[i]);
			}
			else if (type == "g")present_no++;
		}
	}
	/*
	for (auto vertex : vecMeshVertex)
		cout << 'v' << ' ' << vertex.vertex.x << ' ' << vertex.vertex.y << ' ' << vertex.vertex.z << endl;
	for (auto face : vecMeshFace)
		cout << "f " << face.index[0] << ' ' << face.index[1] << ' ' << face.index[2] << endl;//*/

	bounding_box = LBoundingBox();
	for (size_t i = 1; i <= uVertexCnt; i++)bounding_box.Update(vecMeshVertex[i]);
	
}

#ifdef INTERSECTION_COUNTER
UINT intersection_cnter = 0;
#endif

void LMesh::GetIntersection(const LRay& iray, LIntersectionPack* pResult) const
{
#ifdef INTERSECTION_COUNTER
	intersection_cnter = 0;
#endif // INTERSECTION_COUNTER

	pResult->distance = L_INF;
	pResult->intersection = LVec();
	pResult->normal = LVec();
	pResult->model_normal = LVec();
	pResult->success = false;
	pResult->texture = LVec();
	GetIntersection(uRoot, iray, pResult);
	if (pResult->distance == L_INF || !pResult->success)
	{
		pResult->success = false;
		pResult->normal = -iray.v;
		pResult->model_normal = -iray.v;
		pResult->intersection = iray.o;
		pResult->distance = 0.0;
		pResult->texture = LVec();
	}
}

void LMesh::GetIntersectionCnt(const LRay& iray, UINT* cnt) const
{
	*cnt = 0;
	GetIntersectionCnt(uRoot, iray, cnt);
}

void LMesh::Move(LVec dirta)
{
	bounding_box = LBoundingBox();
	for (size_t i = 1; i <= uVertexCnt; i++)
		vecMeshVertex[i] += dirta, bounding_box.Update(vecMeshVertex[i]);
}

void LMesh::Rotate(double rx, double ry, double rz)
{
	bounding_box = LBoundingBox();
	for (size_t i = 1; i <= uVertexCnt; i++)
		vecMeshVertex[i] = vecMeshVertex[i].rotate(rx, ry, rz), bounding_box.Update(vecMeshVertex[i]);
}

void LMesh::Zoom(double scale)
{
	bounding_box = LBoundingBox();
	for (size_t i = 1; i <= uVertexCnt; i++)
		vecMeshVertex[i] *= scale, bounding_box.Update(vecMeshVertex[i]);
}

void LMesh::Normalize(double size)
{
	Zoom(size / max(max(bounding_box.v1.x - bounding_box.v0.x, bounding_box.v1.y - bounding_box.v0.y), bounding_box.v1.z - bounding_box.v0.z));
}

LVec LMesh::CenterPoint() const
{
	return (bounding_box.v0 + bounding_box.v1) * 0.5;
}

void LMesh::Build()
{
	uRoot = 0;
	for (auto& face : vecMeshFace)
	{
		face.triangle = LTriangle(
			vecMeshVertex[face.index_vertex[0]],
			vecMeshVertex[face.index_vertex[1]],
			vecMeshVertex[face.index_vertex[2]]
		);
		face.pos = face.triangle.CenterPoint();
		memset(face.ch, 0, sizeof face.ch);
	}
	vector<size_t> face_nodes;
	face_nodes.resize(uFaceCnt);
	for (size_t i = 0; i < uFaceCnt; i++)face_nodes[i] = i + 1;
	BuildKDTree(uRoot, face_nodes);
}

void LMesh::BuildKDTree(size_t& x, vector<size_t> vecFaceNode)
{
	x = 0;
	if (vecFaceNode.empty())return;
	vector<size_t> divide[8];
	vector<pair<size_t, size_t>> score;
	score.resize(vecFaceNode.size());
	for (size_t i = 0; i < vecFaceNode.size(); i++)score[i] = make_pair(0, vecFaceNode[i]);
	int center = vecFaceNode.size() / 2;
	//sort(vecFaceNode.begin(), vecFaceNode.end(), [this](size_t u, size_t v)->bool {return this->vecMeshFace[u].pos.x < this->vecMeshFace[v].pos.x; });
	sort(score.begin(), score.end(), [this](pair<size_t, size_t> u, pair<size_t, size_t> v)->bool {return this->vecMeshFace[u.second].pos.x < this->vecMeshFace[v.second].pos.x; });
	for (int i = 0; i < vecFaceNode.size(); i++)score[i].first += abs(i - center);
	//sort(vecFaceNode.begin(), vecFaceNode.end(), [this](size_t u, size_t v)->bool {return this->vecMeshFace[u].pos.y < this->vecMeshFace[v].pos.y; });
	sort(score.begin(), score.end(), [this](pair<size_t, size_t> u, pair<size_t, size_t> v)->bool {return this->vecMeshFace[u.second].pos.y < this->vecMeshFace[v.second].pos.y; });
	for (int i = 0; i < vecFaceNode.size(); i++)score[i].first += abs(i - center);
	//sort(vecFaceNode.begin(), vecFaceNode.end(), [this](size_t u, size_t v)->bool {return this->vecMeshFace[u].pos.z < this->vecMeshFace[v].pos.z; });
	sort(score.begin(), score.end(), [this](pair<size_t, size_t> u, pair<size_t, size_t> v)->bool {return this->vecMeshFace[u.second].pos.z < this->vecMeshFace[v.second].pos.z; });
	for (int i = 0; i < vecFaceNode.size(); i++)score[i].first += abs(i - center);
	sort(score.begin(), score.end());
	x = score[0].second;
	vecMeshFace[x].bounding_box = vecMeshFace[x].triangle.GetBoundingBox();
	for (size_t i = 1; i < score.size(); i++)
	{
		size_t node = score[i].second;
		divide[RelativeIndex(vecMeshFace[x].pos, vecMeshFace[node].pos)].push_back(node);
	}
	for (size_t direct = 0; direct < 8; direct++)
	{
		BuildKDTree(vecMeshFace[x].ch[direct], divide[direct]);
		if (vecMeshFace[x].ch[direct])vecMeshFace[x].bounding_box += vecMeshFace[vecMeshFace[x].ch[direct]].bounding_box;
	}
}

void LMesh::GetIntersection(size_t x, const LRay& iray, LIntersectionPack* pResult) const
{
 // INTERSECTION_COUNTER
	if (!x)return;
#ifdef INTERSECTION_COUNTER
	intersection_cnter++;
#endif
	/*double bbres = vecMeshFace[x].bounding_box.RayDetect(iray);
	vecMeshFace[x].bounding_box.v0.Print();
	vecMeshFace[x].bounding_box.v1.Print();
	puts("");
	iray.o.Print();
	iray.v.Print();
	printf(" %lf\n",bbres);//*/
	if (vecMeshFace[x].bounding_box.RayDetect(iray) >= pResult->distance)
	{
//		puts("Cut!!!");
		return;
	}
	bool success;
	LVec inter;
	double scale;
	vecMeshFace[x].triangle.GetIntersection(iray, &success, &inter, &scale);
	if (success)
	{
		if (scale < pResult->distance)
		{
			pResult->distance = scale;
			pResult->success = success;
			pResult->intersection = inter;
			LVec normal, n[3], scalev, texture;
			for (size_t i = 0; i < 3; i++)
			{
				n[i] = vecMeshNormal[vecMeshFace[x].index_normal[i]];
				if (n[i].iszero())n[i] = vecMeshFace[x].triangle.pl.n;
				if (n[i] * iray.v > 0.0)n[i] = -n[i];
			}
			scalev = vecMeshFace[x].triangle.ScaleVec(inter);
			normal = n[0] * scalev.x + n[1] * scalev.y + n[2] * scalev.z;
			if (normal.iszero())normal = vecMeshFace[x].triangle.pl.n;
			if (normal * iray.v > 0.0)normal = -normal;
			texture = 
				vecMeshTexture[vecMeshFace[x].index_texture[0]] * scalev.x +
				vecMeshTexture[vecMeshFace[x].index_texture[1]] * scalev.y +
				vecMeshTexture[vecMeshFace[x].index_texture[2]] * scalev.z;
			pResult->normal = normal;
			pResult->model_normal = vecMeshFace[x].triangle.pl.n;
			if (pResult->model_normal * iray.v > 0.0)pResult->model_normal = -pResult->model_normal;
			pResult->texture = texture;
		}
	}
	size_t direct = RelativeIndex(vecMeshFace[x].pos, iray.o);
	GetIntersection(vecMeshFace[x].ch[direct], iray, pResult);
	GetIntersection(vecMeshFace[x].ch[direct ^ 1], iray, pResult);
	GetIntersection(vecMeshFace[x].ch[direct ^ 2], iray, pResult);
	GetIntersection(vecMeshFace[x].ch[direct ^ 4], iray, pResult);
	GetIntersection(vecMeshFace[x].ch[direct ^ 6], iray, pResult);
	GetIntersection(vecMeshFace[x].ch[direct ^ 5], iray, pResult);
	GetIntersection(vecMeshFace[x].ch[direct ^ 3], iray, pResult);
	GetIntersection(vecMeshFace[x].ch[direct ^ 7], iray, pResult);
}

void LMesh::GetIntersectionCnt(size_t x, const LRay& iray, UINT* cnt) const
{
	if (!x)return;
	if (vecMeshFace[x].bounding_box.RayDetect(iray) >= L_INF)return;
	bool success;
	LVec inter;
	double scale;
	vecMeshFace[x].triangle.GetIntersection(iray, &success, &inter, &scale);
	*cnt += success;
	size_t direct = RelativeIndex(vecMeshFace[x].pos, iray.o);
	GetIntersectionCnt(vecMeshFace[x].ch[direct], iray, cnt);
	GetIntersectionCnt(vecMeshFace[x].ch[direct ^ 1], iray, cnt);
	GetIntersectionCnt(vecMeshFace[x].ch[direct ^ 2], iray, cnt);
	GetIntersectionCnt(vecMeshFace[x].ch[direct ^ 4], iray, cnt);
	GetIntersectionCnt(vecMeshFace[x].ch[direct ^ 6], iray, cnt);
	GetIntersectionCnt(vecMeshFace[x].ch[direct ^ 5], iray, cnt);
	GetIntersectionCnt(vecMeshFace[x].ch[direct ^ 3], iray, cnt);
	GetIntersectionCnt(vecMeshFace[x].ch[direct ^ 7], iray, cnt);
}

size_t LMesh::RelativeIndex(LVec o, LVec p) const
{
	size_t res = 0;
	if (p.x > o.x)res |= 1;
	if (p.y > o.y)res |= 2;
	if (p.z > o.z)res |= 4;
	return res;
}

LMeshFace::LMeshFace()
{
	index_vertex[0] = index_vertex[1] = index_vertex[2] = 0;
	index_normal[0] = index_normal[1] = index_normal[2] = 0;
	index_texture[0] = index_texture[1] = index_texture[2] = 0;
	triangle = LTriangle();
	bounding_box = LBoundingBox();
	memset(ch, 0, sizeof ch);
}

LMeshFace::LMeshFace(string index0, string index1, string index2)
{
	TranslateIndex(index0, index_vertex[0], index_normal[0], index_texture[0]);
	TranslateIndex(index1, index_vertex[1], index_normal[1], index_texture[1]);
	TranslateIndex(index2, index_vertex[2], index_normal[2], index_texture[2]);
	pos = LVec();
	triangle = LTriangle();
	bounding_box = LBoundingBox();
	memset(ch, 0, sizeof ch);
}

void LMeshFace::TranslateIndex(string index, UINT& vertex, UINT& normal, UINT& texture)
{
	string part[3];
	UINT ipart = 0;
	for (size_t i = 0; i < index.size(); i++)
	{
		if (index[i] == '/')ipart++;
		else part[ipart] += index[i];
	}
	if (part[0] != "")
	{
		stringstream ss(part[0]);
		ss >> vertex;
	}
	else vertex = 0;
	if (part[1] != "")
	{
		stringstream ss(part[1]);
		ss >> texture;
	}
	else texture = 0;
	if (part[2] != "")
	{
		stringstream ss(part[2]);
		ss >> normal;
	}
	else normal = 0;
}

#ifdef INTERSECTION_COUNTER
UINT GetIntersectionCounter()
{
	return intersection_cnter;
}
#else
UINT GetIntersectionCounter()
{
	return 0xffffffff;
}
#endif // INTERSECTION_COUNTER

LModelMesh::LModelMesh()
{
	mesh = NULL;
	material = NULL;
}

LModelMesh::LModelMesh(LMesh* pMesh, LMaterial* pMaterial)
{
	mesh = pMesh;
	material = pMaterial;
}

bool LModelMesh::GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const
{
	LIntersectionPack interaction_pack;
	mesh->GetIntersection(iray, &interaction_pack);
	if (interaction_pack.success)
	{
		pSurfaceInfo->intersection = interaction_pack.intersection;
		pSurfaceInfo->distance = interaction_pack.distance;
		pSurfaceInfo->material = material;
		pSurfaceInfo->normal = interaction_pack.normal;
		if (pSurfaceInfo->normal * iray.v > 0)pSurfaceInfo->normal = -pSurfaceInfo->normal;
		pSurfaceInfo->model_normal = interaction_pack.model_normal;
		pSurfaceInfo->ray = iray;
		pSurfaceInfo->transformed_pos = interaction_pack.texture;
	}
	return interaction_pack.success;
}

void LModelMesh::GetMedium(LRay iray, double time, LMedium** pMedium) const
{
	if (!medium||!mesh->bounding_box.isInside(iray.o))
	{
		*pMedium = NULL;
		return;
	}
	UINT cnt = 0; mesh->GetIntersectionCnt(iray, &cnt);
	if (cnt & 1)* pMedium = medium;
	else *pMedium = NULL;
}

LModelTransform::LModelTransform()
{
	coordinate = LCoordinate();
	model = NULL;
}

LModelTransform::LModelTransform(LCoordinate cdn, LModel* pModel)
{
	coordinate = cdn;
	model = pModel;
}

bool LModelTransform::GetIntersection(LRay iray, double time, LSurfaceInfo* pSurfaceInfo) const
{
	//LRay tray = LRay(coordinate.PutIn(iray.o), coordinate.PutIn(iray.o + iray.v) - coordinate.PutIn(iray.o));
	bool res = model->GetIntersection(coordinate.PutIn(iray), time, pSurfaceInfo);
	if (res)
	{
		pSurfaceInfo->normal = coordinate.GetOut(pSurfaceInfo->intersection + pSurfaceInfo->normal);
		pSurfaceInfo->model_normal = coordinate.GetOut(pSurfaceInfo->intersection + pSurfaceInfo->model_normal);
		pSurfaceInfo->intersection = coordinate.GetOut(pSurfaceInfo->intersection);
		pSurfaceInfo->normal -= pSurfaceInfo->intersection; pSurfaceInfo->normal = pSurfaceInfo->normal.zoom(1.0);
		pSurfaceInfo->model_normal -= pSurfaceInfo->intersection; pSurfaceInfo->model_normal = pSurfaceInfo->model_normal.zoom(1.0);
		pSurfaceInfo->distance = (pSurfaceInfo->intersection - iray.o).mod();
		pSurfaceInfo->ray = coordinate.GetOut(pSurfaceInfo->ray);
	}
	return res;
}

void LModelTransform::GetMedium(LRay iray, double time, LMedium** pMedium) const
{
	model->GetMedium(coordinate.PutIn(iray), time, pMedium);
}

#include "LightGeometry.h"

LTriangle::LTriangle()
{
	v0 = v1 = v2 = LVec();
	pl = LPlane();
}

LTriangle::LTriangle(LVec _v0, LVec _v1, LVec _v2)
{
	v0 = _v0;
	v1 = _v1;
	v2 = _v2;
	if (v0 != v1 && v1 != v2 && v0 != v2)
		pl = LPlane(v0, v1, v2);
	else
		pl = LPlane();
}

bool LTriangle::islegal() const
{
	return v0.isfinite() && v1.isfinite() && v2.isfinite() && v0 != v1 && v1 != v2 && v0 != v2 && pl.islegal();
}

void LTriangle::GetIntersection(const LRay& ray, bool* pbSuccess, LVec* pvResult, double* pdScale) const
{
	assert(islegal());
	assert(ray.islegal());
	bool suc;
	LVec res;
	double sca;
	::GetIntersection(pl, ray, &suc, &res, &sca);
	if (suc)
	{
		if (!OnTriangle(res))
		{
			suc = false;
			sca = 0.0;
			res = ray.o;
		}
	}
	if (pbSuccess)* pbSuccess = suc;
	if (pvResult)* pvResult = res;
	if (pdScale)* pdScale = sca;
}

bool LTriangle::OnTriangle(LVec v) const
{
	return diff(angle(v0 - v, v1 - v) + angle(v2 - v, v1 - v) + angle(v0 - v, v2 - v), 2 * L_PI) < 1e-7;
}

LBoundingBox LTriangle::GetBoundingBox() const
{
	return LBoundingBox(v0) + LBoundingBox(v1) + LBoundingBox(v2);
}

LVec LTriangle::ScaleVec(LVec v) const
{
	assert(islegal());
	assert(v.isfinite());
	LRay ray; LVec vm, vv, res;
	ray = LRay(v1, v2 - v1);
	vm = ray & v0;
	vv = v0 - vm;
	res.x = (v - vm) * vv / vv.mod2();
	ray = LRay(v0, v2 - v0);
	vm = ray & v1;
	vv = v1 - vm;
	res.y = (v - vm) * vv / vv.mod2();
	ray = LRay(v1, v0 - v1);
	vm = ray & v2;
	vv = v2 - vm;
	res.z = (v - vm) * vv / vv.mod2();
	return res;
}

LVec LTriangle::CenterPoint() const
{
	return (v0 + v1 + v2) * (1.0 / 3.0);
}

LBoundingBox::LBoundingBox()
{
	v0 = LVec(L_INF, L_INF, L_INF);
	v1 = LVec(-L_INF, -L_INF, -L_INF);
}

LBoundingBox::LBoundingBox(LVec vec)
{
	v0 = v1 = vec;
}

bool LBoundingBox::islegal() const
{
	return v0.isfinite() && v1.isfinite() && v0.x <= v1.x && v0.y <= v1.y && v0.z <= v1.z;
}

bool LBoundingBox::isInside(LVec vec) const
{
	return v0.x <= vec.x && vec.x <= v1.x
		&& v0.y <= vec.y && vec.y <= v1.y
		&& v0.z <= vec.z && vec.z <= v1.z;
}

void LBoundingBox::Update(LVec vec)
{
	v0.x = min(v0.x, vec.x);
	v0.y = min(v0.y, vec.y);
	v0.z = min(v0.z, vec.z);
	v1.x = max(v1.x, vec.x);
	v1.y = max(v1.y, vec.y);
	v1.z = max(v1.z, vec.z);
}

double LBoundingBox::RayDetect(LRay ray) const
{
	if (!islegal())return false;
	int inside = 0;
	LVec bound[2] = { v0,v1 };
	for (int i = 0; i < 3; i++)
	{
		if (i == 1)
		{
			swap(bound[0].x, bound[0].y);
			swap(bound[1].x, bound[1].y);
			swap(ray.o.x, ray.o.y);
			swap(ray.v.x, ray.v.y);
		}
		else if (i == 2)
		{
			swap(bound[0].x, bound[0].z);
			swap(bound[1].x, bound[1].z);
			swap(ray.o.x, ray.o.z);
			swap(ray.v.x, ray.v.z);
		}
		LVec inter;
		double distance;
		if (ray.o.x < bound[0].x)
			if (ray.v.x <= 0.0)continue;
			else distance = (bound[0].x - ray.o.x) / ray.v.x;
		else if (ray.o.x <= bound[1].x) { inside++; continue; }
		else
			if (ray.v.x >= 0.0)continue;
			else distance = (bound[1].x - ray.o.x) / ray.v.x;
		inter = ray.o + ray.v * distance;
		if (inter.isfinite())
			if (bound[0].y <= inter.y && inter.y <= bound[1].y && bound[0].z <= inter.z && inter.z <= bound[1].z)return distance;
	}
	return (inside == 3) ? 0.0 : L_INF;
}

LBoundingBox LBoundingBox::operator+(const LBoundingBox& b) const
{
	LBoundingBox ret = *this;
	ret.Update(b.v0);
	ret.Update(b.v1);
	return ret;
}

LBoundingBox& LBoundingBox::operator+=(const LBoundingBox& b)
{
	Update(b.v0);
	Update(b.v1);
	return *this;
}

LCoordinate::LCoordinate()
{
	x = LVec(1, 0, 0);
	y = LVec(0, 1, 0);
	z = LVec(0, 0, 1);
	o = LVec();
}

LCoordinate::LCoordinate(double rx, double ry, double rz)
{
	x = LVec(1, 0, 0).rotate(rx, ry, rz);
	y = LVec(0, 1, 0).rotate(rx, ry, rz);
	z = LVec(0, 0, 1).rotate(rx, ry, rz);
	o = LVec();
}

void LCoordinate::Reset()
{
	x = LVec(1, 0, 0);
	y = LVec(0, 1, 0);
	z = LVec(0, 0, 1);
	o = LVec();
}

void LCoordinate::Move(LVec dirta)
{
	o += dirta;
}

void LCoordinate::MoveRelative(LVec dirta)
{
	o += dirta.x * x + dirta.y * y + dirta.z * z;
}

void LCoordinate::Rotate(double rx, double ry, double rz)
{
	x = x.rotate(rx, ry, rz);
	y = y.rotate(rx, ry, rz);
	z = z.rotate(rx, ry, rz);
}

void LCoordinate::Zoom(double scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
}

LVec LCoordinate::PutIn(LVec v) const
{
	return v.x* x + v.y * y + v.z * z + o;
}

LVec LCoordinate::GetOut(LVec v) const
{
	v -= o;
	return LVec(v * x / x.mod2(), v * y / y.mod2(), v * z / z.mod2());
}

LRay LCoordinate::PutIn(LRay ray) const
{
	LVec o = PutIn(ray.o), v = PutIn(ray.v + ray.o);
	return LRay(o, v - o);
}

LRay LCoordinate::GetOut(LRay ray) const
{
	LVec o = GetOut(ray.o), v = GetOut(ray.v + ray.o);
	return LRay(o, v - o);
}

LSphere::LSphere()
{
	o = LVec();
	radius = 0.0;
}

LSphere::LSphere(LVec _o, double _radius)
{
	o = _o;
	radius = _radius;
}

bool LSphere::islegal() const
{
	return o.isfinite() && isfinite(radius);
}

void LSphere::GetIntersection(const LRay& ray, bool* pbSuccess, LVec* pvResult, double* pdScale) const
{
	assert(islegal());
	assert(ray.islegal());
	bool success = false;
	double scale = 0.0;
	LVec res = ray.o;
	LVec vertical = (ray & o);
	double dist = (vertical - o).mod();
	if (dist < radius)
	{
		dist = sqrt(radius * radius - dist * dist);
		scale = (vertical - ray.o) * ray.v;
		if (scale - dist > L_EPS)
		{
			scale -= dist;
			success = true;
		}
		else if (scale + dist > L_EPS)
		{
			scale += dist;
			success = true;
		}
	}
	if (!success)scale = 0.0;
	else res = ray.o + ray.v * scale;
	if (pbSuccess)* pbSuccess = success;
	if (pvResult)* pvResult = res;
	if (pdScale)* pdScale = scale;
}

LBoundingBox LSphere::GetBoundingBox() const
{
	return LBoundingBox(LVec(o.x - radius, o.y - radius, o.z - radius)) + LBoundingBox(LVec(o.x + radius, o.y + radius, o.z + radius));
}

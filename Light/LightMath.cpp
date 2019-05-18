#include "LightMath.h"

LVec::LVec()
{
	x = y = z = 0;
}

LVec::LVec(double _x, double _y, double _z)
{
	x = _x; y = _y; z = _z;
}

LVec LVec::operator+(const LVec& b) const
{
	return LVec(x + b.x, y + b.y, z + b.z);
}

LVec LVec::operator-() const
{
	return LVec(-x, -y, -z);
}

LVec LVec::operator-(const LVec& b) const
{
	return LVec(x - b.x, y - b.y, z - b.z);
}

double LVec::operator*(const LVec& b) const
{
	return x * b.x + y * b.y + z * b.z;
}

LVec LVec::operator/(const double& b) const
{
	return LVec(x / b, y / b, z / b);
}

LVec LVec::operator^(const LVec& b) const
{
	return LVec(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
}

double LVec::mod() const
{
	return sqrt(x * x + y * y + z * z);
}

double LVec::mod2() const
{
	return x * x + y * y + z * z;
}

LVec LVec::zoom(double l) const
{
	assert(!iszero());
	return *this* (l / mod());
}

LVec LVec::rotate(double rx, double ry, double rz) const
{
	LVec ret = *this;
	::rotate(ret.x, ret.y, rz);
	::rotate(ret.y, ret.z, rx);
	::rotate(ret.x, ret.z, ry);
	return ret;
}

bool LVec::iszero() const
{
	return mod2() <= L_EPS * L_EPS;
}

bool LVec::isnan() const
{
	return ::isnan(x) || ::isnan(y) || ::isnan(z);
}

bool LVec::isnormal() const
{
	return !isnan() && !iszero();
}

bool LVec::isfinite() const
{
	return ::isfinite(x) && ::isfinite(y) && ::isfinite(z);
}

bool LVec::isinf() const
{
	return !isnan() && (::isinf(x) || ::isinf(y) || ::isinf(z));
}

bool LVec::operator==(const LVec& b) const
{
	return (*this - b).iszero();
}

bool LVec::operator!=(const LVec& b) const
{
	return !(*this - b).iszero();
}

bool LVec::operator<(const double& b) const
{
	return mod2() < b * b;
}

bool LVec::operator<=(const double& b) const
{
	return mod2() <= b * b;
}

bool LVec::operator>(const double& b) const
{
	return mod2() > b* b;
}

bool LVec::operator>=(const double& b) const
{
	return mod2() >= b * b;
}

LVec& LVec::operator+=(const LVec& b)
{
	return *this = *this + b;
}

LVec& LVec::operator-=(const LVec& b)
{
	return *this = *this - b;
}

LVec& LVec::operator*=(const double& b)
{
	return *this = *this * b;
}

LVec& LVec::operator/=(const double& b)
{
	return *this = *this / b;
}

LVec& LVec::operator^=(const LVec& b)
{
	return *this = *this ^ b;
}

bool LVec::operator||(const LVec& b) const
{
	return operator^(b).iszero();
}

bool LVec::isvertical(const LVec& b) const
{
	return ::iszero(operator*(b));
}

bool LVec::isparallel(const LVec& b) const
{
	return operator||(b);
}

void LVec::Print() const
{
	printf_s("(%lf,%lf,%lf)", x, y, z);
}

LVec operator*(const LVec& v, const double& d)
{
	return LVec(v.x * d, v.y * d, v.z * d);
}

LVec operator*(const double& d, const LVec& v)
{
	return LVec(v.x * d, v.y * d, v.z * d);
}

LVec operator&(const LPlane& pl, const LVec& v)
{
	assert(pl.islegal());
	assert(!v.isnan());
	return v - pl.n * (pl.n * v + pl.d);
}

LVec operator&(const LVec& v, const LPlane& pl)
{
	assert(pl.islegal());
	assert(!v.isnan());
	return v - pl.n * (pl.n * v + pl.d);
}

LPlane operator|(const LPlane& pl, const LVec& v)
{
	LPlane ret;
	ret.n = pl.n;
	ret.d = -(pl.n * v);
	return ret;
}

LPlane operator|(const LVec& v, const LPlane& pl)
{
	LPlane ret;
	ret.n = pl.n;
	ret.d = -(pl.n * v);
	return ret;
}

bool operator&&(const LVec& b, const LPlane& pl)
{
	return abs(pl.verify(b)) < L_EPS;
}

bool operator&&(const LPlane& pl, const LVec& b)
{
	return abs(pl.verify(b)) < L_EPS;
}

bool operator||(const LVec& b, const LPlane& pl)
{
	return abs(b * pl.n) < L_EPS;
}

bool operator||(const LPlane& pl, const LVec& b)
{
	return abs(b * pl.n) < L_EPS;
}

bool operator||(const LPlane& pl, const LRay& ray)
{
	return pl || ray.v;
}

bool operator||(const LRay& ray, const LPlane& pl)
{
	return pl || ray.v;
}

bool operator&&(const LPlane& pl, const LRay& ray)
{
	if (pl || ray)return false;
	double scale = pl.verify(ray.o) / -(ray.v * pl.n);
	return scale >= 0.0;
}

bool operator&&(const LRay& ray, const LPlane& pl)
{
	return pl && ray;
}

LVec operator&(const LPlane& pl, const LRay& ray)
{
	assert(pl.islegal());
	assert(ray.islegal());
	assert(!(pl || ray));
	double scale = pl.verify(ray.o) / -(ray.v * pl.n);
	//double scale = (pl.n * ray.o + pl.d) / -(ray.v * pl.n); //算出射线端点到平面的沿射线方向的距离
	assert(scale >= 0.0);
	scale = max(scale, 0.0);
	return ray.o + ray.v * scale;
}

LVec operator&(const LRay& ray, const LPlane& pl)
{
	return pl & ray;
}

LVec operator&(const LRay& ray, const LVec& vec)
{
	assert(ray.islegal());
	assert(!vec.isnan());
	return ray.o + ray.v * (ray.v * vec - ray.v * ray.o);
}

LVec operator&(const LVec& vec, const LRay& ray)
{
	return ray & vec;
}

bool operator||(const LPlane& pl, const LLine& line)
{
	return pl || line._vec();
}

bool operator||(const LLine& line, const LPlane& pl)
{
	return pl || line._vec();
}

bool operator&&(const LPlane& pl, const LLine& line)
{
	return !(pl || line._vec());
}

bool operator&&(const LLine& line, const LPlane& pl)
{
	return !(pl || line._vec());
}

LVec operator&(const LPlane& pl, const LLine& line)
{
	assert(pl.islegal());
	assert(line.islegal());
	assert(pl && line);
	LVec v = line.v1 - line.v0;
	double scale = -pl.verify(line.v0) / (v * pl.n);
	return scale * v + line.v0;
}

LVec operator&(const LLine& line, const LPlane& pl)
{
	return pl & line;
}

double diff(double a, double b)
{
	return abs(a - b);
}

double iszero(double d)
{
	return abs(d) < L_EPS;
}

void rotate(double& x, double& y, double rad)
{
	double S = sin(rad), C = cos(rad);
	double X, Y;
	X = C * x + S * y;
	Y = C * y - S * x;
	x = X; y = Y;
}

double angle(LVec v0, LVec v1)
{
	assert(!v0.iszero());
	assert(!v1.iszero());
	return abs(acos(min(v0 * v1 / v0.mod() / v1.mod(), 1.0)));
}

void GetIntersection(const LPlane& pl, const LRay& ray, bool* pbSuccess, LVec* pvResult, double* pdScale)
{
	assert(pl.islegal());
	assert(ray.islegal());
	bool success = true;
	double scale;
	if (pl || ray)scale = 0.0, success = false;
	else scale = pl.verify(ray.o) / -(ray.v * pl.n);
	if (scale < L_EPS)success = false;
	scale = max(scale, 0.0);
	LVec result = ray.o + ray.v * scale;
	if (pbSuccess)* pbSuccess = success;
	if (pvResult)* pvResult = result;
	if (pdScale)* pdScale = scale;
}

LVec RandInSphere(RandomDevice& randdevice)
{
	LVec res;
	do
	{
		res = LVec(
			randdevice.rand_double(-1.0, 1.0),
			randdevice.rand_double(-1.0, 1.0),
			randdevice.rand_double(-1.0, 1.0)
			);
	} while (res > 1.0 || res.iszero());
	return res;
}

LVec RandOnSphere(RandomDevice& randdevice)
{
	double alpha = randdevice.rand_double(-L_PI, L_PI);
	LVec res;
	res.y = randdevice.rand_double(-1, 1);
	res.x = sqrt(1.0 - res.y * res.y);
	res.z = sin(alpha) * res.x;
	res.x *= cos(alpha);
	return res;
}

LVec RandOnSphere(unsigned& index)
{
	index++;
	double alpha = Halton(0, index) * 2 * L_PI - L_PI;
	LVec res;
	res.y = Halton(1, index) * 2 - 1;
	res.x = sqrt(1.0 - res.y * res.y);
	res.z = sin(alpha) * res.x;
	res.x *= cos(alpha);
	return res;
}

//4*(1-costheta^2)*(1-k*costheta)
//(1-costheta)*(1-k)*2*PI/3

LVec RandOnSphereCrown(RandomDevice& randdevice, double costheta)
{
	double alpha = randdevice.rand_double(-L_PI, L_PI);
	LVec res;
	res.y = randdevice.rand_double(costheta, 1);
	res.x = sqrt(1.0 - res.y * res.y);
	res.z = sin(alpha) * res.x;
	res.x *= cos(alpha);
	return res;
}

LVec RandOnSphereCrown(RandomDevice& randdevice, double costheta, LVec centual)
{
	assert(!centual.iszero());
	return BindVec(centual, RandOnSphereCrown(randdevice, costheta));
	centual = centual.zoom(1.0);
	LVec res = RandOnSphereCrown(randdevice, costheta), x, z;
	x = LVec(-centual.z, 0, centual.x);
	if (x.iszero())
		return res * centual.y;
	x = x.zoom(1.0);
	z = x ^ centual;
	return res.x* x + res.y * centual + res.z * z;
}

LVec RandOnSphereCrown(unsigned& index, double costheta)
{
	index++;
	double alpha = Halton(0, index) * 2 * L_PI - L_PI;
	LVec res;
	res.y = Halton(1, index) * (1 - costheta) + costheta;// randdevice.rand_double(costheta, 1);
	res.x = sqrt(1.0 - res.y * res.y);
	res.z = sin(alpha) * res.x;
	res.x *= cos(alpha);
	return res;
}

LVec RandOnSphereCrown(unsigned& index, double costheta, LVec centual)
{
	assert(!centual.iszero());
	return BindVec(centual, RandOnSphereCrown(index, costheta));
	centual = centual.zoom(1.0);
	LVec res = RandOnSphereCrown(index, costheta), x, z;
	x = LVec(-centual.z, 0, centual.x);
	if (x.iszero())
		return res * centual.y;
	x = x.zoom(1.0);
	z = x ^ centual;
	return res.x* x + res.y * centual + res.z * z;
}

LVec BindVec(LVec centual, LVec v)
{
	centual = centual.zoom(1.0);
	LVec res = v, x, z;
	x = LVec(-centual.z, 0, centual.x);
	if (x.iszero())
		return res * centual.y;
	x = x.zoom(1.0);
	z = x ^ centual;
	return res.x* x + res.y * centual + res.z * z;
}

LPlane::LPlane()
{
	n = LVec();
	d = 0.0;
}

LPlane::LPlane(LVec _n, double _d)
{
	double scale = _n.mod();
	assert(scale > L_EPS);
	n = _n / scale;
	d = _d / scale;
}

LPlane::LPlane(LVec v0, LVec v1, LVec v2)
{
	v1 -= v0;
	v2 -= v0;
	n = v1 ^ v2;
	if (!n.iszero())
		n = n.zoom(1.0), d = -(n * v0);
	else
		n = LVec(), d = 0.0;
}

bool LPlane::islegal() const
{
	return n.isnormal() && isfinite(d) && (n.mod2() - 1.0) <= L_EPS;
}

double LPlane::verify(LVec v) const
{
	return v * n + d;
}

bool LPlane::operator||(const LPlane& b) const
{
	return (n ^ b.n).iszero();
}

bool LPlane::operator&&(const LPlane& b) const
{
	return !(n ^ b.n).iszero();
}

bool LPlane::isvertical(const LPlane& pl) const
{
	return n.isvertical(pl.n);
}

bool LPlane::isparallel(const LPlane& pl) const
{
	return n.isparallel(pl.n);
}

bool LPlane::iscross(const LPlane& pl) const
{
	return !isparallel(pl);
}

bool LPlane::isvertical(const LVec& v) const
{
	return v.isparallel(n);
}

bool LPlane::isparallel(const LVec& v) const
{
	return v.isvertical(n);
}

bool LPlane::iscross(const LVec& v) const
{
	return *this&& v;
}

void LPlane::Print() const
{
	printf("%lfx+%lfy+%lfz+%lf=0", n.x, n.y, n.z, d);
}

LRay::LRay()
{
	o = LVec();
	v = LVec();
}

LRay::LRay(LVec _o, LVec _v)
{
	assert(!_o.isnan());
	assert(!_v.iszero());
	o = _o;
	v = _v.zoom(1.0);
}

bool LRay::islegal() const
{
	return !o.isnan() && !v.isnan() && abs(v.mod2() - 1.0) < L_EPS;
}

bool LRay::isvertical(const LPlane& pl) const
{
	return v.isparallel(pl.n);
}

bool LRay::isparallel(const LPlane& pl) const
{
	return v.isvertical(pl.n);
}

bool LRay::iscross(const LPlane& pl) const
{
	return *this&& pl;
}

LVec LRay::_vec() const
{
	return v;
}

LLine LRay::_line() const
{
	return LLine(o, o + v);
}

LLine::LLine()
{
	v0 = v1 = LVec();
}

LLine::LLine(LVec _v0, LVec _v1)
{
	v0 = _v0;
	v1 = _v1;
}

LLine::LLine(LRay ray)
{
	v0 = ray.o;
	v1 = ray.v + ray.o;
}

bool LLine::islegal() const
{
	return !v0.isnan() && !v1.isnan() && v0 != v1;
}

bool LLine::isvertical(const LPlane& pl) const
{
	return pl.isvertical(_vec());
}

bool LLine::isparallel(const LPlane& pl) const
{
	return pl.isparallel(_vec());
}

bool LLine::iscross(const LPlane& pl) const
{
	return !pl.isparallel(_vec());
}

LVec LLine::_vec() const
{
	return LVec(v1 - v0);
}

LRay LLine::_ray() const
{
	return LRay(v0, v1 - v0);
}

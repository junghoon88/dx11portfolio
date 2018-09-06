#include "stdafx.h"
#include "Math.h"

const float Math::PI = 3.14159265f;
const float Math::TwoPI   = 6.28318531f;
const float Math::PiOver4 = 0.7853982f;

const float Math::FloatMinValue = -3E+38f;
//const float Math::Epsilon = 1.401298E-45f;
const float Math::FloatMaxValue = 3E+38f;
//const float Math::PositiveInfinity = 1 / Epsilon;
//const float Math::NegativeInfinity = -1 / Epsilon;
//const float Math::NaN = PositiveInfinity / PositiveInfinity;


int Math::Min(int n1, int n2)
{
	return (n1 <= n2) ? n1 : n2;
}

int Math::Max(int n1, int n2)
{
	return (n1 >= n2) ? n1 : n2;
}

float Math::Min(float f1, float f2)
{
	return (f1 <= f2) ? f1 : f2;
}

float Math::Max(float f1, float f2)
{
	return (f1 >= f2) ? f1 : f2;
}

void Math::SwapMinMax(int * min, int * max)
{
	if (*min > *max)
	{
		int temp = *min;
		*min = *max;
		*max = temp;
	}
}

void Math::SwapMinMax(LONG * min, LONG * max)
{
	if (*min > *max)
	{
		LONG temp = *min;
		*min = *max;
		*max = temp;
	}
}

void Math::SwapMinMax(float* min, float* max)
{
	if (*min > *max)
	{
		float temp = *min;
		*min = *max;
		*max = temp;
	}
}

float Math::Length(D3DXVECTOR3 vec)
{
	double l = vec.x*vec.x + vec.y*vec.y + vec.z*vec.z;
	return (float)sqrt(l);
}

float Math::LengthSquared(D3DXVECTOR3 vec)
{
	return (vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

float Math::Modulo(float val1, float val2)
{
	while (val1 - val2 >= 0)
		val1 -= val2;

	return val1;
}

float Math::ToRadian(float degree)
{
	return degree * PI / 180.0f;
}

D3DXVECTOR3 Math::ToRadian(D3DXVECTOR3 vec)
{
	D3DXVECTOR3 res;
	res.x = vec.x * PI / 180.0f;
	res.y = vec.y * PI / 180.0f;
	res.z = vec.z * PI / 180.0f;
	return res;
}

float Math::ToDegree(float radian)
{
	return radian * 180.0f / PI;
}

D3DXVECTOR3 Math::ToDegree(D3DXVECTOR3 vec)
{
	D3DXVECTOR3 res;
	res.x = vec.x * 180.0f / PI;
	res.y = vec.y * 180.0f / PI;
	res.z = vec.z * 180.0f / PI;
	return res;
}

float Math::WrapAngle(float angle, bool isDegree)
{
	float base   = isDegree ? 180.0f : Math::PI;
	float offset = isDegree ? 360.0f : Math::TwoPI;

	while (angle < -base)
	{
		angle += offset;
	}
	while (angle > base)
	{
		angle -= offset;
	}

	return angle;
}

float Math::WrapAngleDeg(float angle)
{
	return WrapAngle(angle, true);
}

float Math::WrapAngleRad(float angle)
{
	return WrapAngle(angle, false);
}

int Math::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}
float Math::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;
	
	return r1 + val;
}

int Math::Clamp(int value, int minValue, int maxValue)
{
	SwapMinMax(&minValue, &maxValue);

	value = value > maxValue ? maxValue : value;
	value = value < minValue ? minValue : value;

	return value;
}

float Math::Clamp(float value, float minValue, float maxValue)
{
	SwapMinMax(&minValue, &maxValue);

	value = value > maxValue ? maxValue : value;
	value = value < minValue ? minValue : value;

	return value;
}

D3DXVECTOR2 Math::Abs(D3DXVECTOR2 vec)
{
	D3DXVECTOR2 res;
	res.x = abs(vec.x);
	res.y = abs(vec.y);

	return res;
}

float Math::Fraction(float f1)
{
	int i1 = (int)f1;

	return f1 - (float)i1;
}


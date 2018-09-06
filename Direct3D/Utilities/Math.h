#pragma once
class Math
{
public:
	static const float PI;
	static const float TwoPI;
	static const float PiOver4;

	static const float FloatMinValue;
	//static const float Epsilon;
	static const float FloatMaxValue;
	//static const float PositiveInfinity;
	//static const float NegativeInfinity;
	//static const float NaN;

	static int Min(int n1, int n2);
	static int Max(int n1, int n2);
	static float Min(float f1, float f2);
	static float Max(float f1, float f2);


	static void SwapMinMax(int* min, int* max);
	static void SwapMinMax(LONG* min, LONG* max);
	static void SwapMinMax(float* min, float* max);

	static float Length(D3DXVECTOR3 vec);
	static float LengthSquared(D3DXVECTOR3 vec);

	static float Modulo(float val1, float val2);

	//Degree, Radian 
	static float ToRadian(float degree);
	static D3DXVECTOR3 ToRadian(D3DXVECTOR3 vec);
	static float ToDegree(float radian);
	static D3DXVECTOR3 ToDegree(D3DXVECTOR3 vec);

	static float WrapAngle(float angle, bool isDegree);
	static float WrapAngleDeg(float angle);
	static float WrapAngleRad(float angle);


	static int Random(int r1, int r2);
	static float Random(float r1, float r2);

	static int Clamp(int value, int minValue, int maxValue);
	static float Clamp(float value, float minValue, float maxValue);

	static D3DXVECTOR2 Abs(D3DXVECTOR2 vec);

	static float Fraction(float f1);
};
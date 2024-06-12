#include "MathUtilityForText.h"

// 2項演算子オーバーロード
// Vextor3の足算
const Vector3 operator+(const Vector3& v1, const Vector3& v2) { 
	Vector3 temp(v1);
	return temp += v2;
}
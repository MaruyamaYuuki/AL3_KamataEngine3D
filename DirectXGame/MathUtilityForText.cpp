#include "MathUtilityForText.h"

// 2項演算子オーバーロード
// Vextor3の足算
const Vector3 operator+(const Vector3& v1, const Vector3& v2) { 
	Vector3 temp(v1);
	return temp += v2;
}
// Vextor3の減算
const Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	Vector3 temp(v1);
	return temp -= v2;
}

// Vector3とスカラーの乗算
const Vector3 operator*(const Vector3& v, float scalar) { return Vector3(v.x * scalar, v.y * scalar, v.z * scalar); }

// Lerp関数の実装
Vector3 Lerp(const Vector3& start, const Vector3& end, float t) { return start + (end - start) * t; }
#include <Vector3.h>
#define _USE_MATH_DEFINES
#include<math.h>

// 2項演算子オーバーロード

// Vextor3の乗算
const Vector3 operator*(const Vector3& v1, const Vector3& v2);

// Vector3とスカラーの乗算
const Vector3 operator*(const Vector3& v, float scalar);

// Lerp関数の宣言
Vector3 Lerp(const Vector3& start, const Vector3& end, float t);

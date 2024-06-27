#include <Vector3.h>
#include <Matrix4x4.h>
#include <cmath>

class myMath {
public:
	static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
	static Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
	static Matrix4x4 MakeScaleMatrix(const Vector3& scale);
	static Matrix4x4 MakeRotateXMatrix(float radius);
	static Matrix4x4 MakeRotateYMatrix(float radius);
	static Matrix4x4 MakeRotateZMatrix(float radius);
	static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	static Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
};
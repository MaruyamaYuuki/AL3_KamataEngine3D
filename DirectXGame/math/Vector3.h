#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;

	// コンストラクタ
	Vector3() : x(0), y(0), z(0) {}
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	// 加算演算子の定義
	Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }

		// 減算演算子の定義
	Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }

	// += 演算子のオーバーロード
	Vector3& operator+=(const Vector3& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}

	Vector3& operator-=(const Vector3& other) {
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}

	Vector3& operator*=(float s) {
		this->x *= s;
		this->y *= s;
		this->z *= s;
		return *this;
	}

	Vector3& operator*=(const Vector3& other) {
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;
		return *this;
	}
};



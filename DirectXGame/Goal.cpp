#define NOMINMAX
#include "Goal.h"
#include <cassert>
#include <numbers>
#include <Input.h>
#include <algorithm>
#include <DebugText.h>
#include <cmath>
#include "MathUtilityForText.h"

void Goal::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数をメンバ変数に記録
	model_ = model;
	viewProjection_ = viewProjection;
	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Goal::Updata() {
	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();
}

void Goal::Draw() {
	model_->Draw(worldTransform_, *viewProjection_); 
}

Vector3 Goal::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

AABB Goal::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Goal::OnCollision(const Player* player) { 
	(void)player; 
}

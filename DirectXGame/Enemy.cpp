#define NOMINMAX
#include "Enemy.h"
#include <cassert>
#include <numbers>
#include <Input.h>
#include <algorithm>
#include <DebugText.h>
#include <cmath>

void Enemy::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数をメンバ変数に記録
	model_ = model;
	viewProjection_ = viewProjection;
	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;

	// 速度の設定
	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;
}

void Enemy::Updata() {
	// 移動
	worldTransform_.translation_ += velocity_;

	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;
	//回転アニメーション
	float param = std::sin((2*std::numbers::pi_v<float> / 180.0f) * kWalkMotionTime / walkTimer_);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = radian * (std::numbers::pi_v<float> / 180.0f);
	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() { 
	model_->Draw(worldTransform_, *viewProjection_); 
}
#define NOMINMAX
#include "CameraController.h"
#include "Player.h"
#include <algorithm>

void CameraController::Initialize() {
    // ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();
	// 追従対象とオフセットからカメラの目標座標を計算
	goalPos = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;

	// 座標補間によりゆったり追従
	viewProjection_.translation_ = Lerp(viewProjection_.translation_, goalPos, kInterpolationRate);

	// 追従対象が画面外に出ないように補正
	viewProjection_.translation_.x = std::max(viewProjection_.translation_.x, targetWorldTransform.translation_.x + kMargin.left);
	viewProjection_.translation_.x = std::min(viewProjection_.translation_.x, targetWorldTransform.translation_.x + kMargin.right);
	viewProjection_.translation_.y = std::max(viewProjection_.translation_.y, targetWorldTransform.translation_.y + kMargin.bottom);
	viewProjection_.translation_.y = std::min(viewProjection_.translation_.y, targetWorldTransform.translation_.y + kMargin.top);

	// 行動範囲制御
	viewProjection_.translation_.x = std::max(viewProjection_.translation_.x, movebleArea_.left);
	viewProjection_.translation_.x = std::min(viewProjection_.translation_.x, movebleArea_.right);
	viewProjection_.translation_.y = std::max(viewProjection_.translation_.y, movebleArea_.bottom);
	viewProjection_.translation_.y = std::min(viewProjection_.translation_.y, movebleArea_.top);

	// 行列を更新する
	viewProjection_.UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	viewProjection_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}
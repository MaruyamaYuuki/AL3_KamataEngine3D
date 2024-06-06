#define NOMINMAX
#include "Player.h"
#include <cassert>
#include <numbers>
#include <Input.h>
#include <algorithm>



void Player::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position) {
    // NULLポインタチェック
	assert(model);
	// 引数をメンバ変数に記録
	model_ = model;
	textureHandle_ = textureHandle;
	viewProjection_ = viewProjection;
	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
};

void Player::Update() {

	// 移動入力
	// 設置状態
	if (onGround_) {
    	// 左右移動操作
    	if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
    		// 左右加速
    		Vector3 acceleration = {};
    		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
    			// 左移動中の右入力
    			if (velocity_.x < 0.0f) {
    				// 速度と逆方向に入力中は急ブレーキ
    				velocity_.x *= (1.0f - kAcceleration);
    			}
    			acceleration.x += kAcceleration;

    			if (lrDirection_ != lRDirection::kRight) {
    				lrDirection_ = lRDirection::kRight;
    				turnFirstRotationY_ = worldTransform_.rotation_.y;
    				turnTimer_ = kTimeTurn;
    			}
    		} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
    			// 右移動中の左入力
    			if (velocity_.x > 0.0f) {
    				// 速度と逆方向に入力中は急ブレーキ
    				velocity_.x *= (1.0f - kAcceleration);
    			}
    			acceleration.x -= kAcceleration;

    			if (lrDirection_ != lRDirection::kLeft) {
    				lrDirection_ = lRDirection::kLeft;
    				turnFirstRotationY_ = worldTransform_.rotation_.y;
    				turnTimer_ = kTimeTurn;
    			}
    		} 
    		// 加速/減速
    		velocity_.x += acceleration.x;

    		// 最大速度制限
    		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

    	}else {
    		    // 非入力時は移動減衰をかける
    			velocity_.x *= (1.0f - kAcceleration);
    	}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ処理
			Vector3 jumNum(0, kJumpAcceleration, 0);
			velocity_.x += jumNum.x;
			velocity_.y += jumNum.y;
			velocity_.z += jumNum.z;
		}
	// 空中
	} else {
        // 落下速度
		Vector3 fallNum(0, -kGravityAcceleration, 0);
		velocity_.x += fallNum.x;
		velocity_.y += fallNum.y;
		velocity_.z += fallNum.z;
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	
	// 着地フラグ
	bool landing = false;

	// 地面との当たり判定
	// 下降中？
	if (velocity_.y < 0) {
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 2.0f) {
			landing = true;
		}
	}

	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 2.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuationLanding);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}

	// 旋回制御
	if (turnTimer_>0.0f)
	{
		turnTimer_ -= 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {
			std::numbers::pi_v<float> / 2.0f, 
			std::numbers::pi_v<float> * 3.0f / 2.0f
		};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		    // 補間の割合を計算（0から1までの値）
		float t = (kTimeTurn - turnTimer_) / kTimeTurn;
		t = std::clamp(t, 0.0f, 1.0f);

		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = Lerp(turnFirstRotationY_, destinationRotationY, t);
	}

	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();

};

void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_,*viewProjection_,textureHandle_);
};
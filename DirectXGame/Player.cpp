#define NOMINMAX
#include "Player.h"
#include <cassert>
#include <numbers>
#include <Input.h>
#include <algorithm>
#include "MapChipFiled.h"
#include <DebugText.h>



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

	playerMove();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.velocity_ = velocity_;

	// マップ衝突チェック
	ColisionMap(collisionMapInfo);

	CollisionResultMove(collisionMapInfo);

	HitCeiling(collisionMapInfo);
	
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
			velocity_.x *= (1.0f - kAttenuation);
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
	model_->Draw(worldTransform_,*viewProjection_);
};

void Player::playerMove() {
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

		} else {
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
}

void Player::ColisionMap(CollisionMapInfo& info) { 
	CheckMapCollisionTop(info);
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) { 
	Vector3 offserTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
    };

	return center + offserTable[static_cast<uint32_t>(corner)];
}

void Player::CheckMapCollisionTop(CollisionMapInfo& info) {
	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
	}

	// 上昇あり？
	if (info.velocity_.y <= 0) {
		return;
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipFiled::IndexSet indexSetLeftTop;
	indexSetLeftTop = mapChipFiled_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipFiled_->GetMapChipTypeByIndex(indexSetLeftTop.xIndex, indexSetLeftTop.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	MapChipFiled::IndexSet indexSetRightTop;
	indexSetRightTop = mapChipFiled_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipFiled_->GetMapChipTypeByIndex(indexSetRightTop.xIndex, indexSetRightTop.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		MapChipFiled::IndexSet indexSet = mapChipFiled_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipFiled::Rect rect = mapChipFiled_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.velocity_.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.ceilingFlag = true;
	}
}

void Player::CollisionResultMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_ += info.velocity_;
}

void Player::HitCeiling(const CollisionMapInfo& info) {
	// 天井に当たった？
	if (info.ceilingFlag) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}
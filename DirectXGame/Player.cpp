#define NOMINMAX
#include "Player.h"
#include "MapChipFiled.h"
#include "MathUtilityForText.h"
#include <DebugText.h>
#include <Input.h>
#include <algorithm>
#include <cassert>
#include <numbers>

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数をメンバ変数に記録
	model_ = model;
	viewProjection_ = viewProjection;
	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
};

void Player::Update() {

	PlayerMove();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.velocity_ = velocity_;

	// マップ衝突チェック
	CheckMapColision(collisionMapInfo);
	// 判定結果を反映して移動
	CollisionResultMove(collisionMapInfo);
	// 天井に接触している場合の処理
	HitCeiling(collisionMapInfo);
	// 壁に接触している場合の処理
	UpdataWall(collisionMapInfo);
	// 接地状態の切り替え
	UpdataGround(collisionMapInfo);

	// 旋回制御
	PlayerTurning();

	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();
};

void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *viewProjection_);
};

void Player::PlayerMove() {
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

		// 着地フラグ
		landing = false;

		// 地面との当たり判定
		// 下降中？
		if (velocity_.y < 0) {
			// Y座標が地面以下になったら着地
			if (worldTransform_.translation_.y <= 1.0f) {
				landing = true;
			}
		}
	}

	// 移動
	worldTransform_.translation_ += velocity_;
	;

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
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}
}

void Player::CheckMapColision(CollisionMapInfo& info) {
	CheckMapCollisionTop(info);
	CheckMapCollisionBottom(info);
	CheckMapCollisionLeft(info);
	CheckMapCollisionRight(info);
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
    };

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

// 上
void Player::CheckMapCollisionTop(CollisionMapInfo& info) {
	// 上昇あり？
	if (info.velocity_.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipFiled::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipFiled::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.velocity_.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (+kHeight / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.ceilingFlag = true;
	}
}

// 下
void Player::CheckMapCollisionBottom(CollisionMapInfo& info) {
	// 下降あり？
	if (info.velocity_.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真下の当たり判定を行う
	bool hit = false;

	// 左下点の判定
	MapChipFiled::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		DebugText::GetInstance()->ConsolePrintf("hitRight\n");
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		DebugText::GetInstance()->ConsolePrintf("hitLeft\n");
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.velocity_ + Vector3(0, -kHeight / 2.0f, 0));
		// めり込み先のブロックの範囲矩形
		MapChipFiled::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.velocity_.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + (+kHeight / 2.0f + kBlank));
		// 地面に当たったことを記録する
		info.landing = true;
		DebugText::GetInstance()->ConsolePrintf("landing\n");
	}
}

// 右
void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	// 右移動あり？
	if (info.velocity_.x <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右の当たり判定を行う
	bool hit = false;

	// 右上点の判定
	MapChipFiled::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(+kWidth / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		MapChipFiled::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.velocity_.x = std::min(0.0f, rect.left - worldTransform_.translation_.x + (kWidth / 2.0f + kBlank));
		// 壁に当たったことを判定結果に記録
		info.WallFlag = true;
	}
}

// 左
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	// 左移動あり？
	if (info.velocity_.x >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 左の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipFiled::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ - Vector3(kWidth / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		MapChipFiled::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.velocity_.x = std::max(0.0f, rect.right - worldTransform_.translation_.x - (kWidth / 2.0f - kBlank));
		// 壁に当たったことを判定結果に記録
		info.WallFlag = true;
	}
}

void Player::CollisionResultMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_ += info.velocity_;
}

void Player::HitCeiling(const CollisionMapInfo& info) {
	// 天井に当たった？
	if (info.ceilingFlag) {
		velocity_.y = 0;
	}
}

void Player::UpdataGround(const CollisionMapInfo& info) {
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		} else {
			// 移動後の4つの角の座標
			std::array<Vector3, kNumCorner> positionNew;

			for (uint32_t i = 0; i < positionNew.size(); ++i) {
				positionNew[i] = CornerPosition(worldTransform_.translation_ + info.velocity_, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			// 真下の当たり判定を行う
			bool ground = false;

			// 左下点の判定
			MapChipFiled::IndexSet inexSet;
			inexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom] + Vector3(0, -offSet, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(inexSet.xIndex, inexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				ground = true;
			}

			// 右下点の判定
			inexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom] + Vector3(0, -offSet, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(inexSet.xIndex, inexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				ground = true;
			}

			if (!ground) {
				// 空中状態に切り替える
				onGround_ = false;
				DebugText::GetInstance()->ConsolePrintf("NotonGround\n");
			}
		}
	} else {
		// 着地
		if (info.landing) {
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
			DebugText::GetInstance()->ConsolePrintf("onGround\n");
		}
	}
}

void Player::UpdataWall(const CollisionMapInfo& info) {
	// 壁接触による減速
	if (info.WallFlag) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::PlayerTurning() {

	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// 補間の割合を計算（0から1までの値）
		float t = (kTimeTurn - turnTimer_) / kTimeTurn;
		t = std::clamp(t, 0.0f, 1.0f);

		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = Lerp(turnFirstRotationY_, destinationRotationY, t);
	}
}
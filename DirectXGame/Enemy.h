#include "WorldTransform.h"
#include "Model.h"
#include "Vector3.h"

/// <summary>
/// 敵
/// </summart>
class Enemy {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Updata();

	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	ViewProjection* viewProjection_ = nullptr;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.05f;

	// 速度
	Vector3 velocity_ = {};

	// 最初の角度
	static inline const float kWalkMotionAngleStart = 2.0f;
	// 最後の角度
	static inline const float kWalkMotionAngleEnd = -2.0f;
	// アニメーションの周期となる時間
	static inline const float kWalkMotionTime = 1.0f;
	// 経過時間
	float walkTimer_ = 0.0f;

};
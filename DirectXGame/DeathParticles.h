#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <array>
#define _USE_MATH_DEFINES
#include <math.h>
#include "ObjectColor.h"
#include "Vector4.h"

/// <summary>
/// デス演出用パーティクル
/// </summary>
class DeathParticles {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

private:
	Model* model_ = nullptr;
	ViewProjection* viewProjection_ = nullptr;
	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	std::array<WorldTransform, kNumParticles> worldTransform_;

	// 存続時間（消滅までの時間）<秒>
	static inline const float kDuration = 0.7f;
	// 移動の速さ
	static inline const float kSpeed = 0.1f;
	// 分割した1個分の角度
	static inline const float kAngleUnit = (2 * float(M_PI)) / kNumParticles;

	// 終了フラグ 
	bool isFinished_ = false;
	// 経過時間カウント
	float counter_ = 0.0f;

		// 色変更オブジェクト
	ObjectColor objectColor_;
	// 色の数値
	Vector4 color_;
};
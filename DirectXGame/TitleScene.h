#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Fade.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {
public:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

public:
	TitleScene();

	~TitleScene();

	void Initialize();

	void Update();

	void Draw();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

private:
	// 終了フラグ
	bool finished_ = false;

	DirectXCommon* dxCommon_ = nullptr;

	Model* modelTitle_ = nullptr;

	WorldTransform titleWorldTransform_;

	ViewProjection viewProjection_;

	// 最初の角度
	static inline const float kWalkMotionPositionStart = 0.1f;
	// 最後の角度
	static inline const float kWalkMotionPositionEnd = -0.1f;
	// アニメーションの周期となる時間
	static inline const float kWalkMotionTime = 3.0f;
	// 経過時間
	float timer_ = 0.0f;

	Fade* fade_ = nullptr;
	float fadeTime = 0.5f;

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;
};
#include "Model.h"
#include "WorldTransform.h"
#include "Vector3.h"

enum class lRDirection {
	kRight,
	kLeft,
};

/// <summary>
/// 自キャラ
/// </summary>
class Player{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <parm name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <param neme="viewProjection">ビュープロジェクション</pram>
	void Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	float Lerp(float start, float end, float t) { return start + t * (end - start); }

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const Vector3& GetVelocity() const { return velocity_; }

	private:
		// ワールド変換データ
	    WorldTransform worldTransform_;
		// モデル
	    Model* model_ = nullptr;
		//　テクスチャハンドル
	    uint32_t textureHandle_ = 0u;
	    ViewProjection* viewProjection_ = nullptr;

		Vector3 velocity_ = {};

		static inline const float kAcceleration = 0.3f;
	    static inline const float kAttenuation = 0.3f;
	    static inline const float kLimitRunSpeed = 0.3f;

		lRDirection lrDirection_ = lRDirection::kRight;
		// 旋回開始時の角度
	    float turnFirstRotationY_ = 0.0f;
		// 旋回タイマー
	    float turnTimer_ = 0.0f;
		// 旋回時間<秒>
	    static inline const float kTimeTurn = 0.3f;

		// 設置状態フラグ
	    bool onGround_ = true;
		// 重力加速度（下方向）
	    static inline const float kGravityAcceleration = 1.0f;
		// 最大落下速度（下方向）
	    static inline const float kLimitFallSpeed = 0.5f;
		// ジャンプ初速（上方向）
	    static inline const float kJumpAcceleration = 3.0f;

		static inline const float kAttenuationLanding = 0.9f;
};
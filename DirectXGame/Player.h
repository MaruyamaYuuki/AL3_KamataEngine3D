#include "Model.h"
#include "WorldTransform.h"
#include "Vector3.h"

class MapChipFiled;

enum class lRDirection {
	kRight,
	kLeft,
};

/// <summary>
/// 自キャラ
/// </summary>
class Player{
public:
	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool ceilingFlag = false;
		bool WallFlag = false;
		bool landing = false;
		Vector3 velocity_;
	};
	//角
	enum Corner {
		kRightBottom,   // 右下
		kLeftBottom,    // 左下
		kRightTop,      // 右上
		kLeftTop,       // 左上

		kNumCorner      // 要素数
	};

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

	void SetMapChipFiled(MapChipFiled* mapChipFiled) { mapChipFiled_ = mapChipFiled; }

	void playerMove();

	void ColisionMap(CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void CheckMapCollisionTop(CollisionMapInfo& info);

	void CollisionResultMove(const CollisionMapInfo& info);

	void HitCeiling(const CollisionMapInfo& info);

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
		// マップチップふによるフィールド
	    MapChipFiled* mapChipFiled_ = nullptr;
		// キャラクターの当たり判定サイズ
	    static inline const float kWidth =1.8f;
	    static inline const float kHeight = 1.8f;

		static inline const float kBlank = 0;
};
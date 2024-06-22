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
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

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

	void SetMapChipFiled(MapChipFiled* mapChipFiled) { mapChipField_ = mapChipFiled; }

	void PlayerMove();

	void CheckMapColision(CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	// マップ衝突判定上方向
	void CheckMapCollisionTop(CollisionMapInfo& info);

	// 判定結果を反映して移動させる
	void CollisionResultMove(const CollisionMapInfo& info);

	// 天井に接触している場合の処理
	void HitCeiling(const CollisionMapInfo& info);

	// マップ衝突処理下方向
	void CheckMapCollisionBottom(CollisionMapInfo& info);

	// 接地状態の切り替え処理
	void UpdataGround(const CollisionMapInfo& info);

	// マップ衝突処理右判定
	void CheckMapCollisionRight(CollisionMapInfo& info);

	// マップ衝突処理左判定
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	// 壁に接触している場合の処理
	void UpdataWall(const CollisionMapInfo& info);

	// 旋回処理
	void PlayerTurning();

	private:
		// ワールド変換データ
	    WorldTransform worldTransform_;
		// モデル
	    Model* model_ = nullptr;
	    ViewProjection* viewProjection_ = nullptr;

		Vector3 velocity_ = {};

		static inline const float kAcceleration = 0.1f;
	    static inline const float kAttenuation = 0.3f;
	    static inline const float kLimitRunSpeed = 0.1f;

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
	    static inline const float kGravityAcceleration = 0.3f;
		// 最大落下速度（下方向）
	    static inline const float kLimitFallSpeed = 0.3f;
		// ジャンプ初速（上方向）
	    static inline const float kJumpAcceleration = 1.0f;
		// マップチップふによるフィールド
	    MapChipFiled* mapChipField_ = nullptr;
		// キャラクターの当たり判定サイズ
	    static inline const float kWidth =0.8f;
	    static inline const float kHeight = 0.8f;

		static inline const float kBlank = 0.01f;

		static inline const float kAttenuationWall = 0.3f;

			// 着地フラグ
	    bool landing = false;

		float offSet = 0.5f;
};
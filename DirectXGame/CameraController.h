#include "ViewProjection.h"
#include "MathUtilityForText.h"

class Player;

/// <summery>
/// カメラコントロール
/// </summary>
class CameraController {

	public:
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 0.0f;
	};

public:
	/// <summary>
	/// 初期化
    /// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
    void Update();

	void SetTarget(Player* target) { target_ = target; }

	const ViewProjection& GetViewProjection() const { return viewProjection_; }

	void Reset();

	void SetMovebleaArea(Rect area) { movebleArea_ = area; };

private:
	// ビュープロジェクション
	ViewProjection viewProjection_;
	Player* target_ = nullptr;
	// 追従対象のカメラの座標の差（オフセット）
	Vector3 targetOffset_ = {0, 0, -15.0f};
	Rect movebleArea_ = {0, 100, 0, 100};
};
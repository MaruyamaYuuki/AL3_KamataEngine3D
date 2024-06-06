#include "ViewProjection.h"

class Player;

/// <summery>
/// カメラコントロール
/// </summary>
class CameraController {

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

	void Reset();

private:
	// ビュープロジェクション
    ViewProjection viewProjection_;
	Player* target_ = nullptr;
	// 追従対象のカメラの座標の差（オフセット）
	Vector3 targetOffset_ = {0, 0, -15.0f};
};
#include "WorldTransform.h"
#include "Model.h"
#include "Vector3.h"

class Player;

class Goal {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Updata();

	void Draw();

	Vector3 GetWorldPosition();

	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Player* player);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	ViewProjection* viewProjection_ = nullptr;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
};
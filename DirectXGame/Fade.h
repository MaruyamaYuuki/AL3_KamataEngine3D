#include "Sprite.h"
#include "DirectXCommon.h"

/// <summary>
/// フェード
/// </summary>
class Fade {
public:
	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

public:
	void Inisialize();

	void Update();

	void Draw(ID3D12GraphicsCommandList* commandList);

	// フェード開始
	void Start(Status status, float duration);

	// フェード停止
	void Stop();

	// フェード終了判定
	bool IsFinished() const;

private:
	uint32_t textureHandle_ = 0;
	Sprite* sprite_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	// 現在のフェードの状態
	Status status_ = Status::None;
	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;
};
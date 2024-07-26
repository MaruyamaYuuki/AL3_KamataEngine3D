#include "TitleScene.h"
#include <DebugText.h>
#include <numbers>
#define _USE_MATH_DEFINES
#include<math.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() { 
	delete modelTitle_; 
	delete fade_;
}

void TitleScene::Initialize() { 
	dxCommon_ = DirectXCommon::GetInstance();

	modelTitle_ = Model::CreateFromOBJ("titleText", true); 

	titleWorldTransform_.Initialize();
	titleWorldTransform_.translation_.z -= 42.f;

	viewProjection_.Initialize();

	timer_ = 0.0f;

	fade_ = new Fade();
	fade_->Inisialize();
	fade_->Start(Fade::Status::FadeIn, fadeTime);
}

void TitleScene::Update() {
	// タイマーを加算
	timer_ += 1.0f / 60.0f;
	// 上下アニメーション
	float param = std::sin((2 * std::numbers::pi_v<float>)*timer_ / kWalkMotionTime);
	float radian = kWalkMotionPositionStart + kWalkMotionPositionEnd * (param + 1.0f) / 2.0f;
	titleWorldTransform_.translation_.y = sinf((radian * float(M_PI)) / 2);

	titleWorldTransform_.UpdateMatrix();
	switch (phase_) {
	case TitleScene::Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Stop();
			phase_ = Phase::kMain;
		}
		break;
	case TitleScene::Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, fadeTime);
			phase_ = Phase::kFadeOut;
		}
		break;
	case TitleScene::Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			fade_->Stop();
			finished_ = true;
		}
		break;
	}
}

void TitleScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	modelTitle_->Draw(titleWorldTransform_, viewProjection_);

	Model::PostDraw();

	fade_->Draw(commandList);
}
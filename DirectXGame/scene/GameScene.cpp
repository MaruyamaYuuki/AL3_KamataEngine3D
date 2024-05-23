#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_; 
	delete blockModel_;
	delete modelSkydome_;
	// 自キャラの解放
	delete player_;
	// 天球の解放
	delete skydome_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
		    delete worldTransformBlock;
	    }
	}
	worldTransformBlocks_.clear();
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandle_ = TextureManager::Load("mario.png");
	// 3Dモデルの生成
	model_ = Model::Create();
	blockModel_ = Model::Create();
	modelSkydome_ = Model::CreateFromOBJ("Sphere", true);
	viewProjection_.farZ = 150.0f;
	viewProjection_.Initialize();
	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_,textureHandle_,&viewProjection_);
	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &viewProjection_);
	// 要素数
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;
	// ブロック1個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;
	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockHorizontal);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		// 1列の要素数を設定
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if ((i + j) % 2 != 0) {
				continue; // ブロックを作成せずにスキップ
			}
			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
		}

	}

	static const int kWindowWidth = 1280; // 横幅
	static const int kWindowHeight = 720; // 縦幅
	debugCamera_ = new DebugCamera(kWindowWidth, kWindowHeight);
}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update();
	// 天球の更新
	skydome_->Update();
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->UpdateMatrix();
		}
	}

	#ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN)) {
		if (!isDebugCameraActive_)
			isDebugCameraActive_ = true;
		else
			isDebugCameraActive_ = false;
	}
	#endif
	// カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
	    debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
	
	// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	
	// 天球の描画
	skydome_->Draw();

	// 自キャラの描画
	player_->Draw();

	// ブロックの描画/
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
		    blockModel_->Draw(*worldTransformBlock, viewProjection_);	
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

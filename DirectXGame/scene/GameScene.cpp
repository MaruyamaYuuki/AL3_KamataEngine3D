#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <DebugText.h>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_; 
	delete blockModel_;
	delete modelSkydome_;
	delete modelEnemy_;
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
	delete mapChipFiled_;
	delete cameraController_;
	for (Enemy*enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandle_ = TextureManager::Load("mario.png");
	// 3Dモデルの生成
	model_ = Model::Create();
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	blockModel_ = Model::CreateFromOBJ("block", true);
	modelSkydome_ = Model::CreateFromOBJ("Sphere", true);
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	viewProjection_.farZ = 150.0f;
	viewProjection_.Initialize();

	// マップチップフィールドの生成と初期化
	mapChipFiled_ = new MapChipFiled;
	mapChipFiled_->LoadMapChipCsv("Resources/map.csv");
	// 自キャラの生成
	player_ = new Player();
	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipFiled_->GetMapChipPositionByIndex(1, 18);
	// 自キャラの初期化
	player_->Initialize(modelPlayer_, &viewProjection_, playerPosition);
	player_->SetMapChipFiled(mapChipFiled_);
	// 敵の生成・初期化
	for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipFiled_->GetMapChipPositionByIndex(10 - i, 18 -i);
		newEnemy->Initialize(modelEnemy_, &viewProjection_, enemyPosition);
		enemies_.push_back(newEnemy);
	}
	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &viewProjection_);
	// カメラコントローラの初期化
	cameraController_ = new CameraController(); // 生成
	cameraController_->Initialize();            // 初期化
	cameraController_->SetTarget(player_);      // 追従対象をセット
	cameraController_->Reset();                 // リセット(瞬間合わせ)
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovebleaArea(cameraArea);

	static const int kWindowWidth = 1280; // 横幅
	static const int kWindowHeight = 720; // 縦幅
	debugCamera_ = new DebugCamera(kWindowWidth, kWindowHeight);



	GenerateBlocks();
}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update();
	// 敵キャラの更新
	for (Enemy* enemy : enemies_) {
		enemy->Updata();
	}
	// カメラの更新
	cameraController_->Update();
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

	// 全ての当たり判定を行う
	CheckAllCollision();

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
	}  else {
		// ビュープロジェクション行列の更新と転送
		const ViewProjection& cameraViewProjection = cameraController_->GetViewProjection();
		viewProjection_.matView = cameraViewProjection.matView;
		viewProjection_.matProjection = cameraViewProjection.matProjection;
		viewProjection_.TransferMatrix();
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

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

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

void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipFiled_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipFiled_->GetNumBlockHorizontal();
	// 要素数を変更する
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を設定
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipFiled_->GetMapChipTypeByIndex(j,i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipFiled_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

bool GameScene::IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true;
	}
	return false;
}

void GameScene::CheckAllCollision() {

	#pragma region 自キャラと敵の当たり判定
	{
		// 判定1と2の座標
		AABB aabb1, aabb2;

		// 自キャラの座標
		aabb1 = player_->GetAABB();

		//自キャラと手の弾全ての当たり判定
		for (Enemy* enemy : enemies_) {
			// 敵弾の座標
			aabb2 = enemy->GetAABB();

			// AABB同士の交差判定
			if (IsCollision(aabb1, aabb2)) {
				// 自キャラの衝突時コールバックを呼び出す
				player_->OnCollision(enemy);
				// 敵弾の衝突時コールバックを呼び出す
				enemy->OnCollision(player_);
			}
		}
	}
	#pragma endregion
}



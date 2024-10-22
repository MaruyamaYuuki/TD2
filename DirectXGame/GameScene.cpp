#include "GameScene.h"
#include "base/TextureManager.h"
#include <cassert>
#include "2d/DebugText.h"
#include "imgui.h"

using namespace KamataEngine;

GameScene::GameScene() {}

GameScene::~GameScene() { 
	// プレイヤーの解放
	delete player_;
	delete modelPlayer_;
	// 障害物の解放
	for (Hurdle* hurdle : hurdles_) {
		delete hurdle;
	}
	hurdles_.clear();
	delete modelHurdle_;
	// ゴールの解放
	for (Goal* goal : goals_) {
		delete goal;
	}
	goals_.clear();
	// マップチップフィールドの解放
	delete mapChipField_; 
	delete modelBlock_;
	delete backSprite1_;
	delete backSprite2_;
	delete backSprite3_;
	delete backSprite4_;
	delete backSprite5_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete ui_;
}

void GameScene::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();
	audio_ = KamataEngine::Audio::GetInstance();
	camera_.Initialize();

	// ゲームプレイフェーズから開始
	phase_ = Phase::kPlay;

	// マップチップフィールドの生成と初期化
	stage_ = Stage::stage1;
	modelBlock_ = Model::CreateFromOBJ("block", true);
	mapChipField_ = new MapChipField;
	LoadStage();

	// 障害物のモデルの生成
	modelHurdle_ = Model::CreateFromOBJ("hurdle", true);

	// ゴールのモデル生成
	modelGoal_ = Model::CreateFromOBJ("goal", true);

	// プレイヤー初期化
	modelPlayer_ = Model::CreateFromOBJ("player");
	player_ = new Player();
	//Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 16);
	player_->Initialize(modelPlayer_, &camera_, Vector3{130.0f, 10.0f, 0.0f});
	player_->SetMapChipField(mapChipField_);

	textureHandleBack1_ = TextureManager::Load("Back/BackScreen.png");
	textureHandleBack2_ = TextureManager::Load("Back/BackScreen2.png");
	textureHandleBack3_ = TextureManager::Load("Back/Backscreen3.png");

	backSprite1_ = Sprite::Create(textureHandleBack1_, {0, 0});
	backSprite1_->SetSize(size_);
	backSprite2_ = Sprite::Create(textureHandleBack2_, {0, 0});
	backSprite2_->SetSize(size_);
	backSprite3_ = Sprite::Create(textureHandleBack3_, {0, 0});
	backSprite3_->SetSize(size_);
	backSprite4_ = Sprite::Create(textureHandleBack3_, {0, 0});
	backSprite4_->SetSize(size_);
	backSprite5_ = Sprite::Create(textureHandleBack3_, {0, 0});
	backSprite5_->SetSize(size_);

	// フェードの作成
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// カメラコントローラの生成
	cameraController_ = new CameraController();
	// カメラコントローラの初期化
	cameraController_->Initialize();
	// 追従対象をセット
	cameraController_->SetTarget(player_);
	// リセット
	cameraController_->Reset();
	//
	CameraController::Rect cameraArea = {10.0f, 114.0f, 4.5f, 9.5f};
	//
	cameraController_->SetMovebleaArea(cameraArea);

	debugCamera_ = new DebugCamera(1280, 720);

	ui_ = new UI();
	ui_->Initialize();
	
	GenerateBlocks();
}

void GameScene::Update() { 
	ChangePhase();
	ui_->Update(player_->IsDead(), player_->IsGoal());

	switch (phase_) {
	case GameScene::Phase::kPlay:
    	if (input_->TriggerKey(DIK_SPACE)) {
    		player_->setIsGameStart(true);
    	}
		if (input_->TriggerKey(DIK_ESCAPE)) {
			finished_ = true;
		}

        // プレイヤー更新
        player_->Update();
		Vector3 pos = player_->GetWorldPosition();
		backSprite1_->SetPosition({0.0f - pos.x, 0.0f});
		backSprite2_->SetPosition({1300.0f - pos.x, 0.0f});
		backSprite3_->SetPosition({0.0f - pos.x * 20, 0.0f});
		backSprite4_->SetPosition({1300.0f - pos.x * 20, 0.0f});
		backSprite5_->SetPosition({2600.0f - pos.x * 20, 0.0f});
    	// 障害物の更新
    	for (Hurdle* hurdle : hurdles_) {
     		hurdle->Update();
    	}

    	// ゴールの更新
    	for (Goal* goal : goals_) {
     		goal->Update();
    	}

    	/*// ブロックの更新
    	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
    		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
    			if (!worldTransformBlock) {
    				continue;
    			}
    			worldTransformBlock->UpdateMatrix();
    		}
    	}*/

    	// カメラコントローラの更新
    	cameraController_->Update();

    	// カメラ処理
    	if (isDebugCameraActive_) {
    		// デバッグカメラの更新
    		debugCamera_->Update();
    		camera_.matView = debugCamera_->GetCamera().matView;
    		camera_.matProjection = debugCamera_->GetCamera().matProjection;
    		// ビュープロジェクション行列の転送
    		camera_.TransferMatrix();
    	} else {
    		camera_.matView = cameraController_->GetViewProjection().matView;
    		camera_.matProjection = cameraController_->GetViewProjection().matProjection;
    		// ビュープロジェクション行列の更新と転送
    		camera_.TransferMatrix();
    	}

		// 全ての当たり判定を行う
		CheckAllCollision();
		fade_->Update();
		break;
	case GameScene::Phase::kClear:
		// プレイヤー更新
		player_->Update();

		// 障害物の更新
		for (Hurdle* hurdle : hurdles_) {
			hurdle->Update();
		}

		// ゴールの更新
		for (Goal* goal : goals_) {
			goal->Update();
		}

		/*// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}
				worldTransformBlock->UpdateMatrix();
			}
		}*/

		// カメラコントローラの更新
		cameraController_->Update();

		// カメラ処理
		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			camera_.matView = cameraController_->GetViewProjection().matView;
			camera_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の更新と転送
			camera_.TransferMatrix();
		}
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_L)) {
		if (isDebugCameraActive_ == true)
			isDebugCameraActive_ = false;
		else
			isDebugCameraActive_ = true;
	}
#endif
		if (!ui_->IsSerect()) {
        	NextStage();
		}

		break;
	case GameScene::Phase::kDeath:
		// 障害物の更新
		for (Hurdle* hurdle : hurdles_) {
			hurdle->Update();
		}

		// ゴールの更新
		for (Goal* goal : goals_) {
			goal->Update();
		}
		/*// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}
				worldTransformBlock->UpdateMatrix();
			}
		}*/
		

		// カメラコントローラの更新
		cameraController_->Update();

		// カメラ処理
		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			camera_.matView = cameraController_->GetViewProjection().matView;
			camera_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の更新と転送
			camera_.TransferMatrix();
		}


		break;
	case GameScene::Phase::kMain:
		fade_->Update();
		break;
	default:
		break;
	}

}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	KamataEngine::Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの処理を追加できる
	/// </summary>
	backSprite1_->Draw();
	backSprite2_->Draw();
	backSprite3_->Draw();
	backSprite4_->Draw();
	backSprite5_->Draw();

	// スプライト処理後描画
	KamataEngine::Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	KamataEngine::Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//プレイヤー描画
	player_->Draw();

	// 障害物の描画
	for (Hurdle* hurdle : hurdles_) {
		hurdle->Draw();
	}

	// ゴールの描画
	for (Goal* goal : goals_) {
		goal->Draw();
	}

/*// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}*/ 

	// 3Dオブジェクト描画後処理
	KamataEngine::Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	KamataEngine::Sprite::PreDraw(commandList);

	///< summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	ui_->Draw(player_->IsDead(), player_->IsGoal(), player_->IsGameStart(),allClear);

	fade_->Draw(commandList);

	// スプライト描画後処理
	KamataEngine::Sprite::PostDraw();

#pragma endregion
}

void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();
	// 要素数を変更する
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を設定
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	/*for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				// 元の座標を取得してからオフセットを適用
				Vector3 originalPosition = mapChipField_->GetMapChipPositionByIndex(j, i);
				originalPosition.x -= 1.0f; // xに-1のオフセット
				originalPosition.y -= 9.0f; // yに+3のオフセット

				worldTransformBlocks_[i][j]->translation_ = originalPosition;
			}
		}
	}*/ 

	// 障害物の生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kHurdle) {
				Hurdle* newHurdle = new Hurdle();
				// 元の座標を取得してからオフセットを適用
				Vector3 hurdlePosition = mapChipField_->GetMapChipPositionByIndex(j, i);
				hurdlePosition.x -= 1.0f; // xに-1のオフセット
				hurdlePosition.y -= 9.0f; // yに+3のオフセット
				newHurdle->Initialize(modelHurdle_, &camera_, hurdlePosition);
				hurdles_.push_back(newHurdle);
			}
		}
	}

	// ゴールの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kGoal) {
				Goal* newGoal = new Goal();
				// 元の座標を取得してからオフセットを適用
				Vector3 goalPosition = mapChipField_->GetMapChipPositionByIndex(j, i);
				goalPosition.x -= 1.0f; // xに-1のオフセット
				goalPosition.y -= 9.0f; // yに+3のオフセット
				newGoal->Initialize(modelGoal_, &camera_, goalPosition);
				goals_.push_back(newGoal);
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
#pragma region 自キャラとゴールの当たり判定
	{
		// 判定1と2の座標
		AABB aabb1, aabb2;

		// 自キャラの座標
		aabb1 = player_->GetAABB();

		// 自キャラと手の弾全ての当たり判定
		for (Goal* goal : goals_) {
			aabb2 = goal->GetAABB();

			// AABB同士の交差判定
			if (IsCollision(aabb1, aabb2)) {
				// 自キャラの衝突時コールバックを呼び出す
				player_->CollisionGoal(goal);
				// 敵弾の衝突時コールバックを呼び出す
				goal->OnCollision(player_);
			}
		}
	}
#pragma endregion

#pragma region 自キャラと障害物の当たり判定
	{
		// 判定1と2の座標
		AABB aabb1, aabb2;

		// 自キャラの座標
		aabb1 = player_->GetAABB();

		// 自キャラと手の弾全ての当たり判定
		for (Hurdle* hurdle : hurdles_) {
			aabb2 = hurdle->GetAABB();

			// AABB同士の交差判定
			if (IsCollision(aabb1, aabb2)) {
				// 自キャラの衝突時コールバックを呼び出す
				player_->CollisionHurdle(hurdle);
				// 敵弾の衝突時コールバックを呼び出す
				hurdle->OnCollision(player_);
			}
		}
	}
#pragma endregion
}

void GameScene::LoadStage() {
	//mapChipField_->ResetMapChipData();
	switch (stage_) {
	case Stage::stage1:
		mapChipField_->LoadMapChipCsv("Resources/map/newStage1.csv");
		break;
	case Stage::stage2:
		mapChipField_->LoadMapChipCsv("Resources/map/newStage2.csv");
		break;
	case Stage::stage3:
		mapChipField_->LoadMapChipCsv("Resources/map/newStage3.csv");
		break;
	default:
		break;
	}
}

void GameScene::NextStage() {
    if (input_->TriggerKey(DIK_SPACE)) {
    	switch (stage_) {
    	case Stage::stage1:
			stage_ = Stage::stage2;
			needStageReload = true;
    		break;
    	case Stage::stage2:
			stage_ = Stage::stage3;
			needStageReload = true;
			allClear = true;
    		break;
    	case Stage::stage3:
			phase_ = Phase::kFadeIn;
			needStageReload = true;
    		break;
    	default:
    		break;
	    }
	}
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case GameScene::Phase::kPlay:
		if (player_->IsGoal()) {
			phase_ = Phase::kClear;
			if (stage_ == Stage::stage3) {
				allClear = true;
			}
		} else if (player_->IsDead()) {
			phase_ = Phase::kDeath;
		}
		break;
	case GameScene::Phase::kClear:
		if (needStageReload) {
			// マップ関連のデータをリセット
			mapChipField_->ResetMapChipData();

			// 障害物、ゴール、ブロックのクリア
			for (Hurdle* hurdle : hurdles_) {
				delete hurdle;
			}
			hurdles_.clear();
			for (Goal* goal : goals_) {
				delete goal;
			}
			goals_.clear();
			for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
				for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
					delete worldTransformBlock;
				}
			}
			worldTransformBlocks_.clear();

			// ステージをロード
			LoadStage();
			GenerateBlocks();

			// プレイヤーのリセット
			player_->Reset();

			// ステージリロードフラグをリセット
			needStageReload = false;
			phase_ = Phase::kPlay;
		} else if (input_->TriggerKey(DIK_SPACE) && isSerect_) {
			phase_ = Phase::kFadeIn;
		} else if (input_->TriggerKey(DIK_SPACE) && ui_->IsSerect() || input_->TriggerKey(DIK_SPACE) && stage_ == Stage::stage3) {
			finished_ = true;
		}
		break;
	case GameScene::Phase::kDeath:
		if (input_->TriggerKey(DIK_SPACE)) {
			if (!ui_->IsSerect()) {
				// リスタート開始
				isRestarting_ = true;
				countdownTime_ = 3.0f; // カウントダウンをリセット
			} else {
				phase_ = Phase::kFadeIn;
			}
		}

		if (isRestarting_) {

		}
		break;
	case GameScene::Phase::kFadeIn:
		fade_->Start(Fade::Status::FadeOut, 1.0f);
		phase_ = Phase::kMain;
		break;
	case GameScene::Phase::kMain:
		if (fade_->IsFinished()) {
			phase_ = Phase::kFadeOut;
		}
		break;
	case GameScene::Phase::kFadeOut:
		finished_ = true;
		break;
		

	default:
		break;
	}

	// リスタート中の処理
	if (isRestarting_) {
		countdownTime_ -= deltaTime_; // deltaTime_ は経過時間を表す変数と仮定

		if (countdownTime_ <= 0) {
			// カウントダウンが終了したらゲームを再開
			player_->Reset();
			phase_ = Phase::kPlay;
			isRestarting_ = false; // リスタート完了
		}
	}
}

#include "GameScene.h"
#include "base/TextureManager.h"
#include <cassert>
#include "2d/DebugText.h"

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
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();
	audio_ = KamataEngine::Audio::GetInstance();
	camera_.Initialize();


	// マップチップフィールドの生成と初期化
	stage = Stage::stage1;
	modelBlock_ = Model::CreateFromOBJ("block", true);
	mapChipField_ = new MapChipField;
	//mapChipField_->LoadMapChipCsv("Resources/map.csv");
	mapChipField_->LoadMapChipCsv("Resources/testStage.csv");
	LoadStage();

	// 障害物のモデルの生成
	modelHurdle_ = Model::CreateFromOBJ("block", true);

	// ゴールのモデル生成
	modelGoal_ = Model::CreateFromOBJ("block", true);

	// プレイヤー初期化
	modelPlayer_ = Model::CreateFromOBJ("player");
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 16);
	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// カメラコントローラの生成
	cameraController_ = new CameraController();
	// カメラコントローラの初期化
	cameraController_->Initialize();
	// 追従対象をセット
	cameraController_->SetTarget(player_);
	// リセット
	cameraController_->Reset();
	//
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	//
	cameraController_->SetMovebleaArea(cameraArea);


	GenerateBlocks();
}

void GameScene::Update() { 
	NextStage();
	if (needStageReload) {
		LoadStage();
		GenerateBlocks();
		needStageReload = false;
	}
	if (input_->TriggerKey(DIK_0)) {
		player_->setIsGameStart(true);
	}
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

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			worldTransformBlock->UpdateMatrix();
		}
	}

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

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	// 3Dオブジェクト描画後処理
	KamataEngine::Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	KamataEngine::Sprite::PreDraw(commandList);

	///< summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

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
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
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
	}

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
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kHurdle) {
				Goal* newGoal = new Goal();
				// 元の座標を取得してからオフセットを適用
				Vector3 goalPosition = mapChipField_->GetMapChipPositionByIndex(j, i);
				goalPosition.x -= 1.0f; // xに-1のオフセット
				goalPosition.y -= 9.0f; // yに+3のオフセット
				newGoal->Initialize(modelHurdle_, &camera_, goalPosition);
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

}

void GameScene::LoadStage() {
	switch (stage) {
	case Stage::stage1:
		mapChipField_->LoadMapChipCsv("Resources/testStage1.csv");
		break;
	case Stage::stage2:
		mapChipField_->LoadMapChipCsv("Resources/testStage.csv");
		break;
	case Stage::stage3:
		mapChipField_->LoadMapChipCsv("Resources/testStage3.csv");
		break;
	case Stage::stage4:
		mapChipField_->LoadMapChipCsv("Resources/testStage4.csv");
		break;
	default:
		break;
	}
}

void GameScene::NextStage() {
    if (input_->PushKey(DIK_SPACE)) {
    	switch (stage) {
    	case Stage::stage1:
			if (clear_[0]) {
                stage = Stage::stage2;
				needStageReload = true;
			}
    		break;
    	case Stage::stage2:
			if (clear_[1]) {
				stage = Stage::stage3;
				needStageReload = true;
			}
    		break;
    	case Stage::stage3:
			if (clear_[2]) {
				stage = Stage::stage4;
				needStageReload = true;
			}
    		break;
    	case Stage::stage4:
			if (clear_[3]) {
				finished_ = true;
				needStageReload = true;
			}
    		break;
    	default:
    		break;
	    }
    }
}
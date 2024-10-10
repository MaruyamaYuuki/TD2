#include "GameScene.h"
#include "base/TextureManager.h"
#include <cassert>

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
	modelBlock_ = Model::CreateFromOBJ("block", true);
	mapChipField_ = new MapChipField;
	//mapChipField_->LoadMapChipCsv("Resources/map.csv");
	mapChipField_->LoadMapChipCsv("Resources/testStage.csv");

	// 障害物のモデルの生成
	modelHurdle_ = Model::CreateFromOBJ("block", true);

	GenerateBlocks();

	// プレイヤー初期化
	modelPlayer_ = Model::CreateFromOBJ("player");
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 16);
	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);
}

void GameScene::Update() { 

	if (input_->TriggerKey(DIK_0)) {
		player_->setIsGameStart(true);
	}
	// プレイヤー更新
	player_->Update();

	// 障害物の更新
	for (Hurdle* hurdle : hurdles_) {
		hurdle->Update();
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
}

bool GameScene::IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true;
	}
	return false;
}

void GameScene::CheckAllCollision() {

}
#pragma once

#include "2d/Sprite.h"
#include "3d/Camera.h"
#include "3d/Model.h"
#include "3d/ObjectColor.h"
#include "3d/WorldTransform.h"
#include "audio/Audio.h"
#include "base/DirectXCommon.h"
#include "input/Input.h"
#include "Player.h"
#include "MapChipField.h"
#include "3d/ObjectColor.h"

    /// <summary>
    /// ゲームシーン
    /// </summary>
class GameScene {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

    /// <summary>
    /// ブロック生成
    /// </summary>
	void GenerateBlocks();

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;
	KamataEngine::Camera camera_;

	Player* player_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	MapChipField* mapChipFiled_;
	KamataEngine::Model* modelBlock_ = nullptr;
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	// ビュープロジェクション

};
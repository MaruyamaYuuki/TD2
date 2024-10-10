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
#include "Hurdle.h"
#include "Goal.h"
#include "CameraController.h"
#include "3d/DebugCamera.h"

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

    /// <summary>
    /// すべての当たり判定
    /// </summary>
	void CheckAllCollision();

    /// <summary>
    /// 衝突応答
    /// </summary>
	bool IsCollision(const AABB& aabb1, const AABB& aabb2);

    /// <summary>
    /// 終了フラグのgetter
    /// </summary>
	bool IsFinished() const { return finished_; }

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;
	KamataEngine::Camera camera_;

	Player* player_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	MapChipField* mapChipField_;
	KamataEngine::Model* modelBlock_ = nullptr;
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	std::list<Hurdle*> hurdles_;
	KamataEngine::Model* modelHurdle_ = nullptr;

	std::list<Goal*> goals_;
	KamataEngine::Model* modelGoal_ = nullptr;

	CameraController* cameraController_ = nullptr;
	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	// 終了フラグ
	bool finished_ = false;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
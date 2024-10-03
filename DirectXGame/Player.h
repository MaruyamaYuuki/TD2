#pragma once
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "3d/Camera.h"
#include "input/input.h"	
#include "kMath.h"
#include "3d/ObjectColor.h"

/// <summary>
/// プレイヤークラス
/// 
/// </summary>

enum class fallDirection {
	Up,
	Down,
};

class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// ゲームスタートのsetter
	bool setIsGameStart(bool start) { return isGameStart_ = start; }

private:
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::ObjectColor objectColor_;

	/// 移動系
	// ゲームが始まっているかのフラグ(変更不可)
	bool isGameStart_ = false;

	// 移動用Vector3
	KamataEngine::Vector3 move_ = { 0.0f, 0.0f, 0.0f };

	// 落下方向
	bool isDownFall = false;

	// 横移動の上限速度(変更可)
	const float kMaxLimitSpeed_ = 0.2f;
	// 落下速度の上限速度(変更可)
	const float kMaxLimitFallSpeed_ = 0.8f;
	// 移動関数(Updateに入れる)
	void Move();

};

#include "3d/WorldTransform.h"
#include "3d/Camera.h"
#include "3d/Model.h"
#include "2d/Sprite.h"
#include "base/TextureManager.h"
#include "base/DirectXCommon.h"
#pragma once

class Fade {
public:
	// フェードの状態
	enum class Status {
		None,   // フェードなし
		FadeIn, // フェードイン中
		FadeOut, // フェードアウト中
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* commandListv);

	// フェード開始
	void Start(Status status, float duration);

	// フェード開始
	void Stop();

	// フェード終了判定
	bool IsFinished() const;

private:
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::Sprite* sprite_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 現在のフェードの状態
	Status status_ = Status::None;

	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;



};

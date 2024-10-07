#include "Hurdle.h"
#include <cassert>

void Hurdle::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// 引数をメンバ変数に記録
	model_ = model;
	camera_ = camera;
	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	objectColor_.Initialize();
}

void Hurdle::Update() {
	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix();
}

void Hurdle::Draw() { 
	model_->Draw(worldTransform_, *camera_, &objectColor_); }

KamataEngine::Vector3 Hurdle::GetWorldPosition() { 
	KamataEngine::Vector3 worldPos; 

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

AABB Hurdle::GetAABB() { 
	KamataEngine::Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Hurdle::OnCollision(const Player* player) { 
	(void)player; 
}
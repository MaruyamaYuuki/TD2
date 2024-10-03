#include "Player.h"
#include "cassert"
#include "algorithm"
#include "imgui.h"

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera) { 
	assert(model);
	input_ = KamataEngine::Input::GetInstance();
	model_ = model;
	objectColor_.Initialize();
	worldTransform_.Initialize();
	camera_ = camera;
}

void Player::Update() { 
	// ゲームがスタートしていたら更新開始
	if (isGameStart_) {
		Move();
	}
	worldTransform_.UpdateMatrix();
}

void Player::Move() {
	// 自動移動
	move.x += 0.02f;

	// スペースを押して重力を反転
	if (input_->TriggerKey(DIK_SPACE)) {
		isDownFall = !isDownFall;
	}

	if (isDownFall) {
		move.y -= 0.02f;
		move.y = std::clamp(move.y, -kMaxLimitFallSpeed_, 0.0f);
	} else {
		move.y += 0.02f;
		move.y = std::clamp(move.y, 0.0f, kMaxLimitFallSpeed_);
	}


	// clamp処理で移動速度を制限
	move.x = std::clamp(move.x, 0.0f, kMaxLimitSpeed_);

	// 移動量をtranslationに加算
	worldTransform_.translation_ += move;

}

void Player::Draw() { model_->Draw(worldTransform_, *camera_, &objectColor_);}
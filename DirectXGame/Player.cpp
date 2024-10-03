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
	ImGui::Begin("PlayerState");
	ImGui::DragFloat3("Translate", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("Move", &move_.x, 0.1f);
	ImGui::End();
	worldTransform_.UpdateMatrix();
}

void Player::Move() {
	// 自動移動
	move_.x += 0.02f;

	// スペースを押して重力を反転
	if (input_->TriggerKey(DIK_SPACE)) {
		isDownFall = !isDownFall;
	}
	// 画面外に行かないように制限(上下限定)
	if (worldTransform_.translation_.y == 19.0f || worldTransform_.translation_.y == -19.0f) {
		move_.y = 0.0f;
	}

	// 落下方向に応じで変更
	if (isDownFall) {
		move_.y -= 0.02f;
	} else {
		move_.y += 0.02f;
	}

	// clamp処理で移動速度を制限
	move_.x = std::clamp(move_.x, 0.0f, kMaxLimitSpeed_);
	move_.y = std::clamp(move_.y, -kMaxLimitFallSpeed_, kMaxLimitFallSpeed_);

	// 移動量をtranslationに加算
	worldTransform_.translation_ += move_;
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, -19.0f, 19.0f);
	// スペースを押している間y座標を固定する
	/* if (input_->PushKey(DIK_SPACE)) {
		worldTransform_.translation_.y -= move_.y;
		if (isDownFall) {
			move_.y += 0.02f;
		} else {
			move_.y -= 0.02f;
		}
		if (input_->) {
		}
	}*/

}

void Player::Draw() { model_->Draw(worldTransform_, *camera_, &objectColor_);}

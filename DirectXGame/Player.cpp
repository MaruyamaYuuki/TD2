#define NOMINMAX
#include "Player.h"
#include "cassert"
#include "imgui.h"
#include "MapChipField.h"
#include <numbers>
#include <algorithm>

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera, const Vector3& maxMoveableArea) { 
	assert(model);
	input_ = KamataEngine::Input::GetInstance();
	model_ = model;
	objectColor_.Initialize();
	worldTransform_.Initialize();
	worldTransform_.translation_ = {1.0f, 3.0f, 0.0f};
	//worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	maxMoveableArea_ = maxMoveableArea;
	camera_ = camera;
}


void Player::Update() { 
	if (isGameStart_) {
		Move();
	}
	/*ImGui::Begin("PlayerState");
	ImGui::DragFloat3("Translate", &worldTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("Move", &move_.x, 0.1f);
	ImGui::End();*/
	worldTransform_.UpdateMatrix();
}


void Player::Move() {
	// 自動移動
	move_.x += 0.02f;

	// スペースを押して重力を反転
	if (input_->TriggerKey(DIK_SPACE) && !isGoal_) {
		isSwithGravity = true;
		isDownFall = !isDownFall;
		if (udDirection_ != UDDirection::kUp) {
			udDirection_ = UDDirection::kUp;
			turnTimer_ = 0.1f;
			turnFirstRotationX_ = worldTransform_.rotation_.x;
			turnFirstRotationY_ = worldTransform_.rotation_.y;
		} else if (udDirection_ != UDDirection::kDown) {
			udDirection_ = UDDirection::kDown;
			turnTimer_ = 0.1f;
			turnFirstRotationX_ = worldTransform_.rotation_.x;
			turnFirstRotationY_ = worldTransform_.rotation_.y;
		}
	}
	if (turnTimer_ > 0.0f) {
		turnTimer_ += 1.0f / 60 / kTimeTurn_;

		// 左右の自キャラ角度テーブル
		float destinationRotationXTable[] = { 
			std::numbers::pi_v<float>, 
			0.0f,
		};
		float destinationRotationYTable[] = {
		    std::numbers::pi_v<float> * 3.0f / 2.0f,
		    std::numbers::pi_v<float> / 2.0f,
		};
		// 状態に応じた角度を取得する
		float destinationRotationX = destinationRotationXTable[static_cast<uint32_t>(udDirection_)];
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(udDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotation_.x = easeInOut(turnTimer_, turnFirstRotationX_, destinationRotationX);
		worldTransform_.rotation_.y = easeInOut(turnTimer_, turnFirstRotationY_, destinationRotationY);
	}
	if (turnTimer_ > 1.0f) {
		turnTimer_ = 0.0f;
	}
	// 画面外に行かないように制限(上下限定)
	if (worldTransform_.translation_.y == 20.0f || worldTransform_.translation_.y == -20.0f) {
		move_.y = 0.0f;
	}

	// 落下方向に応じで変更
	if (isSwithGravity) {
		move_.y *= (-1.0f);
		isSwithGravity = false;
	} 
	// clamp処理で移動速度を制限
	move_.x = std::clamp(move_.x, 0.0f, kMaxLimitSpeed_);
	move_.y = std::clamp(move_.y, -kMaxLimitFallSpeed_, kMaxLimitFallSpeed_);

	// 移動量をtranslationに加算
	worldTransform_.translation_ += move_;
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, 0.0f, maxMoveableArea_.x);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, -1.0f, maxMoveableArea_.y);

	if (worldTransform_.translation_.x == maxMoveableArea_.x) {
		playerOutCamera = true;
	}

}

void Player::Draw() { model_->Draw(worldTransform_, *camera_, &objectColor_);}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}

AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}


void Player::CollisionGoal(const Goal* goal) {
	(void)goal; 
	isGoal_ = true;
}

void Player::CollisionHurdle(const Hurdle* hurdle) { 
	(void)hurdle; 
	isDead_ = true;
}

void Player::Reset() { 
	worldTransform_.translation_ = {1.0f, 3.0f, 0.0f};
	worldTransform_.rotation_.x = 0;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	worldTransform_.UpdateMatrix();

	isGameStart_ = false;
	isGoal_ = false;
	isDead_ = false;
	playerOutCamera = false;
}

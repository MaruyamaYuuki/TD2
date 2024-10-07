#define NOMINMAX
#include "CameraController.h"
#include "Player.h"

void CameraController::Initialize() { 

	camera_.Initialize();

}

void CameraController::Update() {

	const KamataEngine::Vector3& targetVelocity = target_->GetVelocity();

	// 追従対象のワールドトランスフォームを参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの目標座標を計算
	goalPos.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;


	// 追従対象が画面外に出ないように補正
	camera_.translation_.x = std::max(camera_.translation_.x, targetWorldTransform.translation_.x + kMargin.left);
	camera_.translation_.x = std::min(camera_.translation_.x, targetWorldTransform.translation_.x + kMargin.right);
	// viewProjection_.translation_.y = std::max(viewProjection_.translation_.y, targetWorldTransform.translation_.y + margin.bottom);
	// viewProjection_.translation_.y = std::min(viewProjection_.translation_.y, targetWorldTransform.translation_.y + margin.top);

	// 移動範囲制限
	camera_.translation_.x = std::max(camera_.translation_.x, movebleArea_.left);
	camera_.translation_.x = std::min(camera_.translation_.x, movebleArea_.right);
	// camera_.translation_.y = std::max(viewProjection_.translation_.y, movableArea_.bottom);
	// camera_.translation_.y = std::min(viewProjection_.translation_.y, movableArea_.top);

	// 行列を更新する
	camera_.UpdateMatrix();

}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;

}

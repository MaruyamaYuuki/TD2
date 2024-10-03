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
	// �Q�[�����X�^�[�g���Ă�����X�V�J�n
	if (isGameStart_) {
		Move();
	}
	worldTransform_.UpdateMatrix();
}

void Player::Move() {
	// �����ړ�
	move.x += 0.02f;

	// �X�y�[�X�������ďd�͂𔽓]
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


	// clamp�����ňړ����x�𐧌�
	move.x = std::clamp(move.x, 0.0f, kMaxLimitSpeed_);

	// �ړ��ʂ�translation�ɉ��Z
	worldTransform_.translation_ += move;

}

void Player::Draw() { model_->Draw(worldTransform_, *camera_, &objectColor_);}
#pragma once
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "3d/Camera.h"
#include "input/input.h"	
#include "kMath.h"
#include "3d/ObjectColor.h"

/// <summary>
/// �v���C���[�N���X
/// 
/// </summary>

enum class fallDirection {
	Up,
	Down,
};

class Player {
public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);
	/// <summary>
	/// �X�V
	/// </summary>
	void Update();
	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	// �Q�[���X�^�[�g��setter
	bool setIsGameStart(bool start) { return isGameStart_ = start; }

private:
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::ObjectColor objectColor_;

	/// �ړ��n
	// �Q�[�����n�܂��Ă��邩�̃t���O(�ύX�s��)
	bool isGameStart_ = false;

	// �ړ��pVector3
	KamataEngine::Vector3 move = { 0.0f, 0.0f, 0.0f };

	// ���ړ��̏�����x(�ύX��)
	const float kMaxLimitSpeed_ = 0.2f;
	// �������x�̏�����x(�ύX��)
	const float kMaxLimitFallSpeed_ = 0.3f;
	// �ړ��֐�(Update�ɓ����)
	void Move();

};

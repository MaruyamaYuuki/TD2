#include "3d/Camera.h"
#include "math/MathUtility.h"

//class Player;

/// <summery>
/// �J�����R���g���[��
/// </summary>
class CameraController {

public:
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	//void SetTarget(Player* target) { target_ = target; }

	const KamataEngine::Camera& GetViewProjection() const { return camera_; }

	void Reset();

	void SetMovebleaArea(Rect area) { movebleArea_ = area; };

private:
	// �r���[�v���W�F�N�V����
	KamataEngine::Camera camera_;
	//Player* target_ = nullptr;
	// �Ǐ]�Ώۂ̃J�����̍��W�̍��i�I�t�Z�b�g�j
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};
	// �J�����ړ��͈�
	Rect movebleArea_ = {0, 100, 0, 100};
	// �J�����̖ڕW���W
	KamataEngine::Vector3 goalPos;
	// ���W��Ԋ���
	static inline const float kInterpolationRate = 1.5f;
	// ���x�|����
	static inline const float kVelocityBias = 30.0f;
	// �Ǐ]�Ώۂ̊e�����ւ̃J�����ړ��͈�
	static inline const Rect kMargin = {-5.0f, 5.0f, -4.0f, 4.0f};
};
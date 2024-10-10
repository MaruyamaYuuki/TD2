#include "3d/Camera.h"
#include "math/MathUtility.h"

class Player;

/// <summery>
/// カメラコントロール
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
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void SetTarget(Player* target) { target_ = target; }

	const KamataEngine::Camera& GetViewProjection() const { return camera_; }

	void Reset();

	void SetMovebleaArea(Rect area) { movebleArea_ = area; };

private:
	// ビュープロジェクション
	KamataEngine::Camera camera_;
	//Player* target_ = nullptr;
	// 追従対象のカメラの座標の差（オフセット）
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};
	// カメラ移動範囲
	Rect movebleArea_ = {0, 100, 0, 100};
	// カメラの目標座標
	KamataEngine::Vector3 goalPos;
	// 座標補間割合
	static inline const float kInterpolationRate = 1.5f;
	// 速度掛け率
	static inline const float kVelocityBias = 30.0f;
	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect kMargin = {-5.0f, 5.0f, -4.0f, 4.0f};

	Player* target_ = nullptr;
};
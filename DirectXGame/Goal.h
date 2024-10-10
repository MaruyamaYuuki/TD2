#include "3d/Model.h"
#include "3d/ObjectColor.h"
#include "3d/WorldTransform.h"
#include "AABB.h"
#include "math/Vector3.h"

class Player;

/// <summary>
/// ゴール
/// </summary>
class Goal {
public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	KamataEngine::Vector3 GetWorldPosition();

	AABB GetAABB();

	void OnCollision(const Player* player);

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	// オブジェクトカラー
	KamataEngine::ObjectColor objectColor_;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
};
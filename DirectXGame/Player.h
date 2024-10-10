#pragma once
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "3d/Camera.h"
#include "input/input.h"	
#include "kMath.h"
#include "3d/ObjectColor.h"
#include "AABB.h"

/// <summary>
/// プレイヤークラス
/// 
/// </summary>

class MapChipField;
class Goal;

enum class fallDirection {
	Up,
	Down,
};

class Player {
public:
	// マップとの当たり判定情報
	struct CollisionMapInfo {
		bool isWallHit = false;
		KamataEngine::Vector3 MovePoint;
	};
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, const KamataEngine::Vector3& maxMoveableArea);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// ゲームスタートのsetter
	bool setIsGameStart(bool start) { return isGameStart_ = start; }

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	// マップチップの設定
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const KamataEngine::Vector3& GetVelocity() const { return move_; }

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition();

	// AABBを取得
	AABB GetAABB();

	// ゴールとの衝突応答
	void CollisionGoal(const Goal* goal);

private:
	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::ObjectColor objectColor_;

	/// 移動系
	// ゲームが始まっているかのフラグ(変更不可)
	bool isGameStart_ = false;

	// 最大移動距離
	KamataEngine::Vector3 maxMoveableArea_ = {0.0f, 0.0f, 0.0f};

	// 移動用Vector3
	KamataEngine::Vector3 move_ = { 0.0f, 0.0f, 0.0f };

	// 上下
	enum class UDDirection {
		kUp,
		kDown,
	};

	// 上下
	UDDirection udDirection_ = UDDirection::kDown;

	// 回転タイマー
	float turnTimer_ = 0.0f;
	// 回転完了までの時間
	float kTimeTurn_ = 0.4f;
	// 初期角度
	float turnFirstRotationX_ = 0.0f;
	float turnFirstRotationY_ = 0.0f;

	// 初期の落下方向
	bool isDownFall = true;

	// 横移動の上限速度(変更可)
	const float kMaxLimitSpeed_ = 0.1f;
	// 落下速度の上限速度(変更可)
	const float kMaxLimitFallSpeed_ = 0.4f;
	// 移動関数(Updateに入れる)
	void Move();

	/// マップチップの当たり判定
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// マップ当たり判定
	void MapCollision(CollisionMapInfo& info);

	// 上下左のマップ当たり判定(右スクロールのため右の当たり判定は除外)
	void MapCollisionUpside(CollisionMapInfo& info);
	void MapCollisionUnderside(CollisionMapInfo& info);
	void MapCollisionLeftside(CollisionMapInfo& info);
	
	// 上記の計算結果を反映
	void ReturnMove(const CollisionMapInfo& info);

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	static inline const float kBlank = 1.0f;

	// プレイヤーの生存状態
	bool isDead_ = false;

};

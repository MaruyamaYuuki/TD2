#define NOMINMAX
#include "Player.h"
#include "cassert"
#include "imgui.h"
#include "MapChipField.h"
#include <numbers>
#include <algorithm>

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) { 
	assert(model);
	input_ = KamataEngine::Input::GetInstance();
	model_ = model;
	objectColor_.Initialize();
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	camera_ = camera;
}

void Player::ReturnMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_ += info.MovePoint;
};

void Player::Update() { 
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
	move_.x += 0.01f;

	// スペースを押して重力を反転
	if (input_->TriggerKey(DIK_SPACE)) {
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
	if (worldTransform_.translation_.y == 19.0f || worldTransform_.translation_.y == -19.0f) {
		move_.y = 0.0f;
	}

	// 落下方向に応じで変更
	if (isDownFall) {
		move_.y -= 0.02f;
	} else {
		move_.y += 0.02f;
	}

	// ゲームがスタートしていたら更新開始
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.MovePoint = move_;
	MapCollision(collisionMapInfo);
	ReturnMove(collisionMapInfo);

	// clamp処理で移動速度を制限
	move_.x = std::clamp(move_.x, 0.0f, kMaxLimitSpeed_);
	move_.y = std::clamp(move_.y, -kMaxLimitFallSpeed_, kMaxLimitFallSpeed_);

	// 移動量をtranslationに加算
	worldTransform_.translation_ += move_;
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, -19.0f, 19.0f);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, 0.0f, 10.0f);
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

void Player::MapCollision(CollisionMapInfo& info) { 
	MapCollisionUpside(info);
	MapCollisionUnderside(info);
	MapCollisionLeftside(info);
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::MapCollisionUnderside(CollisionMapInfo& info) {
	// 下降あり？
	if (info.MovePoint.y <= 0) {
		return;
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真下の当たり判定を行う
	bool hit = false;

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ /* + info.MovePoint*/, static_cast<Corner>(i));
	}
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
  		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight * 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.MovePoint.y = std::min(0.0f, rect.bottom - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
		move_.y = std::min(0.0f, rect.bottom - worldTransform_.translation_.y + (kHeight / 2.0f + kBlank));
	}
}

void Player::MapCollisionUpside(CollisionMapInfo& info) {
	// 上昇あり？
	if (info.MovePoint.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.MovePoint, static_cast<Corner>(i));
	}


	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
 		hit = true;
	}
	// ブロックにヒット
	if (hit) {
		//// 現在座標が壁の外か判定
		//MapChipField::IndexSet indexSetNow;
		//indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));
		//if (indexSetNow.yIndex != indexSet.yIndex) {
		//	// めり込みを排除する方向に移動量を設定する
		//	indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.MovePoint + Vector3(0, +kHeight / 2.0f, 0));
		//	// めり込み先のブロックの範囲矩形
		//	MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		//	info.MovePoint.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		//	move_.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		//}
	}
}

void Player::MapCollisionLeftside(CollisionMapInfo& info) {
	// 左移動あり？
	if (info.MovePoint.x < 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionNew;
	std::array<Vector3, kNumCorner> positionNewY;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.MovePoint, static_cast<Corner>(i));
		positionNewY[i] = CornerPosition(worldTransform_.translation_, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;
	// 左上点の判定

	MapChipField::IndexSet indexSet;
	MapChipField::IndexSet indexSetY;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	indexSetY = mapChipField_->GetMapChipIndexSetByPosition(positionNewY[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSetY.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSetY.yIndex);
	if (mapChipType == MapChipType::kHurdle && mapChipTypeNext != MapChipType::kHurdle) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	indexSetY = mapChipField_->GetMapChipIndexSetByPosition(positionNewY[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSetY.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSetY.yIndex);
	if (mapChipType == MapChipType::kHurdle && mapChipTypeNext != MapChipType::kHurdle) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		isDead_ = true;
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
}
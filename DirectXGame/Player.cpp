#include "Player.h"
#include "cassert"
#include "algorithm"
#include "imgui.h"
#include "MapChipField.h"

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) { 
	assert(model);
	input_ = KamataEngine::Input::GetInstance();
	model_ = model;
	objectColor_.Initialize();
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	camera_ = camera;
}

void Player::Update() { 
	// ゲームがスタートしていたら更新開始
	if (isGameStart_) {
		Move();
	}
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.MovePoint = move_;
	MapCollision(collisionMapInfo);
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

void Player::MapCollision(CollisionMapInfo& info) { 
	MapCollisionUnderside(info);
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
	// 真下の当たり判定を行う
	bool hit = false;

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.MovePoint, static_cast<Corner>(i));
	}
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		move_.y = 0.0f;
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
		move_.y = 0.0f;
	}
}

void Player::MapCollisionLeftside(CollisionMapInfo& info) {
	// 左移動あり？
	if (info.MovePoint.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.MovePoint, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;
	// 左上点の判定

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {

	}
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_, &objectColor_);}

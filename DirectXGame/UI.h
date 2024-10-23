#pragma once
#include "3d/Model.h"
#include "3d/WorldTransform.h"
#include "3d/Camera.h"
#include "input/input.h"
#include "2d/Sprite.h"
#include "math/Vector2.h"

class Player;

/// <summary>
/// UI
/// </summary>
class UI {
public:
	void Initialize();

	void Update(bool death, bool goal,bool allClear);

	void Draw(bool death, bool goal, bool start, bool allClear);

	bool IsSerect() const { return serect_; }

	bool IsSerectON() const { return serectON; }

private:
	KamataEngine::Input* input_ = nullptr;
	bool serect_ = false;
	bool serectON = false;

	KamataEngine::Vector2 cursorPosition = {0, 0};

	uint32_t textureHandleStartGuide_ = 0;
	uint32_t textureHandleCtrGuide_ = 0;
	uint32_t textureHandleShadow_ = 0;
	uint32_t textureHadleClear_ = 0;
	uint32_t textureHadleGameOver_ = 0;
	uint32_t textureHandleCursor_ = 0;
	uint32_t textHandleAllClear_ = 0;

	KamataEngine::Sprite* spriteStartGuide_ = nullptr;
	KamataEngine::Sprite* spriteCtrlGuide_ = nullptr;
	KamataEngine::Sprite* spriteShadow_ = nullptr;
	KamataEngine::Sprite* spriteClear_ = nullptr;
	KamataEngine::Sprite* spriteGameOver_ = nullptr;
	KamataEngine::Sprite* spriteCursor_ = nullptr;
	KamataEngine::Sprite* spriteAllClear_ = nullptr;

};
#include "UI.h"
#include "base/TextureManager.h"
#include "2d/DebugText.h"

void UI::Initialize() { 
	input_ = KamataEngine::Input::GetInstance();
	cursorPosition = {400, 410};

	textureHandleStartGuide_ = KamataEngine::TextureManager::Load("startGuide.png");
	spriteStartGuide_ = KamataEngine::Sprite::Create(textureHandleStartGuide_, {0, 0});

	textureHandleCtrGuide_ = KamataEngine::TextureManager::Load("ctrlGuide.png");
	spriteCtrlGuide_ = KamataEngine::Sprite::Create(textureHandleCtrGuide_, {0, 0});

	textureHandleShadow_ = KamataEngine::TextureManager::Load("shadow.png");
	spriteShadow_ = KamataEngine::Sprite::Create(textureHandleShadow_, {0, 0});

	textureHadleClear_ = KamataEngine::TextureManager::Load("clear.png");
	spriteClear_ = KamataEngine::Sprite::Create(textureHadleClear_, {0, 0});

	textureHadleGameOver_ = KamataEngine::TextureManager::Load("GameOver.png");
	spriteGameOver_ = KamataEngine::Sprite::Create(textureHadleGameOver_, {0, 0});

	textureHandleCursor_ = KamataEngine::TextureManager::Load("cursor.png");
	spriteCursor_ = KamataEngine::Sprite::Create(textureHandleCursor_, cursorPosition);

	textHandleAllClear_ = KamataEngine::TextureManager::Load("allClear.png");
	spriteAllClear_ = KamataEngine::Sprite::Create(textHandleAllClear_, {0, 0});
}

void UI::Update(bool death, bool goal) {
	if (death || goal) {

		if (input_->TriggerKey(DIK_UPARROW) || input_->TriggerKey(DIK_DOWNARROW)) {
			if (!serect_) {
				serect_ = true;
				cursorPosition = {400, 520};

			    spriteCursor_ = KamataEngine::Sprite::Create(textureHandleCursor_, cursorPosition);
			} else {
				serect_ = false;
				cursorPosition = {400, 410};
			    spriteCursor_ = KamataEngine::Sprite::Create(textureHandleCursor_, cursorPosition);
			}
		}
	}

}

void UI::Draw(bool death, bool goal, bool start, bool allClear) { 
	if (death) {
		spriteShadow_->Draw();
		spriteGameOver_->Draw();
		spriteCursor_->Draw();
	} else if (goal) {
		spriteShadow_->Draw();
		if (allClear) {
            spriteAllClear_->Draw();
		} else {
    		spriteClear_->Draw();
    		spriteCursor_->Draw();
		}
	} else if (!start) {
		spriteStartGuide_->Draw();
	} else {
		spriteCtrlGuide_->Draw();
	}
}



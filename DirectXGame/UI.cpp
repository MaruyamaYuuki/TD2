#include "UI.h"
#include "base/TextureManager.h"

void UI::Initialize() { 
	input_ = KamataEngine::Input::GetInstance(); 

	textureHandleStartGuide_ = KamataEngine::TextureManager::Load("startGuide.png");
	spriteStartGuide_ = KamataEngine::Sprite::Create(textureHandleStartGuide_, {0, 0});

	textureHandleCtrGuide_ = KamataEngine::TextureManager::Load("ctrlGuide.png");
	spriteCtrlGuide_ = KamataEngine::Sprite::Create(textureHandleCtrGuide_, {0, 0});

	textureHandleShadow_ = KamataEngine::TextureManager::Load("shadow.png");
	spriteShadow_ = KamataEngine::Sprite::Create(textureHandleShadow_, {0, 0});

	textureHadleClear_ = KamataEngine::TextureManager::Load("StageClear.png");
	spriteClear_ = KamataEngine::Sprite::Create(textureHadleClear_, {0, 0});

	textureHadleGameOver_ = KamataEngine::TextureManager::Load("GameOver.png");
	spriteGameOver_ = KamataEngine::Sprite::Create(textureHadleGameOver_, {0, 0});

	textureHandleCursor_ = KamataEngine::TextureManager::Load("cursor.png");
	spriteCursor_ = KamataEngine::Sprite::Create(textureHandleCursor_, cursorPosition);
}

void UI::Update(bool death, bool goal) {
	if (death || goal) {
		if (input_->TriggerKey(DIK_UPARROW) || input_->TriggerKey(DIK_DOWNARROW)) {
			if (!serect_) {
				serect_ = true;
			} else {
				serect_ = false;
			}
		}
	}
}

void UI::Draw(bool death, bool goal, bool start) { 
	if (death) {
		spriteShadow_->Draw();
		spriteGameOver_->Draw();

	} else if (goal) {
		spriteShadow_->Draw();
		spriteClear_->Draw();
	} else if (!start) {
		spriteStartGuide_->Draw();
	} else {
		spriteCtrlGuide_->Draw();
	}
}



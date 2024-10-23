#include "TitleScene.h"

using namespace KamataEngine;

TitleScene::TitleScene() {}

TitleScene::~TitleScene() { 
	delete fade_; 
}

void TitleScene::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();
	audio_ = KamataEngine::Audio::GetInstance();
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
	camera_.Initialize();
	titleSprite1_ = TextureManager::Load("start/Title.png");
	titleSprite2_ = TextureManager::Load("start/Start.png");
	titleSprite3_ = TextureManager::Load("start/StartBuck.png");

	title1_ = Sprite::Create(titleSprite1_, {200, 180});
	title2_ = Sprite::Create(titleSprite2_, {400, 480});
	title3_ = Sprite::Create(titleSprite3_, {0, 0});

}

void TitleScene::Update() { 
	switch (phase_) {
	case TitleScene::Phase::kFadeIn:
		if (input_->TriggerKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kMain;
		}
		fade_->Update();
		break;
	case TitleScene::Phase::kMain:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kFadeOut;
		}
		break;
	case TitleScene::Phase::kFadeOut:
		finished_ = true;
		break;
	default:
		break;
	}
}

void TitleScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	KamataEngine::Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの処理を追加できる
	/// </summary>
	title3_->Draw();
	title2_->Draw();
	title1_->Draw();
	// スプライト処理後描画
	KamataEngine::Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	KamataEngine::Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// プレイヤー描画

	// 3Dオブジェクト描画後処理
	KamataEngine::Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	KamataEngine::Sprite::PreDraw(commandList);

	///< summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	fade_->Draw(commandList);
	// スプライト描画後処理
	KamataEngine::Sprite::PostDraw();

#pragma endregion
}
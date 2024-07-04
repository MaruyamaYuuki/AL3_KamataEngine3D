#include "TitleScene.h"
#include <DebugText.h>

void TitleScene::Initialize() {

}

void TitleScene::Update() {
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}
}

void TitleScene::Draw() {

}
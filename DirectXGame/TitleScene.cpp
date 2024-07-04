#include "TitleScene.h"
#include <DebugText.h>

void TitleScene::Initialize() {

}

void TitleScene::Update() {
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
		DebugText::GetInstance()->ConsolePrintf("Push Space\n");
	}
}

void TitleScene::Draw() {

}
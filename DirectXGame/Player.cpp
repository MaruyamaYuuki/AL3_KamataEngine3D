#include "Player.h"
#include <cassert>

void Player::Initialize(Model* model, uint32_t textureHandle){
    // NULLポインタチェック
	assert(model);

	model_ = model;
	textureHandle_ = textureHandle;


};

void Player::Update(){};

void Player::Draw(){};
#include "WorldTransform.h"
#include "myMath.h"

void WorldTransform::UpdateMatrix() { 

	matWorld_ = myMath::MakeAffineMatrix(scale_, rotation_, translation_);
	// 定数バッファに転送する
	TransferMatrix();
}
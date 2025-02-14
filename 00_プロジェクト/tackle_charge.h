//=============================================
//
//タックルチャージ中のエフェクト[tackle_charge.h]
//Auther Matsuda Towa
//
//=============================================
#include "main.h"
#include "billboard.h"

#ifndef _TACKLE_CHARGE_H_ //これが定義されてないとき

#define _TACKLE_CHARGE_H_

class CTackleCharge : public CBillboard
{
public:
	static const int PRIORITY = 10; //描画順
	static const D3DXVECTOR3 SIZE; //サイズ

	CTackleCharge(int nPriority = PRIORITY);
	~CTackleCharge()override;
	HRESULT Init()override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	static CTackleCharge* Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col);

private:
	static const std::string TEXTURE_NAME; //テクスチャの名前
};

#endif
//=============================================
//
//タックルチャージ中のエフェクト[tackle_charge.cpp]
//Auther Matsuda Towa
//
//=============================================
#include "tackle_charge.h"
#include "manager.h"

//読み込むブロック情報のファイル
const std::string CTackleCharge::TEXTURE_NAME = "data\\TEXTURE\\ult_effect.png";
const D3DXVECTOR3 CTackleCharge::SIZE = {180.0f,180.0f,0.0f};

//=============================================
//コンストラクタ
//=============================================
CTackleCharge::CTackleCharge(int nPriority):CBillboard(nPriority)
{
}

//=============================================
//デストラクタ
//=============================================
CTackleCharge::~CTackleCharge()
{
}

//=============================================
//初期化
//=============================================
HRESULT CTackleCharge::Init()
{
	//テクスチャ取得
	CTexture* pTexture = CManager::GetInstance()->GetTexture();

	//テクスチャ設定
	BindTexture(pTexture->GetAddress(pTexture->Regist(&TEXTURE_NAME))); 

	//親クラスの初期化
	CBillboard::Init();

	//頂点座標
	SetVtx(D3DXVECTOR3(0.0f, 0.0f, -1.0f));
	return S_OK;
}

//=============================================
//終了
//=============================================
void CTackleCharge::Uninit()
{
	//親クラスの終了
	CBillboard::Uninit();
}

//=============================================
//更新
//=============================================
void CTackleCharge::Update()
{
	//親クラスの更新
	CBillboard::Update();

	//頂点座標
	SetVtx(D3DXVECTOR3(0.0f, 0.0f, -1.0f));
}

//=============================================
//描画
//=============================================
void CTackleCharge::Draw()
{
	CRenderer* pRender = CManager::GetInstance()->GetRenderer();

	LPDIRECT3DDEVICE9 pDevice = pRender->GetDevice();

	//αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//ライトを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//親クラスの描画
	CBillboard::Draw();

	//ライトを有効に戻す
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//=============================================
//生成
//=============================================
CTackleCharge* CTackleCharge::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col)
{
	CTackleCharge* pTackleCharge = new CTackleCharge;

	if (pTackleCharge == nullptr) { return nullptr; }

	pTackleCharge->SetPos(pos);
	pTackleCharge->SetSize(size);
	pTackleCharge->SetColor(col);
	pTackleCharge->SetType(OBJECT_TYPE_EFFECT);

	pTackleCharge->Init();

	return pTackleCharge;
}

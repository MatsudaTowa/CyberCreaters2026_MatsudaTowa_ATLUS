//=============================================
//
//ボスの敵の処理[boss_enemy.cpp]
//Auther Matsuda Towa
//
//=============================================
#include "boss_enemy.h"
#include "boss_behavior.h"

const float CBossEnemy::FIND_PLAYER_DISTANCE = 300.0f; //この値までだったら追いかける
const float CBossEnemy::LOST_PLAYER_DISTANCE = 100.0f; //この値以上だったら見失う
//影のサイズ
const D3DXVECTOR3 CBossEnemy::SHADOW_SIZE = { 30.0f, 0.0, 30.0f };

//=============================================
//コンストラクタ
//=============================================
CBossEnemy::CBossEnemy(int nPriority):CEnemy(nPriority),
m_pChase(nullptr),										//追跡処理
m_pWandering(nullptr),									//徘徊処理
m_pConfusion(nullptr),									//混乱処理
m_pTackle(nullptr),										//タックル処理
m_pSearch(nullptr),										//探索処理
m_pRampage(nullptr),									//暴走処理
m_pDashEffect(nullptr),									//ダッシュエフェクト
m_pTackleCharge(nullptr)								//タックル前のエフェクト
{
	if (m_pWandering == nullptr)
	{
		m_pWandering = new CBossWandering;
	}
	if (m_pChase == nullptr)
	{
		m_pChase = new CBossChase;
	}
	if (m_pConfusion == nullptr)
	{
		m_pConfusion = new CBossConfusion;
	}
	if (m_pStan == nullptr)
	{
		m_pStan = new CEnemyStan;
	}
	if (m_pGunAttack == nullptr)
	{
		m_pGunAttack = new CBossGunAttack;
	}
	if (m_pTackle == nullptr)
	{
		m_pTackle = new CBossTackle;
	}
	if (m_pRampage == nullptr)
	{
		m_pRampage = new CBossRampage;
	}
	if (m_pSearch == nullptr)
	{
		m_pSearch = new CBossSearch;
	}
}

//=============================================
//デストラクタ
//=============================================
CBossEnemy::~CBossEnemy()
{
	if (m_pBossState != nullptr)
	{
		delete m_pBossState;
	}
	if (m_pWandering != nullptr)
	{
		delete m_pWandering;
	}
	if (m_pChase != nullptr)
	{
		delete m_pChase;
	}
	if (m_pConfusion != nullptr)
	{
		delete m_pConfusion;
	}
	if (m_pTackle != nullptr)
	{
		delete m_pTackle;
	}
	if (m_pRampage != nullptr)
	{
		delete m_pRampage;
	}
	if (m_pSearch != nullptr)
	{
		delete m_pSearch;
	}
	if (m_pDashEffect != nullptr)
	{
		m_pDashEffect->Uninit();
		m_pDashEffect = nullptr;
	}
	if (m_pTackleCharge != nullptr)
	{
		m_pTackleCharge->Uninit();
		m_pTackleCharge = nullptr;
	}
}

//=============================================
//初期化
//=============================================
HRESULT CBossEnemy::Init()
{
	if (m_pBossState == nullptr)
	{
		m_pBossState = new CWanderingState;
	}
	//銃初期化
	if (m_pGun == nullptr)
	{
		m_pGun = new CMiniGun;

		m_pGun->Init();
	}

	SetLife(DEFAULT_LIFE);
	//影のサイズ設定
	SetShadowSize(SHADOW_SIZE);
	//親クラスの初期化を呼ぶ
	CEnemy::Init();
	return S_OK;
}

//=============================================
//終了
//=============================================
void CBossEnemy::Uninit()
{
	//親クラスの終了処理を呼ぶ
	CEnemy::Uninit();
}

//=============================================
//更新
//=============================================
void CBossEnemy::Update()
{
	CEnemy::Update();

	ProcessState(); //各ステートの実行処理

	Motion(NUM_PARTS); //モーション処理

#ifdef _DEBUG
	if (CManager::GetInstance()->GetKeyboard()->GetTrigger(DIK_9))
	{
		SetLife(300);
	}
#endif // _DEBUG


	for (int nCnt = 0; nCnt < GetNumParts(); nCnt++)
	{
		if (m_apModel[nCnt]->GetColisionBlockInfo().bColision_X)
		{
			m_HitAxis = X;
			break;
		}
		else if (m_apModel[nCnt]->GetColisionBlockInfo().bColision_Z)
		{
			m_HitAxis = Z;
			break;
		}
		else
		{
			m_HitAxis = NONE;
		}
	}
}

//=============================================
//ステートの実行処理
//=============================================
void CBossEnemy::ProcessState()
{
	m_pBossState->Chase(this);

	m_pBossState->Wandering(this);

	m_pBossState->Stan(this);

	m_pBossState->Confusion(this);

	m_pBossState->Tackle(this);

	m_pBossState->Search(this);

	m_pBossState->Rampage(this);
}

//=============================================
//描画
//=============================================
void CBossEnemy::Draw()
{
	//デバッグ表示
	DrawDebug();
	CEnemy::Draw();
}

//=============================================
//ボスの状態切り替え
//=============================================
void CBossEnemy::ChangeState(CBossState* state)
{
	//今のステートを消し引数のステートに切り替える
	if (m_pBossState != nullptr)
	{
		m_pBossState->End(this);
		delete m_pBossState;
		m_pBossState = state;
		m_pBossState->Start(this);
	}
}

//=============================================
//タックルの処理
//=============================================
void CBossEnemy::TackleAction()
{
	SetMotion(CBossEnemy::MOTION_TACKLE);

	if (m_pDashEffect == nullptr)
	{
		float fAngle = atan2f(sinf(GetRot().y), cosf(GetRot().y));

		//ダッシュエフェクト生成
		m_pDashEffect = CDashEffect::Create({m_apModel[3]->GetMtxWorld()._41,m_apModel[3]->GetMtxWorld()._42 - 100.0f,m_apModel[3]->GetMtxWorld()._43 }
		, { 0.0f,fAngle,0.0f });
	}

	//移動量取得
	D3DXVECTOR3 move = GetMove();

	//移動量加算
	move.x += sinf(GetRot().y) *  GetSpeed() * -15.0f;
	move.z += cosf(GetRot().y) *  GetSpeed() * -15.0f;

	//移動量代入
	SetMove(move);

	if ( m_pDashEffect != nullptr)
	{//エフェクトがあったら
	 //エフェクトを動かす
		 m_pDashEffect->SetPos({ m_apModel[3]->GetMtxWorld()._41,m_apModel[3]->GetMtxWorld()._42 - 100.0f,m_apModel[3]->GetMtxWorld()._43 });
	}

	//自分の方向を取得
	D3DXVECTOR3 vec = { sinf( GetRot().y + D3DX_PI), 0.0f, cosf( GetRot().y + D3DX_PI) };

	//プレイヤーとの判定チェック
	 ColisionPlayer();
}

//=============================================
//プレイヤーとの当たり判定
//=============================================
void CBossEnemy::ColisionPlayer()
{
	for (int nPartsCnt = 0; nPartsCnt < GetNumParts(); ++nPartsCnt)
	{
		D3DXVECTOR3 pos = { m_apModel[nPartsCnt]->GetMtxWorld()._41,m_apModel[nPartsCnt]->GetMtxWorld()._42,m_apModel[nPartsCnt]->GetMtxWorld()._43 };
		D3DXVECTOR3 Minpos = m_apModel[nPartsCnt]->GetMin();
		D3DXVECTOR3 Maxpos = m_apModel[nPartsCnt]->GetMax();
		for (int nCnt = 0; nCnt < MAX_OBJECT; nCnt++)
		{
			//オブジェクト取得
			CObject* pObj = CObject::Getobject(CPlayer::PLAYER_PRIORITY, nCnt);
			if (pObj != nullptr)
			{//オブジェクトに要素が入っていたら
				//タイプ取得
				CObject::OBJECT_TYPE type = pObj->GetType();

				//ブロックとの当たり判定
				if (type == CObject::OBJECT_TYPE::OBJECT_TYPE_PLAYER)
				{
					//安全にダウンキャスト
					CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObj);

					if (pPlayer->GetEnemyColision())
					{//プレイヤーがエネミーと当たる状態のとき
						CheckColisionPlayer(pPlayer, nPartsCnt, pos, Minpos, Maxpos);
					}
				}
			}
		}
	}
}

//=============================================
//プレイヤーとの当たり判定チェック
//=============================================
void CBossEnemy::CheckColisionPlayer(CPlayer* pPlayer, int nPartsCnt, const D3DXVECTOR3& pos, const D3DXVECTOR3& Minpos, const D3DXVECTOR3& Maxpos)
{
	for (int nPartsCnt = 0; nPartsCnt < pPlayer->GetNumParts(); nPartsCnt++)
	{
		D3DXVECTOR3 playerpos = { pPlayer->m_apModel[nPartsCnt]->GetMtxWorld()._41,
			pPlayer->m_apModel[nPartsCnt]->GetMtxWorld()._42,
			pPlayer->m_apModel[nPartsCnt]->GetMtxWorld()._43 };

		D3DXVECTOR3 PlayerMinpos = pPlayer->m_apModel[nPartsCnt]->GetMin();
		D3DXVECTOR3 PlayerMaxpos = pPlayer->m_apModel[nPartsCnt]->GetMax();
		CColision::COLISION colision = CManager::GetInstance()->GetColision()->CheckColisionSphere(pos, Minpos, Maxpos,
			playerpos, PlayerMinpos, PlayerMaxpos);

		if (colision == CColision::COLISION::COLISON_Z || colision == CColision::COLISION::COLISON_X)
		{
			D3DXVECTOR3 Playermove = D3DXVECTOR3(sinf(GetRot().y + D3DX_PI) * 15.0f,
				30.0f, cosf(GetRot().y + D3DX_PI) * 15.0f);
			// X軸衝突時の処理
			pPlayer->SetMove(Playermove);
			pPlayer->ChangePlayerState(new CBlownState);
			pPlayer->Damage(m_pTackle->TACKLE_DAMAGE);
		}
	}
}
//=============================================
//ウルトヒット処理
//=============================================
void CBossEnemy::MediumUltHit(D3DXVECTOR3 UltPos, int nDamage)
{
	ChangeState(new CBossStanState);
	//親クラスのヒット処理
	CEnemy::MediumUltHit(UltPos, nDamage);
}

//=============================================
//ブロックとの判定
//=============================================
void CBossEnemy::HitBlock(int NumParts)
{
	CCharacter::HitBlock(NumParts);

	for (int nPartsCnt = 0; nPartsCnt < NumParts; ++nPartsCnt)
	{
		D3DXVECTOR3 pos = { m_apModel[nPartsCnt]->GetMtxWorld()._41,m_apModel[nPartsCnt]->GetMtxWorld()._42,m_apModel[nPartsCnt]->GetMtxWorld()._43 };
		D3DXVECTOR3 oldpos = m_apModel[nPartsCnt]->GetOldPos();
		D3DXVECTOR3 Minpos = m_apModel[nPartsCnt]->GetMin();
		D3DXVECTOR3 Maxpos = m_apModel[nPartsCnt]->GetMax();
		for (int nCnt = 0; nCnt < MAX_OBJECT; nCnt++)
		{
			if (m_apModel[nPartsCnt]->GetColisionBlockInfo().bColision_X
				|| m_apModel[nPartsCnt]->GetColisionBlockInfo().bColision_Z)
			{
				
				break;
			}
		}
	}
}

//=============================================
//当たってない判定に
//=============================================
void CBossEnemy::ColisionReset()
{
	for (int nCntParts = 0; nCntParts < GetNumParts(); nCntParts++)
	{
		m_apModel[nCntParts]->GetColisionBlockInfo().bColision_X = false;
		m_apModel[nCntParts]->GetColisionBlockInfo().bColision_Z = false;
	}
}

//=============================================
//ボスのデバッグ表示
//=============================================
void CBossEnemy::DrawDebug()
{
#ifdef _DEBUG
	m_pWandering->DrawDebug();
	m_pBossState->DrawDebug();
	m_pChase->DrawDebug();
	LPD3DXFONT pFont = CManager::GetInstance()->GetRenderer()->GetFont();
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	char aStr[256];

	for (int nCnt = 0; nCnt < GetNumParts(); nCnt++)
	{
		if (m_apModel[nCnt]->GetColisionBlockInfo().bColision_X)
		{
			sprintf(&aStr[0], "\n\n\n\n\n\nHP:%d move:%.1f,%.1f,%.1f rot:%.1f,%.1f,%.1f 当たり判定:X", GetLife(), GetMove().x, GetMove().y, GetMove().z, GetRot().x, GetRot().y, GetRot().z);
		}
		else if (m_apModel[nCnt]->GetColisionBlockInfo().bColision_Z)
		{
			sprintf(&aStr[0], "\n\n\n\n\n\nHP:%d move:%.1f,%.1f,%.1f rot:%.1f,%.1f,%.1f 当たり判定:Z", GetLife(), GetMove().x, GetMove().y, GetMove().z, GetRot().x, GetRot().y, GetRot().z);
		}
		else
		{
			sprintf(&aStr[0], "\n\n\n\n\n\nHP:%d move:%.1f,%.1f,%.1f rot:%.1f,%.1f,%.1f 当たり判定:当たってない", GetLife(), GetMove().x, GetMove().y, GetMove().z, GetRot().x, GetRot().y, GetRot().z);
		}
	}

	//テキストの描画
	pFont->DrawText(NULL, &aStr[0], -1, &rect, DT_RIGHT, D3DCOLOR_RGBA(255, 0, 0, 255));
#endif // _DEBUG
}

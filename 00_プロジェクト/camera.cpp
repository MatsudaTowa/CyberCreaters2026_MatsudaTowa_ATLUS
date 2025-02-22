//=============================================
//
//3DTemplate[camera.cpp]
//Auther Matsuda Towa
//
//=============================================
#include "camera.h"
#include "renderer.h"
#include "manager.h"
#include "object.h"
#include "player.h"
#include "player.h"

//通常の移動速度
const float CCamera::DEFAULT_MOVE = 1.0f;

//通常の移動速度
const float CCamera::DAMPING_COEFFICIENT = 0.2f;

//通常状態のYの距離
const float CCamera::DEFAULT_LENGTH_Y = 200.0f;

//通常状態のZの距離
const float CCamera::DEFAULT_LENGTH_Z = 500.0f;

//バードビュー時のYの距離
const float CCamera::BIRDVIEW_LENGTH_Y = 300.0f;

//サイドビュー時のXの距離
const float CCamera::SIDEVIEW_LENGTH_X = 20.0f;

//サイドビュー時のYの距離
const float CCamera::SIDEVIEW_LENGTH_Y = 50.0f;

//サイドビュー時のZの距離
const float CCamera::SIDEVIEW_LENGTH_Z = 200.0f;

//サードビュー時の補正値
const float CCamera::THIRDVIEW_CORRECT_X = 20.0f;
const float CCamera::THIRDVIEW_CORRECT_Y = 105.0f;
const float CCamera::THIRDVIEW_CORRECT_Z = 20.0f;

//サードパーソンビュー時のXの最大可動域
const float CCamera::MAX_TURN_X = 0.5f;
//サードパーソンビュー時のXの最小可動域
const float CCamera::MIN_TURN_X = -0.15f;

//=============================================
//コンストラクタ
//=============================================
CCamera::CCamera():

m_fAngle(0.0f),
m_fLength(0.0f),
m_moveR({0.0f,0.0f,0.0f}),
m_moveV({0.0f,0.0f,0.0f}),
m_mtxProjection(),
m_mtxView(),
m_pCameraState(),
m_posR({0.0f,0.0f,0.0f}),
m_posV({0.0f,0.0f,0.0f}),
m_rot({0.0f,0.0,0.0f}),
m_rotmove({0.0f,0.0f,0.0f}),
m_vecU({0.0f,0.0f,0.0f})
{
}

//=============================================
//デストラクタ
//=============================================
CCamera::~CCamera()
{
}

//=============================================
//初期化
//=============================================
HRESULT CCamera::Init()
{
	if (m_pCameraState == nullptr)
	{
		m_pCameraState = new CThirdView;
	}
	m_posV = D3DXVECTOR3(0.0f, 200.0f, -180.0f); //視点
	m_posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f); //注視

	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f); //上方向ベクトル

	m_moveV = D3DXVECTOR3(0.0f, 0.0f, 0.0f); //視点移動量
	m_moveR = D3DXVECTOR3(0.0f, 0.0f, 0.0f); //注視点移動量

	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); //角度
	m_rotmove = D3DXVECTOR3(0.0f, 0.0f, 0.0f); //回転量

	D3DXVECTOR3 vecCamera = m_posR - m_posV;
	m_fLength = sqrtf(vecCamera.y * vecCamera.y + vecCamera.z * vecCamera.z);

	//対角線の角度を算出する
	m_fAngle = atan2f(vecCamera.y, vecCamera.z);
	return S_OK;
}

//=============================================
//終了
//=============================================
void CCamera::Uninit()
{
	if (m_pCameraState != nullptr)
	{
		delete m_pCameraState;
		m_pCameraState = nullptr;
	}
}

//=============================================
//更新
//=============================================
void CCamera::Update()
{
	//キーボード情報取得
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();

	if (m_pCameraState != nullptr)
	{
		m_pCameraState->ThirdView(this);
		m_pCameraState->FreeView(this);
		m_pCameraState->Ult(this);
	}

	//マウス情報取得
	CInputMouse* pMouse = CManager::GetInstance()->GetMouse();
	//現在のシーンを取得 TODO:シーン参照するな
	CScene::MODE pScene = CScene::GetSceneMode();
	if (pScene != CScene::MODE::MODE_TITLE)
	{
		m_rot.y += pMouse->GetMouseMove().x * 0.001f;
		m_rot.x += pMouse->GetMouseMove().y * 0.001f;
	}

	m_posV = m_posR + D3DXVECTOR3(-m_fLength * cosf(m_rot.x) * sinf(m_rot.y),
		m_fLength * sinf(m_rot.x),
		-m_fLength * cosf(m_rot.x) * cosf(m_rot.y));

	if (m_rot.x <= MIN_TURN_X)
	{
		m_rot.x = MIN_TURN_X;
	}
	if (m_rot.x >= MAX_TURN_X)
	{
		m_rot.x = MAX_TURN_X;
	}

	if (m_rot.y > D3DX_PI)
	{
		m_rot.y = -D3DX_PI;
		//		m_rot.y -= D3DX_PI* 2.0f;
	}

	if (m_rot.y < -D3DX_PI)
	{
		m_rot.y = D3DX_PI;
	}

	if (m_rot.x > D3DX_PI)
	{
		m_rot.x = -D3DX_PI;
		//		m_rot.y -= D3DX_PI* 2.0f;
	}

	if (m_rot.x < -D3DX_PI)
	{
		m_rot.x = D3DX_PI;
	}

	m_posV += m_moveV;
	m_posR += m_moveR;

	//移動量を更新(減速）
	m_moveV.x += (0.0f - m_moveV.x) * DAMPING_COEFFICIENT;
	m_moveV.y += (0.0f - m_moveV.y) * DAMPING_COEFFICIENT;
	m_moveV.z += (0.0f - m_moveV.z) * DAMPING_COEFFICIENT;

	m_moveR.x += (0.0f - m_moveR.x) * DAMPING_COEFFICIENT;
	m_moveR.y += (0.0f - m_moveR.y) * DAMPING_COEFFICIENT;
	m_moveR.z += (0.0f - m_moveR.z) * DAMPING_COEFFICIENT;

}

//=============================================
//カメラ設定
//=============================================
void CCamera::SetCamera()
{
	CRenderer* pRender = CManager::GetInstance()->GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetDevice();

	//プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	//プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(&m_mtxProjection,
		D3DXToRadian(45.0f),
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
		10.0f,
		3000.0f);

	//プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	//ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	D3DXVECTOR3 posV = m_posV;
	D3DXVECTOR3 posR = m_posR;

	posV.y += -15.0f;
	posR.y += -15.0f;

	//ビューマトリックスの作成
	D3DXMatrixLookAtLH(&m_mtxView,
		&posV,
		&posR,
		&m_vecU);

	//ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
}

//======================================
//カメラリセット
//======================================
void CCamera::ResetCamera()
{
	m_posV = D3DXVECTOR3(0.0f, 30.0f, -180.0f); //視点
	m_posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f); //注視

	m_vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f); //上方向ベクトル

	m_moveV = D3DXVECTOR3(0.0f, 0.0f, 0.0f); //視点移動量
	m_moveR = D3DXVECTOR3(0.0f, 0.0f, 0.0f); //注視点移動量

	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); //角度
	m_rotmove = D3DXVECTOR3(0.0f, 0.0f, 0.0f); //回転量

	D3DXVECTOR3 vecCamera = m_posR - m_posV;
	m_fLength = sqrtf(vecCamera.y * vecCamera.y + vecCamera.z * vecCamera.z);

	//対角線の角度を算出する
	m_fAngle = atan2f(vecCamera.y, vecCamera.z);

}

//=============================================
//カメラ移動量
//=============================================
void CCamera::InputMove()
{
	//キーボード情報取得
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();

	if (pKeyboard->GetPress(DIK_J) == true)
	{
		m_moveV.x -= sinf(D3DX_PI / 2 + m_rot.y) * DEFAULT_MOVE;
		m_moveR.x -= sinf(D3DX_PI / 2 + m_rot.y) * DEFAULT_MOVE;

		m_moveV.z -= cosf(D3DX_PI / 2 + m_rot.y) * DEFAULT_MOVE;
		m_moveR.z -= cosf(D3DX_PI / 2 + m_rot.y) * DEFAULT_MOVE;

	}

	if (pKeyboard->GetPress(DIK_L) == true)
	{
		m_moveV.x += sinf(D3DX_PI / 2 + m_rot.y) * DEFAULT_MOVE;
		m_moveR.x += sinf(D3DX_PI / 2 + m_rot.y) * DEFAULT_MOVE;

		m_moveV.z += cosf(D3DX_PI / 2 + m_rot.y) * DEFAULT_MOVE;
		m_moveR.z += cosf(D3DX_PI / 2 + m_rot.y) * DEFAULT_MOVE;

	}

	if (pKeyboard->GetPress(DIK_I) == true)
	{
		m_moveV.x -= sinf(D3DX_PI + m_rot.y) * DEFAULT_MOVE;
		m_moveR.x -= sinf(D3DX_PI + m_rot.y) * DEFAULT_MOVE;

		m_moveV.z -= cosf(D3DX_PI + m_rot.y) * DEFAULT_MOVE;
		m_moveR.z -= cosf(D3DX_PI + m_rot.y) * DEFAULT_MOVE;

	}

	if (pKeyboard->GetPress(DIK_K) == true)
	{
		m_moveV.x += sinf(D3DX_PI + m_rot.y) * DEFAULT_MOVE;
		m_moveR.x += sinf(D3DX_PI + m_rot.y) * DEFAULT_MOVE;

		m_moveV.z += cosf(D3DX_PI + m_rot.y) * DEFAULT_MOVE;
		m_moveR.z += cosf(D3DX_PI + m_rot.y) * DEFAULT_MOVE;
	}


	if (pKeyboard->GetPress(DIK_SPACE))
	{
		m_moveV.y += 1.0f;
		m_moveR.y += 1.0f;
	}

	if (pKeyboard->GetPress(DIK_LSHIFT))
	{
		m_moveV.y -= 1.0f;
		m_moveR.y -= 1.0f;
	}
}

//=============================================
//カメラの状態変更
//=============================================
void CCamera::ChangeCameraState(CCameraState* state)
{
	//今のステートを消し引数のステートに切り替える
	if (m_pCameraState != nullptr)
	{
		delete m_pCameraState;
		m_pCameraState = state;
	}
}

//=============================================
//カメラ回転量
//=============================================
void CCamera::CameraTurn()
{
	//キーボード情報取得
	CInputKeyboard* pKeyboard = CManager::GetInstance()->GetKeyboard();
	if (pKeyboard->GetPress(DIK_Q) == true)
	{
		m_rot.y -= 0.02f;

		m_posR.x = m_posV.x + sinf(m_rot.y) * m_fLength;
		m_posR.z = m_posV.z + cosf(m_rot.y) * m_fLength;

	}

	if (pKeyboard->GetPress(DIK_E) == true)
	{
		m_rot.y += 0.02f;
		m_posR.x = m_posV.x + sinf(m_rot.y) * m_fLength;
		m_posR.z = m_posV.z + cosf(m_rot.y) * m_fLength;
	}

	if (pKeyboard->GetPress(DIK_U) == true)
	{
		m_rot.y -= 0.02f;

		m_posV.x = m_posR.x - sinf(m_rot.y) * m_fLength;

		m_posV.z = m_posR.z - cosf(m_rot.y) * m_fLength;
	}

	if (pKeyboard->GetPress(DIK_O) == true)
	{
		m_rot.y += 0.02f;

		m_posV.x = m_posR.x - sinf(m_rot.y) * m_fLength;

		m_posV.z = m_posR.z - cosf(m_rot.y) * m_fLength;
	}
}

//=============================================
//サードパーソンビュー処理
//=============================================
void CCamera::ThirdViewCamera()
{
	for (int nCnt = 0; nCnt < CObject::MAX_OBJECT; nCnt++)
	{
		//オブジェクト取得
		CObject* pObj = CObject::Getobject(CPlayer::PLAYER_PRIORITY, nCnt);
		if (pObj != nullptr)
		{//ヌルポインタじゃなければ
			//タイプ取得
			CObject::OBJECT_TYPE type = pObj->GetType();
			if (type == CObject::OBJECT_TYPE::OBJECT_TYPE_PLAYER)
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObj);

				m_posR.x = pPlayer->GetPos().x;
				m_posR.y = pPlayer->GetPos().y + THIRDVIEW_CORRECT_Y;
				m_posR.z = pPlayer->GetPos().z;

				////カメラをプレイヤーの右にする処理:TODO
				//m_posR.x = pPlayer->GetPos().x + THIRDVIEW_CORRECT_X;
				//m_posR.y = pPlayer->GetPos().y + THIRDVIEW_CORRECT_Y;
				//m_posR.z = pPlayer->GetPos().z + THIRDVIEW_CORRECT_Z;

				m_posV = m_posR + D3DXVECTOR3(-m_fLength * cosf(m_rot.x) * sinf(m_rot.y),
					m_fLength * sinf(m_rot.x),
				-m_fLength * cosf(m_rot.x) * cosf(m_rot.y));

			}
		}
	}
}

//=============================================
//カメラのデバッグ表示
//=============================================
void CCamera::DebugCameraDraw()
{
#ifdef _DEBUG
	LPD3DXFONT pFont = CManager::GetInstance()->GetRenderer()->GetFont();
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	char aStr[256];

	sprintf(&aStr[0], "\n\n\n\n\n\n\n\n\n\n[camera]\nposR:%.1f,%.1f,%.1f\nposV:%.1f,%.1f,%.1f\nrot:%.1f,%.1f,%.1f"
		, m_posR.x, m_posR.y, m_posR.z, m_posV.x, m_posV.y, m_posV.z, m_rot.x, m_rot.y, m_rot.z);
	//テキストの描画
	pFont->DrawText(NULL, &aStr[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 0, 0, 255));
#endif // _DEBUG
}

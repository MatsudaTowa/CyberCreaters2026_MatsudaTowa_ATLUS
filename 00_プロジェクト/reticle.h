//=============================================
//
//レティクル[reticle.h]
//Auther Matsuda Towa
//
//=============================================
#include "main.h"
#include "billboard.h"
#include "crosshair.h"
#include"centerdot.h"
#include "hitmaker.h"

#ifndef _RETICLE_H_ //これが定義されてないとき

#define _RETICLE_H_

//=============================================
//レティクルクラス
//=============================================
class CReticle
{
public:
	CReticle();
	~CReticle();
	HRESULT Init();
	void Uninit();
	void Update();

	inline void SetPos(D3DXVECTOR3 pos)
	{
		m_ReticlePos = pos;
	}

	inline void SetSize(D3DXVECTOR3 size)
	{
		m_ReticleSize = size;
	}

	inline void SetCol(D3DXCOLOR col)
	{
		m_ReticleCol = col;
	}

	inline D3DXVECTOR3& GetReticlePos()
	{
		return m_ReticlePos;
	}

	inline D3DXVECTOR3& GetReticleSize()
	{
		return m_ReticleSize;
	}

	inline D3DXCOLOR& GetReticleCol()
	{
		return m_ReticleCol;
	}

	CCrossHair* m_pCrosshair;
	CCenterDot* m_pCenterDot;
	CHitMaker* m_pHitMaker;
private:

	D3DXVECTOR3 m_ReticlePos;
	D3DXVECTOR3 m_ReticleSize;
	D3DXCOLOR m_ReticleCol;
};

#endif
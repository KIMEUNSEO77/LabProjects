// Player.h


#pragma once

#include "GameObject.h"
#include "Camera.h"

// 플레이어 객체도 게임 객체이기 때문에 CGameObject 클래스를 상속받음
class CPlayer : public CGameObject
{
public:
	CPlayer() {}
	virtual ~CPlayer() { if (m_pCamera) delete m_pCamera; }

private:
	// 플레이어 객체에 포함된 카메라
	CCamera* m_pCamera = nullptr;

public:
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void Move(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void SetCamera(CCamera* pCamera) { m_pCamera = pCamera; }

	CCamera* GetCamera() { return(m_pCamera); }
};


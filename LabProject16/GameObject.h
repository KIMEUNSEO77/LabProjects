// GameObject.h
#pragma once

#include "Mesh.h"
#include "Camera.h"

class CShader;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
protected:
	XMFLOAT4X4 m_xmf4x4World;  // 월드 변환 행렬 (이 오브젝트를 어디에 놓을지)
	CMesh* m_pMesh = NULL;     // 이 오브젝트가 어떤 모양인지
	CShader* m_pShader = NULL; // 어떻게 그릴지
public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(CMesh* pMesh);
	virtual void SetShader(CShader* pShader);
	virtual void Animate(float fTimeElapsed);  // 시간에 따라 움직임 (이동, 회전, 신축 등)
	virtual void OnPrepareRender();            // 렌더 전 상태 준비
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera); // 실제 그릴지

	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle); // 오브젝트를 회전시키는 함수

	// 게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	// 게임 객체의 위치를 설정
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	// 게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	// 게임 객체를 회전(x-축, y-축, z-축)
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	// 상수 버퍼를 생성
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	// 상수 버퍼의 내용을 갱신
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	// 게임 객체가 카메라에 보인는 가를 검사
	bool IsVisible(CCamera* pCamera = NULL);
};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis =
			xmf3RotationAxis;
	}
	virtual void Animate(float fTimeElapsed);
};


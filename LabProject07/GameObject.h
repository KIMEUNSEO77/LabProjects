// GameObject.h
#pragma once

#include "Mesh.h"
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
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList); // 실제 그릴지

	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle); // 오브젝트를 회전시키는 함수
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


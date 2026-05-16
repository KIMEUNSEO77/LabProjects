// Camera.h
#pragma once

// 화면 종횡비
#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

// 카메라 상수 버퍼를 위한 구조체 선언 (셰이더(VS)에 카메라 정보를 넘기기 위한 데이터 묶음)
struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;  // 카메라 위치와 방향을 반영해 카메라 기준으로 세상을 다시 배치. (월드 좌표 -> 카메라 좌표)
	XMFLOAT4X4 m_xmf4x4Projection;  // 원근감 적용해 화면에 어떻게 보이게 할지 결정 (카메라 좌표 -> 화면 좌표)
};

class CCamera
{
protected:
	// 카메라 변환 행렬
	XMFLOAT4X4 m_xmf4x4View;

	// 투영 변환 행렬
	XMFLOAT4X4 m_xmf4x4Projection;

	// 뷰포트와 씨저 사각형
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

public:
	CCamera();
	virtual ~CCamera();
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float
		fAspectRatio, float fFOVAngle);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ =
		0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);
};


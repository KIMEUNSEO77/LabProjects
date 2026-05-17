// Scene.h
#pragma once

#include "Timer.h"
#include "Shader.h"

struct LIGHT
{
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT3 m_xmf3Position;
	float m_fFalloff;
	XMFLOAT3 m_xmf3Direction;
	float m_fTheta; // cos(m_fTheta)
	XMFLOAT3 m_xmf3Attenuation;
	float m_fPhi;   // cos(m_fPhi)
	bool m_bEnable;
	int m_nType;
	float m_fRange;
	float padding;
};

struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	XMFLOAT4 m_xmf4GlobalAmbient;
};

struct MATERIALS
{
	MATERIAL m_pReflections[MAX_MATERIALS];
};

class CScene
{
public:
	CScene();
	~CScene();

	// 씬에서 마우스와 키보드 메시지를 처리
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();

	// 그래픽 루트 시그너쳐를 생성
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

	//씬의 모든 조명과 재질을 생성
	void BuildLightsAndMaterials();
	//씬의 모든 조명과 재질을 위한 리소스를 생성하고 갱신
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	CPlayer* m_pPlayer = NULL;

protected:	
	// 배치(Batch) 처리를 하기 위해 씬을 셰이더들의 리스트로 표현
	CObjectsShader* m_pShaders = NULL;
	int m_nShaders = 0;

	// 씬의 조명
	LIGHTS* m_pLights = NULL;

	// 조명을 나탄는 리소스와 리소스에 대한 포인터
	ID3D12Resource* m_pd3dcbLights = NULL;
	LIGHTS* m_pcbMappedLights = NULL;

	//씬의 객체들에 적용되는 재질
	MATERIALS* m_pMaterials = NULL;

	//재질을 나타내는 리소스와 리소스에 대한 포인터
	ID3D12Resource* m_pd3dcbMaterials = NULL;
	MATERIAL* m_pcbMappedMaterials = NULL;



	// 렌더링 파이프라인의 "설정 정보"를 GPU에게 전달하는 중요한 객체들
	// 
	// 루트 시그너쳐를 나타내는 인터페이스 포인터
	// GPU에게 어떤 데이터를 전달할지에 대한 규칙 (슬롯에 어떻게 연결할지?)
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	// 파이프라인 상태를 나타내는 인터페이스 포인터
	// GPU 렌더링 파이프라인의 전체 설정을 담고 있음. (어떻게 그릴지?)
	//ID3D12PipelineState* m_pd3dPipelineState = NULL;
};


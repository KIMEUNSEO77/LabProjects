// Scene.h
#pragma once

#include "Timer.h"
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	//씬에서 마우스와 키보드 메시지를 처리한다.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
		lParam);
	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	void ReleaseObjects();
	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	void ReleaseUploadBuffers();

	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

protected:
	//씬은 셰이더들의 집합이다. 셰이더들은 게임 객체들의 집합이다.
	CShader** m_ppShaders = NULL;
	int m_nShaders = 0;


	// 렌더링 파이프라인의 "설정 정보"를 GPU에게 전달하는 중요한 객체들
	// 
	// 루트 시그너쳐를 나타내는 인터페이스 포인터
	// GPU에게 어떤 데이터를 전달할지에 대한 규칙 (슬롯에 어떻게 연결할지?)
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	// 파이프라인 상태를 나타내는 인터페이스 포인터
	// GPU 렌더링 파이프라인의 전체 설정을 담고 있음. (어떻게 그릴지?)
	ID3D12PipelineState* m_pd3dPipelineState = NULL;
};


// Scene.h
#pragma once

#include "Timer.h"

class CScene
{
public:
	CScene();
	~CScene();
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	void CreateGraphicsPipelineState(ID3D12Device* pd3dDevice);
	void BuildObjects(ID3D12Device* pd3dDevice);
	void ReleaseObjects();
	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void PrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	// 렌더링 파이프라인의 "설정 정보"를 GPU에게 전달하는 중요한 객체들
	// 
	// 루트 시그너쳐를 나타내는 인터페이스 포인터
	// GPU에게 어떤 데이터를 전달할지에 대한 규칙 (슬롯에 어떻게 연결할지?)
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	// 파이프라인 상태를 나타내는 인터페이스 포인터
	// GPU 렌더링 파이프라인의 전체 설정을 담고 있음. (어떻게 그릴지?)
	ID3D12PipelineState* m_pd3dPipelineState = NULL;
};


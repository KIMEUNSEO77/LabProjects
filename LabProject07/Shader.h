// Shader.h
#pragma once

#include "GameObject.h"
#include "Camera.h"

//게임 객체의 정보를 셰이더에게 넘겨주기 위한 구조체(상수 버퍼)이다.
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

// 셰이더 소스 코드를 컴파일하고 그래픽스 상태 객체를 생성
// hlsl 파일을 컴파일 하고, 그 셰이더를 실제 GPU 파이프라인에 연결해서 여러 오브젝트들을 관리하며 렌더링 하는 관리자 클래스
class CShader
{
public:
	CShader();
	virtual ~CShader();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();   // 정점 데이터 구조를 GPU에게 알려주는 함수
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(); // 레스터라이저 상태객체를 생성하는 함수(삼각형을 화면에 어떻게 그릴지)ex) 와이어 프레임 등
	virtual D3D12_BLEND_DESC CreateBlendState();           // 색을 섞는 방식
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(); // 깊이/스텐실 상태, 누가 앞에 있고 뒤에 있는지 판단하는 함수

	// 정점, 픽셀 셰이더 생성
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

	// 셰이더 소스 코드를 컴파일하는 함수
	D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob);

	// 셰이더를 이용해 PSO를 만드는 함수(모든 정보를 다 모아서 파이프라인 상태객체를 만듦)
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature);


	// 셰이더에 넘겨줄 데이터 관리를 위한 함수들(월드 행렬, 뷰 행렬, 투영 행렬 등)
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();

	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList,
		XMFLOAT4X4* pxmf4x4World);

	// 셰이더가 포함하는 게임 객체들을 렌더링하는 함수들
	virtual void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
protected:
	// 파이프라인 상태 객체들의 리스트(배열)
	// 셰이더와 각종 렌더 상태들을 묶어논 PSO 목록
	ID3D12PipelineState** m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;
};

// 어떤 입력 데이터를 받을지, 버텍스 셰이더를 쓸지, 픽셀 셰이더를 쓸지
// 어떻게 GPU 파이프라인을 구성할지
// 셰이더의 구체적인 구현 클래스
class CDiffusedShader : public CShader
{
public:
	CDiffusedShader();
	virtual ~CDiffusedShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature
		* pd3dGraphicsRootSignature);
};


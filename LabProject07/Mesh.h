// Mesh.h
#pragma once

// Mesh를 GPU에 올리고 렌더링하기 위한 클래스들을 선언

// GPU에 올려질 메쉬를 표현하기 위한 클래스를 선언
class CMesh
{
public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();
protected:
	ID3D12Resource* m_pd3dVertexBuffer = NULL;   // 정점 버퍼
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;  // CPU -> GPU 데이터 옮길 때 사용할 임시 버퍼
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;  // GPU에게 알려줄 버퍼 사용 설명서
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // 정점을 어떻게 연결할 것인지
	UINT m_nSlot = 0;  // 여러 버퍼 쓸 때 슬롯 번호
	UINT m_nVertices = 0;  // 정점 개수
	UINT m_nStride = 0;    // 정점 하나 크기(바이트 단위)
	UINT m_nOffset = 0;    // 버퍼 시작 위치
public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);  // GPU에 메쉬를 렌더링하는 함수
};

// 정점을 표현하기 위한 클래스를 선언 (한 점의 정보)
class CVertex
{
protected:
	// 정점의 위치 벡터(모든 정점은 최소한 위치 벡터를 가져야 한다)
	XMFLOAT3 m_xmf3Position;
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() {}
};

class CDiffusedVertex : public CVertex
{
protected:
	// 정점의 색상
	XMFLOAT4 m_xmf4Diffuse;
public:
	CDiffusedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
		m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = XMFLOAT3(x, y, z); 
		m_xmf4Diffuse = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = xmf3Position; 
		m_xmf4Diffuse = xmf4Diffuse;
	}
	~CDiffusedVertex() {}
};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CTriangleMesh() {}
};


#pragma once

class CMesh
{
};

class CPoint3D
{
public:
	CPoint3D() {}
	CPoint3D(float x, float y, float z) {this->x = x;
		this->y = y;
		this->z = z;
	}
	virtual ~CPoint3D() {}

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

class CVertex
{
public:
	CVertex() {}
	CVertex(float x, float y, float z) { m_f3Position = CPoint3D(x, y, z); }
	virtual ~CVertex() {}

	CPoint3D m_f3Position;
};

class CPolygon
{
public:
	CPolygon() {}
	CPolygon(int nVertices) {};
	virtual ~CPolygon() {}

	// 다각형(면)을 구성하는 정점들의 리스트
	int m_nVertices = 0;
	CVertex* m_pVertices = nullptr;

	void SetVertex(int nIndex, CVertex vertex);
};


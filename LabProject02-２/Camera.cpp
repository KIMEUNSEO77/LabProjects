// Camera.cpp


#include "stdafx.h"
#include "Camera.h"
#include "Player.h"

void CViewport::SetViewport(int nLeft, int nTop, int nWidth, int
	nHeight)
{
	m_nLeft = nLeft;
	m_nTop = nTop;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

/*카메라가 여러 번 회전을 하게 되면 누적된 실수 표현의 문제 때문에 카메라의 로컬 x-축(Right), y
축(Up), z-축(Look)이 서로 직교하지 않을 수 있다. 카메라의 로컬 x-축(Right), y-축(Up), z-축(Look)
이 서로 직교하도록 만들어준다.*/
void CCamera::GenerateViewMatrix()
{
	//카메라의 z-축을 기준으로 카메라의 좌표축들이 직교하도록 만든다.
	XMVECTOR xmvLook = XMVector3Normalize(XMLoadFloat3(&m_xmf3Look));
	XMVECTOR xmvUp = XMVector3Normalize(XMLoadFloat3(&m_xmf3Up));
	XMVECTOR xmvRight = XMVector3Normalize(XMVector3Cross(xmvUp,
		xmvLook));
	xmvUp = XMVector3Normalize(XMVector3Cross(xmvLook, xmvRight));
	XMStoreFloat3(&m_xmf3Look, xmvLook);
	XMStoreFloat3(&m_xmf3Right, xmvRight);
	XMStoreFloat3(&m_xmf3Up, xmvUp);
	//카메라 변환 행렬은 카메라 월드변환 행렬의 역행렬(전치행렬)이다.
	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x;
	m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y;
	m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z;
	m_xmf4x4View._33 = m_xmf3Look.z;
	XMVECTOR xmvPosition = XMLoadFloat3(&m_xmf3Position);
	m_xmf4x4View._41 = -XMVectorGetX(XMVector3Dot(xmvPosition,
		xmvRight));
	m_xmf4x4View._42 = -XMVectorGetX(XMVector3Dot(xmvPosition, xmvUp));
	m_xmf4x4View._43 = -XMVectorGetX(XMVector3Dot(xmvPosition,
		xmvLook));
	XMStoreFloat4x4(&m_xmf4x4ViewProject,
		XMMatrixMultiply(XMLoadFloat4x4(&m_xmf4x4View),
			XMLoadFloat4x4(&m_xmf4x4Project)));
}

//카메라가 xmf3LookAt을 바라보도록 카메라 변환 행렬을 갱신한다.
void CCamera::SetLookAt(XMFLOAT3& xmf3Position, XMFLOAT3& xmf3LookAt,
	XMFLOAT3& xmf3Up)
{
	m_xmf3Position = xmf3Position;
	XMStoreFloat4x4(&m_xmf4x4View,
		XMMatrixLookAtLH(XMLoadFloat3(&m_xmf3Position),
			XMLoadFloat3(&xmf3LookAt), XMLoadFloat3(&xmf3Up)));
	//카메라 변환 행렬에서 카메라의 x-축, y-축, z-축을 구한다.
	XMVECTORF32 xmf32vRight = { m_xmf4x4View._11, m_xmf4x4View._21,
	m_xmf4x4View._31, 0.0f };
	XMVECTORF32 xmf32vUp = { m_xmf4x4View._12, m_xmf4x4View._22,
	m_xmf4x4View._32, 0.0f };
	XMVECTORF32 xmf32vLook = { m_xmf4x4View._13, m_xmf4x4View._23,
	m_xmf4x4View._33, 0.0f };
	XMStoreFloat3(&m_xmf3Right, XMVector3Normalize(xmf32vRight));
	XMStoreFloat3(&m_xmf3Up, XMVector3Normalize(xmf32vUp));
	XMStoreFloat3(&m_xmf3Look, XMVector3Normalize(xmf32vLook));
}

void CCamera::SetLookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	//현재 카메라의 위치에서 플레이어를 바라보기 위한 카메라 변환 행렬을 생성한다.
	SetLookAt(m_xmf3Position, xmf3LookAt, xmf3Up);
}

void CCamera::SetViewport(int nLeft, int nTop, int nWidth, int nHeight)
{
	m_Viewport.SetViewport(nLeft, nTop, nWidth, nHeight);
	m_fAspectRatio = float(m_Viewport.m_nWidth) /
		float(m_Viewport.m_nHeight);
}

void CCamera::SetFOVAngle(float fFOVAngle)
{
	m_fFOVAngle = fFOVAngle;
	m_fProjectRectDistance = float(1.0f /
		tan(XMConvertToRadians(fFOVAngle * 0.5f)));
}

//원근 투영 변환 행렬을 생성한다.
void CCamera::GeneratePerspectiveProjectionMatrix(float
	fNearPlaneDistance, float fFarPlaneDistance, float fFOVAngle)
{
	float fAspectRatio = (float(m_Viewport.m_nWidth) /
		float(m_Viewport.m_nHeight));
	XMStoreFloat4x4(&m_xmf4x4Project,
		XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio,
			fNearPlaneDistance, fFarPlaneDistance));
}

//카메라를 이동한다.
void CCamera::Move(XMFLOAT3& xmf3Shift)
{
	XMStoreFloat3(&m_xmf3Position,
		XMVectorAdd(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&xmf3Shift)));
}

void CCamera::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z));
}

//카메라의 로컬 x-축(Right), y-축(Up), z-축(Look)을 기준으로 회전하는 함수이다.
void CCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
	if (fPitch != 0.0f)
	{
		XMMATRIX xmmtxRotate =
			XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right),
				XMConvertToRadians(fPitch));
		//카메라의 로컬 x-축(Right)을 중심으로 회전하는 행렬로 y-축(Up), z-축(Look)을 회전한다.
		XMStoreFloat3(&m_xmf3Look,
			XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), xmmtxRotate));
		XMStoreFloat3(&m_xmf3Up,
			XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), xmmtxRotate));
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX xmmtxRotate =
			XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up),
				XMConvertToRadians(fYaw));
		XMStoreFloat3(&m_xmf3Look,
			XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), xmmtxRotate));
		XMStoreFloat3(&m_xmf3Right,
			XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), xmmtxRotate));
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX xmmtxRotate =
			XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look),
				XMConvertToRadians(fRoll));
		XMStoreFloat3(&m_xmf3Up,
			XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), xmmtxRotate));
		XMStoreFloat3(&m_xmf3Right,
			XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), xmmtxRotate));
	}
}

void CCamera::Update(CPlayer* pPlayer, XMFLOAT3& xmf3LookAt, float
	fTimeElapsed)
{
	//플레이어의 로컬 x-축, y-축, z-축 벡터로부터 회전 행렬(플레이어와 같은 방향을 나타내는 행렬)을 
	생성한다.
		XMMATRIX xmmtx4Rotate;
	xmmtx4Rotate.r[0] = XMVectorSet(pPlayer->m_xmf3Right.x,
		pPlayer->m_xmf3Right.y, pPlayer->m_xmf3Right.z, 0.0f);
	xmmtx4Rotate.r[1] = XMVectorSet(pPlayer->m_xmf3Up.x,
		pPlayer->m_xmf3Up.y, pPlayer->m_xmf3Up.z, 0.0f);
	xmmtx4Rotate.r[2] = XMVectorSet(pPlayer->m_xmf3Look.x,
		pPlayer->m_xmf3Look.y, pPlayer->m_xmf3Look.z, 0.0f);
	xmmtx4Rotate.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR xmvPosition = XMLoadFloat3(&m_xmf3Position);
	//카메라 오프셋 벡터를 회전 행렬로 변환(회전)한다.
	XMVECTOR xmvOffset =
		XMVector3TransformCoord(XMLoadFloat3(&pPlayer->m_xmf3CameraOffset),
			xmmtx4Rotate);
	//회전한 카메라의 위치는 플레이어의 위치에 회전한 카메라 오프셋 벡터를 더한 것이다.
	XMVECTOR xmvNewPosition =
		XMVectorAdd(XMLoadFloat3(&pPlayer->m_xmf3Position), xmvOffset);
	//현재의 카메라의 위치에서 회전한 카메라의 위치까지의 방향과 거리를 나타내는 벡터이다.
	XMVECTOR xmvDirection = XMVectorSubtract(xmvNewPosition,
		xmvPosition);
	float fLength = XMVectorGetX(XMVector3Length(xmvDirection));
	xmvDirection = XMVector3Normalize(xmvDirection);

	/*카메라의 래그(Lag)는 플레이어가 회전하더라도 카메라가 동시에 따라서 회전하지 않고 약간의 시차를
두고 회전하는 효과를 구현하기 위한 것이다. 다음 그림에서 플레이어가 회전할 때 카메라는 c1에서 c2
로 시차를 두고 천천히(나누어서) 이동하면서 플레이어를 바라보도록 한다. 4.0을 40.0으로 바꾸어 회전
을 해보면 차이를 느낄 수 있을 것이다.*/
	float fTimeLagScale = fTimeElapsed * 4.0f;
	float fDistance = fLength * fTimeLagScale;
	if (fDistance > fLength) fDistance = fLength;
	if (fLength < 0.01f) fDistance = fLength;
	if (fDistance > 0)
	{
		//카메라를 공전하지 않고 이동을 한다(회전의 각도가 작은 경우 회전 이동은 선형 이동과 거의 같다).
		XMStoreFloat3(&m_xmf3Position, XMVectorAdd(xmvPosition,
			XMVectorScale(xmvDirection, fDistance)));
		//카메라가 플레이어를 바라보도록 한다.
		SetLookAt(pPlayer->m_xmf3Position, pPlayer->m_xmf3Up);
	}
}




// 월드 좌표계 점을 카메라 좌표계로 변환
CPoint3D CCamera::CameraTransform(CPoint3D& f3World)
{
	// 카메라를 월드 좌표계의 원점으로 이동
	CPoint3D f3Camera = f3World;
	f3Camera.x -= m_fxPosition;
	f3Camera.y -= m_fyPosition;
	f3Camera.z -= m_fzPosition;

	float fPitch = DegreeToRadian(-m_fxRotation);
	float fYaw = DegreeToRadian(-m_fyRotation);
	float fRoll = DegreeToRadian(-m_fzRotation);

	// 카메라를 월드 좌표계의 축과 일치하도록 회전
	CPoint3D f3Rotated = f3Camera;
	if (fPitch != 0.0f)
	{
		f3Rotated.y = float(f3Camera.y * cos(fPitch) - f3Camera.z * sin(fPitch));
		f3Rotated.z = float(f3Camera.y * sin(fPitch) + f3Camera.z * cos(fPitch));
		f3Camera.y = f3Rotated.y;
		f3Camera.z = f3Rotated.z;
	}
	if (fYaw != 0.0f)
	{
		f3Rotated.x = float(f3Camera.x * cos(fYaw) + f3Camera.z * sin(fYaw));
		f3Rotated.z = float(-f3Camera.x * sin(fYaw) + f3Camera.z * cos(fYaw));
		f3Camera.x = f3Rotated.x;
		f3Camera.z = f3Rotated.z;
	}
	if (fRoll != 0.0f)
	{
		f3Rotated.x = float(f3Camera.x * cos(fRoll) - f3Camera.y * sin(fRoll));
		f3Rotated.y = float(f3Camera.x * sin(fRoll) + f3Camera.y * cos(fRoll));
		f3Camera.x = f3Rotated.x;
		f3Camera.y = f3Rotated.y;
	}

	return(f3Camera);
}

// 카메라 좌표계 점을 투영 좌표계로 변환
CPoint3D CCamera::ProjectionTransform(CPoint3D& f3Camera)
{
	CPoint3D f3Project = f3Camera;

	if (f3Camera.z != 0.0f)
	{
		// 카메라의 시야각이 90°가 아닌 경우 투영 사각형까지의 거리를 곱함 (화면 비율을 맞추기 위해)
		// 원근 투영 공식 : x' = (x * d) / z, y' = (y * d) / z
		f3Project.x = float((f3Camera.x * m_fProjectRectDistance) / (m_fAspectRatio * f3Camera.z));
		f3Project.y = float((f3Camera.y * m_fProjectRectDistance) / f3Camera.z);

		// 투영 좌표계는 2차원이므로 z-좌표에 카메라 좌표계 z-좌표를 저장
		f3Project.z = f3Camera.z;
	}

	return(f3Project);
}

// 투영 좌표계 점을 화면 좌표계로 변환
CPoint3D CCamera::ScreenTransform(CPoint3D& f3Projection)
{
	CPoint3D f3Screen = f3Projection;

	float fHalfWidth = m_pViewport->m_nWidth * 0.5f;
	float fHalfHeight = m_pViewport->m_nHeight * 0.5f;

	f3Screen.x = (f3Projection.x * fHalfWidth) + m_pViewport->m_nLeft + fHalfWidth;
	f3Screen.y = (-f3Projection.y * fHalfHeight) + m_pViewport->m_nTop + fHalfHeight;

	return(f3Screen);
}

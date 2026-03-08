// GraphicsPipeline.cpp

#include "stdafx.h"
#include "GraphicsPipeline.h"


CGameObject* CGraphicsPipeline::m_pGameObject = NULL;
CCamera* CGraphicsPipeline::m_pCamera = NULL;

// 모델 좌표계의 점을 월드 변환, 카메라 변환, 원근 투영 변환을 순차적으로 수행
// 월드 변환은 게임 객체의 정보(위치, 방향) 필요
// 카메라 변환은 카메라의 정보(위치, 방향) 필요
// 원근 투영 변환은 카메라의 정보(시야각, 종횡비) 필요
CPoint3D CGraphicsPipeline::Project(CPoint3D& f3Model)
{
	CPoint3D f3World = m_pGameObject->WorldTransform(f3Model);
	CPoint3D f3Camera = m_pCamera->CameraTransform(f3World);
	CPoint3D f3Projection = m_pCamera->ProjectionTransform(f3Camera);
	return(f3Projection);
}

// 투영 좌표계의 점을 화면 변환을 수행
// 화면 변환은 카메라의 정보(뷰포트) 필요
CPoint3D CGraphicsPipeline::ScreenTransform(CPoint3D& f3Projection)
{
	CPoint3D f3Screen = m_pCamera->ScreenTransform(f3Projection);
	return(f3Screen);
}
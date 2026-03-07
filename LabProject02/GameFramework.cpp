// GameFramework.cpp

#include "stdafx.h"
#include "GameFramework.h"


CGameFramework::CGameFramework()
{
	m_pdxgiFactory = nullptr;
	m_pdxgiSwapChain = nullptr;
	m_pd3dDevice = nullptr;

	m_pd3dCommandAllocator = nullptr;
	m_pd3dCommandQueue = nullptr;
	m_pd3dPipeLineState = nullptr;
	m_pd3dCommandList = nullptr;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dSwapChainBackBuffers[i] = nullptr;
	m_pd3dRtvDescriptorHeap = nullptr;
	m_nRtvDescriptorIncrementSize = 0;

	m_pd3dDepthStencilBuffer = nullptr;
	m_pd3dDsvDescriptorHeap = nullptr;
	m_nDsvDescriptorIncrementSize = 0;

	m_nSwapChainBufferIndex = 0;

	m_hFenceEvent = nullptr;
	m_pd3dFence = nullptr;
	m_nFenceValue = 0;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;
}

CGameFramework::~CGameFramework()
{

}
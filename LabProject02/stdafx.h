// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// Direct3D와 관련 있는 헤더 파일들 추가
#include <string>      // 문자열 처리
#include <wrl.h>       // Microsoft의 Windows Runtime Library (WRL) 헤더 파일
#include <shellapi.h>  // Shell API 헤더 파일 (파일 열기 대화상자 등)

// Direct3D 11 필수 헤더 파일들
#include <d3d11.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>

// DirectXMath 라이브러리 헤더 파일 (3D 좌표 계산 이용)
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

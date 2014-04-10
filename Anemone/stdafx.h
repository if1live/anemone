﻿// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

//#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <ios>
#include <tchar.h>

#include "TransEngine.h"
#include "TextRenderer.h"
#include "TextProcess.h"

// GDIPlus
#include <GdiPlus.h>
#pragma comment(lib,"GdiPlus.lib")
using namespace Gdiplus;

// STL
#include <string>

// hWnd 모음
struct _hWnds
{
	HINSTANCE hInst;
	HWND Main;
	HWND Setting;
	HWND View;
	HWND Clip;
};
extern _hWnds hWnds;

// 클래스 모음
struct _Class
{
	CTextRenderer *TextRenderer;
	CTransEngine *TransEngine;
	CTextProcess *TextProcess;
};
extern _Class Cl;

struct render_attr
{
	FontFamily *Font;
	int Size_A;
	int Size_B;
	int Size_C;
	int Size_S;
};

static render_attr *rdName;
static render_attr *rdOriginal;
static render_attr *rdTrans;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
INT IsWritableMemory(LPVOID pMemoryAddr);
INT IsReadableMemory(LPVOID pMemoryAddr);
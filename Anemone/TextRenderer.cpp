﻿#include "StdAfx.h"
#include "TextRenderer.h"


CTextRenderer::CTextRenderer()
{
	szNameOrg = new std::wstring();
	szNameTrans = new std::wstring();
	szContextOrg = new std::wstring();
	szContextTrans = new std::wstring();

	*szContextOrg = L"테스트 문자열입니다 테스트 테스트 테스트!~!@$%^&*()_1234567890";
	*szContextTrans = L"테스트 문자열입니다 테스트 테스트 테스트!~!@$%^&*()_1234567890";
}

bool CTextRenderer::Init()
{
	GdiplusStartupInput gdiplusStartupInput;
	if (GdiplusStartup(&m_gpToken, &gdiplusStartupInput, NULL) != Status(Ok)) return false;
	return true;
}

int CTextRenderer::DrawText(Graphics *graphics, const wchar_t *contextText, wchar_t *fontName, int fntSize, int outlineInThick, int outlineOutThick, Color textColor, Color outlineInColor, Color outlineOutColor, Color shadowColor, bool textVisible, bool outlineInVisible, bool outlineOutVisible, bool shadowVisible, Rect *layoutRect)
{
	// 텍스트를 표시하지 않으면 리턴시킨다
	if (textVisible == false) return true;

	GraphicsPath path;
	GraphicsPath path_shadow;

	// 폰트 설정
	FontFamily fontFamily(fontName);
	if (!fontFamily.IsAvailable())
	{
		MessageBox(0, L"폰트 사용 불가능", 0, 0);
		return false;
	}

	StringFormat strformat = StringFormat::GenericTypographic();
	//strformat.SetFormatFlags(StringFormatFlagsMeasureTrailingSpaces);
	strformat.SetTrimming(StringTrimmingCharacter);
	
	Gdiplus::RectF boundRect;

	int outlineTotalThick = 0;
	if (outlineInVisible) outlineTotalThick += outlineInThick;
	if (outlineOutVisible) outlineTotalThick += outlineOutThick;

	Font font(&fontFamily, fntSize, FontStyleRegular);
	graphics->MeasureString(contextText, wcslen(contextText), &font, Gdiplus::RectF(layoutRect->X, layoutRect->Y, layoutRect->Width, layoutRect->Height), &strformat, &boundRect);
	SolidBrush tempBrush(outlineOutColor);
	graphics->DrawString(contextText, wcslen(contextText), &font, Gdiplus::RectF(layoutRect->X, layoutRect->Y, layoutRect->Width, layoutRect->Height), &strformat, &tempBrush);

	Gdiplus::REAL emSize = graphics->GetDpiY() * fntSize / 72;

	if (shadowVisible)
	{
		path_shadow.AddString(contextText, wcslen(contextText),
			&fontFamily, FontStyleRegular, emSize, Gdiplus::Rect(layoutRect->X + 4, layoutRect->Y + 4, layoutRect->Width, layoutRect->Height), &strformat);

		Pen pen_shadow(shadowColor, (Gdiplus::REAL)outlineTotalThick);
		pen_shadow.SetLineJoin(LineJoinRound);
		graphics->DrawPath(&pen_shadow, &path_shadow);
	}

	path.AddString(contextText, wcslen(contextText),
		&fontFamily, FontStyleRegular, emSize, Gdiplus::Rect(layoutRect->X, layoutRect->Y, layoutRect->Width, layoutRect->Height), &strformat);

	if (outlineOutVisible)
	{
		Pen penOut(outlineOutColor, outlineTotalThick);
		penOut.SetLineJoin(LineJoinRound);
		graphics->DrawPath(&penOut, &path);
	}

	if (outlineInVisible)
	{
		Pen pen(outlineInColor, outlineInThick);
		pen.SetLineJoin(LineJoinRound);
		graphics->DrawPath(&pen, &path);
	}

	SolidBrush brush(textColor);
	graphics->FillPath(&brush, &path);

	

	return boundRect.Height + 30;
}

bool CTextRenderer::Paint()
{
	PAINTSTRUCT ps;
	HDC hDC, memDC;
	HBITMAP hBM;
	RECT rect;

	hDC = BeginPaint(hWnds.Main, &ps);

	GetWindowRect(hWnds.Main, &rect);
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	memDC = CreateCompatibleDC(hDC);

	BYTE * pBits;
	BITMAPINFOHEADER bmih;

	// 해상도가 바뀌지 않았다면 비트맵을 다시 작성할 필요가 없음
	//if (cx != hBM_x || cy != hBM_y)
	{
		bmih.biSize = sizeof (BITMAPINFOHEADER);
		//bmih.biWidth = r1.right-r1.left;
		//bmih.biHeight = r1.bottom-r1.top;
		bmih.biWidth = cx;
		bmih.biHeight = cy;
		bmih.biPlanes = 1;
		bmih.biBitCount = 32; //
		bmih.biCompression = BI_RGB;
		bmih.biSizeImage = 0;
		bmih.biXPelsPerMeter = 0;
		bmih.biYPelsPerMeter = 0;
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;

		hBM = CreateDIBSection(NULL, (BITMAPINFO *)&bmih, 0, (VOID**)&pBits, NULL, 0);

		//hBM_x = cx;
		//hBM_y = cy;
	}

	SelectObject(memDC, hBM);

	using namespace Gdiplus;
	Graphics graphics(memDC);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	FontFamily fontFamily(L"맑은 고딕");
	StringFormat strformat = StringFormat::GenericTypographic();
	strformat.SetFormatFlags(StringFormatFlagsMeasureTrailingSpaces);
	//wchar_t pszbuf[] = L"테스트 문자열입니다 테스트 테스트 테스트!~!@$%^&*()_1234567890";

	int i_x = 10;
	int i_y = 10;

	int i_shadow_width = 3;
	int i_outline_width = 12;
	// 5 -> 3
	// 10 -> 5
	
	int pad_y = 20;

	pad_y = DrawText(&graphics, (*szContextOrg).c_str(), L"맑은 고딕", 25, 6, 6, Color(255, 255, 255, 255), Color(255, 255, 94, 0), Color(255, 255, 166, 80), Color(32, 0, 0, 0), true, true, true, true, &Gdiplus::Rect(20, pad_y, width - 40, height + 300));
	if (pad_y < height - 25)
		pad_y = DrawText(&graphics, (*szContextTrans).c_str(), L"맑은 고딕", 25, 6, 6, Color(255, 255, 255, 255), Color(255, 255, 0, 0), Color(251, 170, 170), Color(32, 0, 0, 0), true, true, true, true, &Gdiplus::Rect(20, pad_y, width - 40, height + 300));
	/*
	// DrawText(graphics, font, fntSize, outlineInThick, outlineOutThick, shadowThick, textVisible, outlineInVisible, outlineOutVisible, shadowVisible)
	//MessageBox(0, (*szContextOrg).c_str(), 0, 0);
	{
		GraphicsPath path;
		GraphicsPath path_shadow;
		//MessageBox(0, 0, 0, 0);
		path.AddString((*szContextOrg).c_str(), (*szContextOrg).length(),
			&fontFamily, FontStyleRegular, 36, Gdiplus::Rect(10, 10, width - 20, height - 20), &strformat);
		path_shadow.AddString((*szContextOrg).c_str(), (*szContextOrg).length(),
			&fontFamily, FontStyleRegular, 36, Gdiplus::Rect(14, 14, width - 20, height - 20), &strformat);
		
		Pen pen_shadow(Color(32, 0, 0, 0), (Gdiplus::REAL)i_shadow_width + i_outline_width - 5);
		pen_shadow.SetLineJoin(LineJoinRound);
		graphics.DrawPath(&pen_shadow, &path_shadow);
		SolidBrush brush_shadow(Color(32, 0, 0, 0));
		graphics.FillPath(&brush_shadow, &path);

		for (int i = 1; i < i_outline_width + 1; ++i)
		{
			Pen pen(Color(64, 0, 128, 192), (Gdiplus::REAL)i);
			pen.SetLineJoin(LineJoinRound);
			graphics.DrawPath(&pen, &path);

		}

		Pen pen(Color(128, 0, 128, 192), (Gdiplus::REAL)i_outline_width);
		pen.SetLineJoin(LineJoinRound);
		graphics.DrawPath(&pen, &path);

		SolidBrush brush(Color(255, 255, 255));
		graphics.FillPath(&brush, &path);
	}
	{
		GraphicsPath path;
		path.AddString((*szContextTrans).c_str(), (*szContextTrans).length(),
			&fontFamily, FontStyleRegular, 36, Gdiplus::Rect(10, 100, width - 20, height - 20), &strformat);
		Pen penOut(Color(32, 117, 81), 12);
		penOut.SetLineJoin(LineJoinRound);
		graphics.DrawPath(&penOut, &path);

		Pen pen(Color(234, 137, 6), 6);
		pen.SetLineJoin(LineJoinRound);
		graphics.DrawPath(&pen, &path);
		SolidBrush brush(Color(128, 0, 255));
		graphics.FillPath(&brush, &path);
		//SolidBrush brush(Color(255, 255, 255));
		//graphics.FillPath(&brush, &path);
	}
	*/
	int nBorderWidth = 5;
	Pen nBorderPen(Color(30, 0, 0, 0), (Gdiplus::REAL)nBorderWidth);

	graphics.DrawRectangle(&nBorderPen, Rect(1, 1, rect.right - rect.left - nBorderWidth - 1, rect.bottom - rect.top - nBorderWidth - 1));

	POINT dcOffset = { 0, 0 };
	SIZE size = { rect.right - rect.left, rect.bottom - rect.top };

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;

	UpdateLayeredWindow(hWnds.Main, hDC, 0, &size, memDC, &dcOffset, 0, &bf, ULW_ALPHA);

	EndPaint(hWnds.Main, &ps);

	ReleaseDC(NULL, hDC);
	ReleaseDC(NULL, memDC);

	DeleteObject(memDC);
	DeleteObject(hDC); 
	DeleteObject(hBM);
	return true;
}


CTextRenderer::~CTextRenderer()
{
	GdiplusShutdown(m_gpToken);

	delete szNameOrg;
	delete szNameTrans;
	delete szContextOrg;
	delete szContextTrans;
}

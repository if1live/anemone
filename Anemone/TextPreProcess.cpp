#include "stdafx.h"
#include "TextPreProcess.h"
#include <cassert>
#include <regex>
#include <array>
#include <functional>

static std::wstring PreProcessFunc_NameExistAndTextNotStartWithName(const std::wstring &text)
{
	// 일부 게임의 경우는 후킹된 텍스트의 중간에 이름이 섞여있다.
	// 【~】 가 문장 중간에 나오면 이을 문장의 처음으로 재배치
	// 【~】 는 이름을 표시하는데 사용되는 특수문자
	std::wregex nameExistButNotFirstRegex(L"^.+(【.+】).*$");
	std::wsmatch m;
	if (std::regex_match(text, m, nameExistButNotFirstRegex))
	{
		if (m.size() >= 2)
		{
			int pos = m.position(1);
			int len = m.length(1);
			std::wstring bodyHead = text.substr(0, pos);
			std::wstring name = text.substr(pos, len);
			std::wstring bodyRemain = text.substr(pos + len, text.length() - pos - len);
			return (name + bodyHead + bodyRemain);
		}
	}
	return text;
}

static std::wstring PreProcessFunc_RawNameOccurAfterText(const std::wstring &text) 
{
	// 「...」 뒤에 특수문자 없이 이름이 등장하면 순서 뒤집기
	// before: 「…えっと…」天音
	// after: 【天音】「…えっと…」
	std::wregex regex(L"^(「.+」)(.+)$");
	std::wsmatch m;
	if (std::regex_match(text, m, regex)) {
		if (m.size() >= 3) {
			int textPos = m.position(1);
			int textLen = m.length(1);
			int namePos = m.position(2);
			int nameLen = m.length(2);
			std::wstring charaText = text.substr(textPos, textLen);
			std::wstring charaName = text.substr(namePos, nameLen);
			std::wostringstream oss;
			oss << L"【";
			oss << charaName;
			oss << L"】";
			oss << charaText;
			return oss.str();
		}
	}
	return text;
}

CTextPreProcess::CTextPreProcess()
{

}

CTextPreProcess::~CTextPreProcess()
{

}

std::wstring CTextPreProcess::PreProcessText(const std::wstring &input)
{
	// Rule 0. 문장이 너무 짧으면 전처리기를 사용하지 않는다
	const int LOWER_BOUND_FOR_PRE_PROCESS = 2;
	if (input.empty() || input.length() <= LOWER_BOUND_FOR_PRE_PROCESS)
	{
		return input;
	}

	typedef std::function<std::wstring(std::wstring)> PreProcessFuncType;
	std::array<PreProcessFuncType, 2> funcs = {
		PreProcessFunc_NameExistAndTextNotStartWithName,
		PreProcessFunc_RawNameOccurAfterText,
	};

	std::wstring text = input;
	for (auto &func : funcs)
	{
		text = func(text);
	}

	return text;
}

static int testMainForTextPreProcess()
{
	// 전처리기를 간단하게 테스트
	// 아네모네 기존 코드에는 테스트와 관련된 코드가 없어서 적절히 끼워넣음
	CTextPreProcess subject;
	{
		auto input = L"「も【刑事】?…なんなんだよオマエは…」";
		auto expected = L"【刑事】「も?…なんなんだよオマエは…」";
		auto actual = subject.PreProcessText(input);
		assert(actual == expected);
	}
	{
		auto input = L"「も?…なんなんだよオマエは…」【刑事】";
		auto expected = L"【刑事】「も?…なんなんだよオマエは…」";
		auto actual = subject.PreProcessText(input);
		assert(actual == expected);
	}
	{
		auto input = L"「…えっと…」天音";
		auto expected = L"【天音】「…えっと…」";
		auto actual = subject.PreProcessText(input);
		assert(actual == expected);
	}
	{
		// 전처리가가 작업을 수행하지 않는 경우
		std::array<std::wstring, 3> inputTexts = {
			L"【刑事】「も?…なんなんだよオマエは…」",	// 일반 규격 문자열
			L"12",	// 너무 짧다
			L"",	// 비어있다 
		};
		for (auto &input : inputTexts)
		{
			auto actual = subject.PreProcessText(input);
			assert(actual == input);
		}
	}
	return 0;
}

#ifdef _DEBUG
static int __init_test_text_pre_process = testMainForTextPreProcess();
#endif



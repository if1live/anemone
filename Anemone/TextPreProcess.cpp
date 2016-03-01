// ?nicode please
#include "stdafx.h"
#include "TextPreProcess.h"
#include <cassert>
#include <regex>
#include <array>
#include <functional>
#include <set>

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

static std::wstring PreProcessFunc_SanitizeSpecialCharacter(const std::wstring &text)
{
	// ITH의 Link를 이용하면 2개의 후킹 결과물을 합칠수 있다
	// 예를 들어 쓰레드A에서 이름을 얻은 결과물이 "【?】▼", 쓰레드B에서 대사를 얻은 결과물이 「……」"
	// 둘을 합치면 【?】▼「……」가 된다. 이 경우 의도하지 않게 ▼가 들어갔다
	// 대사에 ▼가 직접 들어가진 않을거라고 가정하고 간단하게 구현
	std::set<std::wstring::value_type> invalidCharacters{ L'▼' };
	std::wostringstream oss;
	for (auto ch : text) {
		auto found = invalidCharacters.find(ch);
		if (found == invalidCharacters.end()) {
			oss << ch;
		}
	}
	return oss.str();
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
	std::array<PreProcessFuncType, 3> funcs = {
		PreProcessFunc_NameExistAndTextNotStartWithName,
		PreProcessFunc_RawNameOccurAfterText,
		PreProcessFunc_SanitizeSpecialCharacter
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

	struct TestCase {
		std::wstring input;
		std::wstring expected;
	};
	std::vector<TestCase> cases = {
		{ L"「も【刑事】?…なんなんだよオマエは…」", L"【刑事】「も?…なんなんだよオマエは…」" },
		{ L"「も?…なんなんだよオマエは…」【刑事】", L"【刑事】「も?…なんなんだよオマエは…」" },
		{ L"「…えっと…」天音", L"【天音】「…えっと…」" },
		{ L"【?】▼「……」", L"【?】「……」" },
		{ L"▼羊飼い……か。", L"羊飼い……か。" },
		// not changed
		{ L"【刑事】「も?…なんなんだよオマエは…」", L"【刑事】「も?…なんなんだよオマエは…」" },
		{ L"12", L"12" },
		{ L"", L"" },
	};
	for (auto &c : cases) {
		auto actual = subject.PreProcessText(c.input);
		assert(actual == c.expected);
	}
	return 0;
}

#ifdef _DEBUG
static int __init_test_text_pre_process = testMainForTextPreProcess();
#endif



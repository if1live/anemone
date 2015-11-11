#include "stdafx.h"
#include "TextPreProcess.h"
#include <cassert>
#include <regex>
#include <array>
#include <functional>

static std::wstring PreProcessFunc_NameExistAndTextNotStartWithName(const std::wstring &text)
{
	// �Ϻ� ������ ���� ��ŷ�� �ؽ�Ʈ�� �߰��� �̸��� �����ִ�.
	// ��~�� �� ���� �߰��� ������ ���� ������ ó������ ���ġ
	// ��~�� �� �̸��� ǥ���ϴµ� ���Ǵ� Ư������
	std::wregex nameExistButNotFirstRegex(L"^.+(��.+��).*$");
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
	// ��...�� �ڿ� Ư������ ���� �̸��� �����ϸ� ���� ������
	// before: �������êȡ�������
	// after: �����塽�������êȡ���
	std::wregex regex(L"^(��.+��)(.+)$");
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
			oss << L"��";
			oss << charaName;
			oss << L"��";
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
	// Rule 0. ������ �ʹ� ª���� ��ó���⸦ ������� �ʴ´�
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
	// ��ó���⸦ �����ϰ� �׽�Ʈ
	// �Ƴ׸�� ���� �ڵ忡�� �׽�Ʈ�� ���õ� �ڵ尡 ��� ������ ��������
	CTextPreProcess subject;
	{
		auto input = L"���⡼������?���ʪ�ʪ���諪�ޫ��ϡ���";
		auto expected = L"������������?���ʪ�ʪ���諪�ޫ��ϡ���";
		auto actual = subject.PreProcessText(input);
		assert(actual == expected);
	}
	{
		auto input = L"����?���ʪ�ʪ���諪�ޫ��ϡ�����������";
		auto expected = L"������������?���ʪ�ʪ���諪�ޫ��ϡ���";
		auto actual = subject.PreProcessText(input);
		assert(actual == expected);
	}
	{
		auto input = L"�������êȡ�������";
		auto expected = L"�����塽�������êȡ���";
		auto actual = subject.PreProcessText(input);
		assert(actual == expected);
	}
	{
		// ��ó������ �۾��� �������� �ʴ� ���
		std::array<std::wstring, 3> inputTexts = {
			L"������������?���ʪ�ʪ���諪�ޫ��ϡ���",	// �Ϲ� �԰� ���ڿ�
			L"12",	// �ʹ� ª��
			L"",	// ����ִ� 
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



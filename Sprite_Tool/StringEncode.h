#pragma once
#include "Window/WindowsHeaders.h"
#include <string>

static std::string WideToMultiU8(const std::wstring& src)
{
	// CP_ACP : ANSI �ڵ� ���̺�. CP_UTF8 : UTF-8 �ڵ� ���̺�.
	int len = WideCharToMultiByte(
		CP_UTF8, // �ڵ� ������(���� ���ڵ� ���̺�. ����, ���� ��ȣ �� ��Ÿ ���ڸ� ������ �� �ִ� ���� ����.)
		0,       // �÷���
		&src[0], // ���̵� ĳ����.
		-1,      // ���̵� ĳ���� ũ��. -1�� �Ѱ��ָ�, �� �� null ���ڱ��� ������ �Է� ���ڿ��� ó��. �׸��� null ���ڸ� ������ ���ڿ� ���̸� ����.
		NULL,    // ��Ƽ ����Ʈ ĳ����.
		0,       // ��Ƽ ����Ʈ ĳ���� ũ��.
		NULL,    // Ư�� �ڵ� �������� ǥ���� ���ϴ� ������ ��� ����ϱ� ���� ����Ʈ ���ڿ� ���� ������.
		NULL     // �÷��׸� ����Ű�� ������.
	);
	std::string dest(len, 0);
	WideCharToMultiByte(CP_UTF8, 0, &src[0], -1, &dest[0], len, NULL, NULL);
	return dest;
}
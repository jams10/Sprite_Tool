#pragma once
#include "Window/WindowsHeaders.h"
#include <string>

static std::string WideToMultiU8(const std::wstring& src)
{
	// CP_ACP : ANSI 코드 테이블. CP_UTF8 : UTF-8 코드 테이블.
	int len = WideCharToMultiByte(
		CP_UTF8, // 코드 페이지(문자 인코딩 테이블. 숫자, 문장 부호 및 기타 문자를 포함할 수 있는 문자 집합.)
		0,       // 플래그
		&src[0], // 와이드 캐릭터.
		-1,      // 와이드 캐릭터 크기. -1을 넘겨주면, 맨 끝 null 문자까지 포함해 입력 문자열을 처리. 그리고 null 문자를 포함한 문자열 길이를 리턴.
		NULL,    // 멀티 바이트 캐릭터.
		0,       // 멀티 바이트 캐릭터 크기.
		NULL,    // 특정 코드 페이지로 표현을 못하는 문자의 경우 사용하기 위한 디폴트 문자에 대한 포인터.
		NULL     // 플래그를 가리키는 포인터.
	);
	std::string dest(len, 0);
	WideCharToMultiByte(CP_UTF8, 0, &src[0], -1, &dest[0], len, NULL, NULL);
	return dest;
}
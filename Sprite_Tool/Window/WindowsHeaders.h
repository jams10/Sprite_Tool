#pragma once

// target Windows 7 or later
#define _WIN32_WINNT 0x0601 // �ڵ忡�� �����ϴ� �ּ� � ü�� ������ ������. 0x0601�� �������� 7
#include <sdkddkver.h>      // �������� ������ ���� ��ũ�ε��� ���ǵǾ� �ִ� ��� ����.

// Windows.h ��� ���Ͽ��� �츮�� �ۼ��� ���ø����̼ǿ��� ������� ���� API�� ���� ����ִµ�,
// �Ʒ��� ���� ���� ��ũ�� ���Ǹ� ���� Ư�� API���� ���ܽ��Ѽ� ������ �ð��� ����ų �� �ִ�.
#ifndef FULL_WIN_API       
#define WIN32_LEAN_AND_MEAN // ��ȣȭ, DDE, RPC, �� �� Windows ���ϰ� ���� API�� ����
#endif
#define NOMINMAX            // Windows.h�� min, max ��ũ�ΰ� ���ǵǾ� �ִµ�, �̴� std::min, max�� �浹�ϱ� ������ ��������.

#define STRICT              // ��ȣ�� ������ �� ������ �����ϰ� ����ϴ� �� �� �����ؾ� �ϴ� ����� ����ϵ��� ����.
                            // �ڵ� �ۼ��� �߰������� ���Ǹ� ����̰� �Ǿ� ����� �ð��� �����. Windows �ڵ忡 Ư�� ������.

#include <Windows.h>
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")
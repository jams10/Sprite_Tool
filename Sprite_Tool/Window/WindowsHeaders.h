#pragma once

// target Windows 7 or later
#define _WIN32_WINNT 0x0601 // 코드에서 지원하는 최소 운영 체제 버전을 지정함. 0x0601은 윈도우즈 7
#include <sdkddkver.h>      // 윈도우즈 버전에 대한 매크로들이 정의되어 있는 헤더 파일.

// Windows.h 헤더 파일에는 우리가 작성할 애플리케이션에서 사용하지 않을 API가 많이 들어있는데,
// 아래와 같이 여러 매크로 정의를 통해 특정 API들을 제외시켜서 컴파일 시간을 향상시킬 수 있다.
#ifndef FULL_WIN_API       
#define WIN32_LEAN_AND_MEAN // 암호화, DDE, RPC, 셸 및 Windows 소켓과 같은 API를 제외
#endif
#define NOMINMAX            // Windows.h에 min, max 매크로가 정의되어 있는데, 이는 std::min, max와 충돌하기 때문에 제외해줌.

#define STRICT              // 기호를 정의할 때 형식을 선언하고 사용하는 데 더 주의해야 하는 기능을 사용하도록 설정.
                            // 코드 작성시 추가적으로 주의를 기울이게 되어 디버깅 시간이 단축됨. Windows 코드에 특히 유용함.

#include <Windows.h>
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")
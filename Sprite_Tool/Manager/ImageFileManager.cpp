#include "ImageFileManager.h"
#include "../Sprite/SpriteSheet.h"
#include "../Window/Window.h"
#include "../SpriteAnimation.h"
#include <fstream>
#include <assert.h>

ImageFileManager::ImageFileManager()
{
}

ImageFileManager::~ImageFileManager()
{
}

SpriteSheet* ImageFileManager::GetSheetFromFile(Window& wnd, wchar_t* fileName)
{
	HRESULT hr = S_OK;

	// WIC 팩토리 생성
	IWICImagingFactory* wicFactory = NULL;
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,	// 우리가 만들어 줄 오브젝트의 클래스 아이디.
		NULL,
		CLSCTX_INPROC_SERVER,       // 오브젝트의 실행 컨텍스트 지정. 동일한 프로세스로 지정함.
		IID_IWICImagingFactory,     // 오브젝트와 상호작용할 인터페이스의 ID
		(LPVOID*)&wicFactory        // 4번째 매개변수로 넘긴 IID에 해당하는 인터페이스가 이곳으로 넘어옴.
	);
	assert(hr == S_OK);

	// 디코더 생성
	IWICBitmapDecoder* wicDecoder = NULL;
	hr = wicFactory->CreateDecoderFromFilename(
		fileName,						  // 파일 이름
		NULL,							  // 선호 decoder vendor
		GENERIC_READ,					  // 파일 접근을 읽기로 함.
		WICDecodeMetadataCacheOnLoad,     // decoder가 로드될 때 metadata를 캐시.
		&wicDecoder                       // decoder에 대한 포인터.
	);
	assert(hr == S_OK);

	// 이미지로 부터 한 프레임을 읽어옴.(gif 파일등은 여러 프레임으로 이루어져 있기 때문에 이런 방식을 차용.)
	// 하나의 이미지로 이루어진 일반 포맷들은 그냥 한 프레임만 읽어오면 됨.
	IWICBitmapFrameDecode* wicFrame = NULL;
	hr = wicDecoder->GetFrame(0, &wicFrame);
	assert(hr == S_OK);

	// WIC Bitmap -> ID2DBitmap 포맷으로 변환하기 위한 변환기(converter) 생성.
	IWICFormatConverter* wicConverter = NULL;
	hr = wicFactory->CreateFormatConverter(&wicConverter);
	assert(hr == S_OK);

	// 픽셀 당 32비트 압축 픽셀 BGRA 비트맵으로 변환해주기 위해 변환기를 세팅.
	hr = wicConverter->Initialize(
		wicFrame,						// 변환해줄 비트맵
		GUID_WICPixelFormat32bppPBGRA,  // 변환할 픽셀 형식. 32bit per pixel Packed BGRA : 4바이트 int에 압축되어 들어감. 0xAARRGGBB
		WICBitmapDitherTypeNone,        // 이미지 변환 시 사용할 dither 알고리즘.
		NULL,							// 변환 시 사용할 팔레트.
		0.0,						    // 변환 시 사용할 알파 임계값.
		WICBitmapPaletteTypeCustom      // 변환 시 사용할 팔레트 translation 타입.
	);
	assert(hr == S_OK);

	// 이미지 프레임으로부터 WIC 비트맵 생성.
	IWICBitmap* pIBitmap = NULL;
	hr = wicFactory->CreateBitmapFromSource(
		wicFrame,
		WICBitmapCacheOnDemand,
		&pIBitmap
	);
	assert(hr == S_OK);

	WICPixelFormatGUID uid;
	pIBitmap->GetPixelFormat(&uid);

	if (uid == GUID_WICPixelFormat32bppBGRA)
	{
	}
	if (uid == GUID_WICPixelFormat8bppIndexed)
	{
		assert(false);
		pIBitmap->Release();
		return nullptr;
	}

	// 이미지 프레임으로 부터 D2DBitmap 생성.
	ID2D1Bitmap* pD2DBitmap = NULL;
	hr = wnd.D2D().GetContext()->CreateBitmapFromWicBitmap(
		wicConverter,						// 변환기.
		NULL,								// D2D1_BITMAP_PROPERTIES
		&pD2DBitmap							// 최종 변환된 D2D1 비트맵.
	);
	assert(hr == S_OK);

	SpriteSheet* pSprite = new SpriteSheet(pIBitmap, pD2DBitmap);

	// 사용을 다 한 것들을 해제 해줌.
	//if (pD2DBitmap) pD2DBitmap->Release(); // 나중에 사용할 것이므로 Release는 SpriteSheet 쪽에서 해주어야 함.
	//if (pIBitmap) pIBitmap->Release();
	if (wicFrame) wicFrame->Release();
	if (wicConverter) wicConverter->Release();
	if (wicDecoder) wicDecoder->Release();
	if (wicFactory) wicFactory->Release();

	return pSprite;
}

void ImageFileManager::SaveAnimationFile(const SpriteAnimation& animation, wchar_t* fileName)
{
	std::wofstream animFile(fileName);
	animFile.imbue(std::locale("Korean"));

	// 애니메이션이 참조하는 파일 이름.
	animFile << animation.imageSourceName << '\n';
	// 애니메이션 이름.
	animFile << animation.animationName << '\n';
	// 애니메이션 안에 들어있는 프레임의 개수.
	animFile << animation.nFrames << '\n';
	// 컬러 키 값
	animFile << animation.colorKey << '\n';
	
	for (int i = 0; i < animation.nFrames; ++i)
	{
		// 프레임 기준점.
		animFile << animation.frames[i].originX << ' ' << animation.frames[i].originY << '\n';
		// 프레임 영역.
		animFile <<
			animation.frames[i].left << ' ' <<
			animation.frames[i].top << ' ' <<
			animation.frames[i].right << ' ' <<
			animation.frames[i].bottom << '\n';

		// 콜리젼 개수.
		animFile << animation.frames[i].nCollisions << '\n';
		// 콜리젼들 저장.
		for (int j = 0; j < animation.frames[i].nCollisions; ++j)
		{
			animFile << 
				animation.frames[i].collisions[j].left << ' ' <<
				animation.frames[i].collisions[j].top << ' ' <<
				animation.frames[i].collisions[j].right << ' ' <<
				animation.frames[i].collisions[j].bottom << '\n';
		}
	}

	animFile.close();
}



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

	// WIC ���丮 ����
	IWICImagingFactory* wicFactory = NULL;
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,	// �츮�� ����� �� ������Ʈ�� Ŭ���� ���̵�.
		NULL,
		CLSCTX_INPROC_SERVER,       // ������Ʈ�� ���� ���ؽ�Ʈ ����. ������ ���μ����� ������.
		IID_IWICImagingFactory,     // ������Ʈ�� ��ȣ�ۿ��� �������̽��� ID
		(LPVOID*)&wicFactory        // 4��° �Ű������� �ѱ� IID�� �ش��ϴ� �������̽��� �̰����� �Ѿ��.
	);
	assert(hr == S_OK);

	// ���ڴ� ����
	IWICBitmapDecoder* wicDecoder = NULL;
	hr = wicFactory->CreateDecoderFromFilename(
		fileName,						  // ���� �̸�
		NULL,							  // ��ȣ decoder vendor
		GENERIC_READ,					  // ���� ������ �б�� ��.
		WICDecodeMetadataCacheOnLoad,     // decoder�� �ε�� �� metadata�� ĳ��.
		&wicDecoder                       // decoder�� ���� ������.
	);
	assert(hr == S_OK);

	// �̹����� ���� �� �������� �о��.(gif ���ϵ��� ���� ���������� �̷���� �ֱ� ������ �̷� ����� ����.)
	// �ϳ��� �̹����� �̷���� �Ϲ� ���˵��� �׳� �� �����Ӹ� �о���� ��.
	IWICBitmapFrameDecode* wicFrame = NULL;
	hr = wicDecoder->GetFrame(0, &wicFrame);
	assert(hr == S_OK);

	// WIC Bitmap -> ID2DBitmap �������� ��ȯ�ϱ� ���� ��ȯ��(converter) ����.
	IWICFormatConverter* wicConverter = NULL;
	hr = wicFactory->CreateFormatConverter(&wicConverter);
	assert(hr == S_OK);

	// �ȼ� �� 32��Ʈ ���� �ȼ� BGRA ��Ʈ������ ��ȯ���ֱ� ���� ��ȯ�⸦ ����.
	hr = wicConverter->Initialize(
		wicFrame,						// ��ȯ���� ��Ʈ��
		GUID_WICPixelFormat32bppPBGRA,  // ��ȯ�� �ȼ� ����. 32bit per pixel Packed BGRA : 4����Ʈ int�� ����Ǿ� ��. 0xAARRGGBB
		WICBitmapDitherTypeNone,        // �̹��� ��ȯ �� ����� dither �˰���.
		NULL,							// ��ȯ �� ����� �ȷ�Ʈ.
		0.0,						    // ��ȯ �� ����� ���� �Ӱ谪.
		WICBitmapPaletteTypeCustom      // ��ȯ �� ����� �ȷ�Ʈ translation Ÿ��.
	);
	assert(hr == S_OK);

	// �̹��� ���������κ��� WIC ��Ʈ�� ����.
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

	// �̹��� ���������� ���� D2DBitmap ����.
	ID2D1Bitmap* pD2DBitmap = NULL;
	hr = wnd.D2D().GetContext()->CreateBitmapFromWicBitmap(
		wicConverter,						// ��ȯ��.
		NULL,								// D2D1_BITMAP_PROPERTIES
		&pD2DBitmap							// ���� ��ȯ�� D2D1 ��Ʈ��.
	);
	assert(hr == S_OK);

	SpriteSheet* pSprite = new SpriteSheet(pIBitmap, pD2DBitmap);

	// ����� �� �� �͵��� ���� ����.
	//if (pD2DBitmap) pD2DBitmap->Release(); // ���߿� ����� ���̹Ƿ� Release�� SpriteSheet �ʿ��� ���־�� ��.
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

	// �ִϸ��̼��� �����ϴ� ���� �̸�.
	animFile << animation.imageSourceName << '\n';
	// �ִϸ��̼� �̸�.
	animFile << animation.animationName << '\n';
	// �ִϸ��̼� �ȿ� ����ִ� �������� ����.
	animFile << animation.nFrames << '\n';
	// �÷� Ű ��
	animFile << animation.colorKey << '\n';
	
	for (int i = 0; i < animation.nFrames; ++i)
	{
		// ������ ������.
		animFile << animation.frames[i].originX << ' ' << animation.frames[i].originY << '\n';
		// ������ ����.
		animFile <<
			animation.frames[i].left << ' ' <<
			animation.frames[i].top << ' ' <<
			animation.frames[i].right << ' ' <<
			animation.frames[i].bottom << '\n';

		// �ݸ��� ����.
		animFile << animation.frames[i].nCollisions << '\n';
		// �ݸ����� ����.
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



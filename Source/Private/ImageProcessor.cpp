#include "../Public/ImageProcessor.h"

bool ImageProcessor::OnUserCreate()
{
	pOriginalSprite	= new olc::Sprite(RESSOURCE_PATH + "ile-des-morts.jpg");	
	pOriginalImage	= new olc::Decal(pOriginalSprite);
	pCurrentImage = pOriginalImage;

	//double randomkernel[9] = 
	kernel = Kernel();
	kernel.matrix = new double[9];

	kernel.matrix[0] =  0;
	kernel.matrix[1] =  - 1./4;
	kernel.matrix[2] =  0;
	kernel.matrix[3] =  - 1./4;
	kernel.matrix[4] =  2;
	kernel.matrix[5] =  - 1./4;
	kernel.matrix[6] =  0;
	kernel.matrix[7] =  - 1./4;
	kernel.matrix[8] =  0;

	//BasicFileOpen();
	return true;
}

bool ImageProcessor::OnUserUpdate(float fElapsedTime)
{
	float pixelSpeed = 125.f;
	if (GetKey(olc::ESCAPE).bReleased) return false;
	if (GetMouseWheel() > 0) iZoomLevel = std::min(128, iZoomLevel * 2);
	if (GetMouseWheel() < 0) iZoomLevel = std::max(1, iZoomLevel / 2);

	if (GetKey(olc::UP).bHeld) vSourcePos.y = std::max(0.f, vSourcePos.y - pixelSpeed * fElapsedTime);
	if (GetKey(olc::DOWN).bHeld) vSourcePos.y = std::min(float(pCurrentImage->sprite->Size().y), vSourcePos.y + pixelSpeed * fElapsedTime);
	if (GetKey(olc::LEFT).bHeld) vSourcePos.x = std::max(0.f, vSourcePos.x - pixelSpeed * fElapsedTime);
	if (GetKey(olc::RIGHT).bHeld) vSourcePos.x = std::min(float(pCurrentImage->sprite->Size().x), vSourcePos.x + pixelSpeed * fElapsedTime);

	if (GetKey(olc::ENTER).bReleased) Downscale(16,16,0,0); //Convolute();

	Clear(olc::BLACK);

	//Redraw viewports
	vPxMainViewportPos			= { int(MAIN_VIEWPORT.A.x * ScreenWidth()), int(MAIN_VIEWPORT.A.y * ScreenHeight()) };
	vPxMainViewportSize			= { int((MAIN_VIEWPORT.B.x - MAIN_VIEWPORT.A.x) * ScreenWidth()), int((MAIN_VIEWPORT.B.y - MAIN_VIEWPORT.A.y) * ScreenHeight()) };

	vPxKernelViewportPos		= { int(KERNEL_VIEWPORT.A.x * ScreenWidth()), int(KERNEL_VIEWPORT.A.y* ScreenHeight()) };
	vPxKernelViewportSize		= { int((KERNEL_VIEWPORT.B.x - KERNEL_VIEWPORT.A.x) * ScreenWidth()), int((KERNEL_VIEWPORT.B.y - KERNEL_VIEWPORT.A.y) * ScreenHeight()) };

	vPxImgSelectionViewportPos	= { int(IMAGE_SELECTION_VIEWPORT.A.x * ScreenWidth()), int(IMAGE_SELECTION_VIEWPORT.A.y * ScreenHeight())};
	vPxImgSelectionViewportSize	= { int((IMAGE_SELECTION_VIEWPORT.B.x - IMAGE_SELECTION_VIEWPORT.A.x) * ScreenWidth()),
									int((IMAGE_SELECTION_VIEWPORT.B.y - IMAGE_SELECTION_VIEWPORT.A.y) * ScreenHeight()) };

	olc::vd2d absMousePos = { GetMousePos().x / double(ScreenWidth()), GetMousePos().y / double(ScreenHeight()) };

	fShrinkToFitScale = pCurrentImage->sprite->Size().x / float((MAIN_VIEWPORT.B.x - MAIN_VIEWPORT.A.x) * ScreenWidth() - 2 /*margin*/);

	int iYCenter = MAIN_VIEWPORT.A.y * ScreenHeight() + int((MAIN_VIEWPORT.B.y - MAIN_VIEWPORT.A.y) * ScreenHeight() / 2 - 1);
	int iYOffset = pCurrentImage->sprite->Size().y / (2 * fShrinkToFitScale);
	olc::vi2d vImgPos = { vPxMainViewportPos.x + 1, iYCenter - iYOffset };

	//if (MAIN_VIEWPORT.IsInside(absMousePos))			FillRect(vPxMainViewportPos,		vPxMainViewportSize,		olc::DARK_BLUE);
	//if (KERNEL_VIEWPORT.IsInside(absMousePos))			FillRect(vPxKernelViewportPos,		vPxKernelViewportSize,		olc::DARK_BLUE);
	//if (IMAGE_SELECTION_VIEWPORT.IsInside(absMousePos))	FillRect(vPxImgSelectionViewportPos,vPxImgSelectionViewportSize,olc::DARK_BLUE);

	//Flat Img rendering
	DrawDecal(vImgPos, pCurrentImage, olc::vf2d(float(iZoomLevel), float(iZoomLevel)) / fShrinkToFitScale);
	//DrawSprite(vImgPos, pOriginalSprite, uint8_t(float(iZoomLevel) / fShrinkToFitScale));
	//Handle Zoom and paning
	
	//Mouse pos UV 
	olc::vf2d vViewportMouseUV = olc::vf2d((absMousePos.x - MAIN_VIEWPORT.A.x )/ (MAIN_VIEWPORT.B.x - MAIN_VIEWPORT.A.x),  (absMousePos.y - MAIN_VIEWPORT.A.y) /(MAIN_VIEWPORT.B.y - MAIN_VIEWPORT.A.y));
	vViewportMouseUV.x = std::min(1.f, std::max(0.f, vViewportMouseUV.x));
	vViewportMouseUV.y = std::min(1.f, std::max(0.f, vViewportMouseUV.y));

	olc::vf2d vImgMouseUV;

	DisplayKernel();
	
	//Zoom In Box
	//Project Mouse Coords in image

	//DrawPartialDecal(vImgPos, pCurrentImage, /*vImgPos + */vSourcePos, olc::vf2d(520, 520), olc::vf2d(float(iZoomLevel), float(iZoomLevel)) / fShrinkToFitScale);
	//DrawDecal(vImgPos, pCurrentImage, olc::vf2d(float(iZoomLevel), float(iZoomLevel)) / fShrinkToFitScale);
	DrawRect(vPxMainViewportPos			, vPxMainViewportSize);
	DrawRect(vPxKernelViewportPos		, vPxKernelViewportSize);
	DrawRect(vPxImgSelectionViewportPos	, vPxImgSelectionViewportSize);

	//DEBUG PRINTS
	DrawString({ 15,15 }, absMousePos.str());
	DrawString({ 15,25 }, std::to_string(iZoomLevel));
	DrawString({ 15,35 }, std::to_string(fShrinkToFitScale));
	DrawString({ 15, 45 }, vViewportMouseUV.str(), olc::RED);

	DrawLine(olc::vd2d((MAIN_VIEWPORT.A.x + vViewportMouseUV.x * (MAIN_VIEWPORT.B.x - MAIN_VIEWPORT.A.x)) * ScreenWidth(), MAIN_VIEWPORT.A.y * ScreenWidth()), olc::vd2d((MAIN_VIEWPORT.A.x + vViewportMouseUV.x * (MAIN_VIEWPORT.B.x - MAIN_VIEWPORT.A.x)) * ScreenWidth(), MAIN_VIEWPORT.B.y * ScreenWidth()), olc::RED);
	DrawLine(olc::vd2d(MAIN_VIEWPORT.A.x * ScreenWidth(), (MAIN_VIEWPORT.A.y + vViewportMouseUV.y * (MAIN_VIEWPORT.B.y - MAIN_VIEWPORT.A.y)) * ScreenWidth()), olc::vd2d(MAIN_VIEWPORT.B.x * ScreenWidth(), (MAIN_VIEWPORT.A.y + vViewportMouseUV.y * (MAIN_VIEWPORT.B.y - MAIN_VIEWPORT.A.y)) * ScreenWidth()), olc::RED);


	return true;
}

void ImageProcessor::DisplayKernel()
{
	olc::vi2d vKernelBoxUpLeft = { int(KERNEL_VIEWPORT.A.x * ScreenWidth()), int(KERNEL_VIEWPORT.A.y * ScreenHeight()) };
	olc::vi2d vSingleValueBoxSize = { int((KERNEL_VIEWPORT.B.x - KERNEL_VIEWPORT.A.x) * ScreenWidth() / kernel.uSize),  int((KERNEL_VIEWPORT.B.y - KERNEL_VIEWPORT.A.y) * ScreenHeight() / kernel.uSize) };
	
	size_t maxSize = 0;
	std::vector<std::string> vStrings(kernel.uSize * kernel.uSize, std::string("0"));

	for (uint8_t kx = 0; kx < kernel.uSize; kx++)
	{
		for(uint8_t ky = 0; ky < kernel.uSize; ky++)
		{
			std::stringstream strFinal;
			strFinal << std::setprecision(3) << kernel.matrix[ky * kernel.uSize + kx];
			maxSize = strFinal.str().size() >= maxSize ? strFinal.str().size() : maxSize;
			vStrings[kx + kernel.uSize * ky] = strFinal.str();
		}

	}

	for (uint8_t kx = 0; kx < kernel.uSize; kx++)
	{
		for (uint8_t ky = 0; ky < kernel.uSize; ky++)
		{
			
			std::string strTemp = vStrings[kx + kernel.uSize * ky];
			double xBoxOffset = 0.5 - ((strTemp.size() - 1) / 4.) * .4;
			DrawStringDecal(vKernelBoxUpLeft + olc::vi2d((kx + xBoxOffset) * vSingleValueBoxSize.x, (ky + 0.5) * vSingleValueBoxSize.y), strTemp, olc::WHITE);// , olc::vf2d{.5f, .5f});
		}
	}

	for (uint8_t lx = 1; lx < kernel.uSize; lx++)
	{
		DrawLine(olc::vi2d(int(KERNEL_VIEWPORT.A.x * ScreenWidth()) + lx * vSingleValueBoxSize.x, int(KERNEL_VIEWPORT.A.y * ScreenHeight())), olc::vi2d(int(KERNEL_VIEWPORT.A.x * ScreenWidth()) + lx * vSingleValueBoxSize.x, int(KERNEL_VIEWPORT.B.y * ScreenHeight())));
	}

	for (uint8_t ly = 1; ly < kernel.uSize; ly++)
	{
		DrawLine(olc::vi2d(int(KERNEL_VIEWPORT.A.x * ScreenWidth()), int(KERNEL_VIEWPORT.A.y * ScreenHeight()) + ly * vSingleValueBoxSize.y), olc::vi2d(int(KERNEL_VIEWPORT.B.x * ScreenWidth()), int(KERNEL_VIEWPORT.A.y * ScreenHeight()) + ly * vSingleValueBoxSize.y));
	}
}

void ImageProcessor::Downscale(int32_t uBlockSizeX, int32_t uBlockSizeY)
{
	int32_t blockSizeX = std::max(1, std::min(pOriginalImage->sprite->width, uBlockSizeX));
	int32_t blockSizeY = std::max(1, std::min(pOriginalImage->sprite->height, uBlockSizeY));

	olc::Sprite* newImageSprite = new olc::Sprite(pOriginalImage->sprite->width / blockSizeX, pOriginalImage->sprite->height / blockSizeY);

	for (int x = 0; x < newImageSprite->width; x++)
	{
		for (int y = 0; y < newImageSprite->height; y++)
		{
			//Block Sum
			double sum[3] = { 0., 0., 0. };
			for (int dx = 0; dx < blockSizeX; dx++)
				for (int dy = 0; dy < blockSizeY; dy++)
				{
					int X = x * blockSizeX + dx;
					int Y = y * blockSizeY + dy;

					sum[0] += pOriginalSprite->GetPixel(X, Y).r;
					sum[1] += pOriginalSprite->GetPixel(X, Y).g;
					sum[2] += pOriginalSprite->GetPixel(X, Y).b;
				}
			newImageSprite->SetPixel(x, y, olc::Pixel(sum[0] / (blockSizeY * blockSizeY), sum[1] / (blockSizeY * blockSizeY), sum[2] / (blockSizeY * blockSizeY)));
		}
	}

	pCurrentImage = new olc::Decal(newImageSprite);
}

void ImageProcessor::Downscale(int32_t uBlockSizeX, int32_t uBlockSizeY, int32_t uBlockOffsetX, int32_t uBlockOffsetY)
{
	int32_t blockSizeX = std::max(1, std::min(pOriginalImage->sprite->width, uBlockSizeX));
	int32_t blockSizeY = std::max(1, std::min(pOriginalImage->sprite->height, uBlockSizeY));

	int32_t blockOffsetX = std::max(0, std::min(blockSizeX - 1, uBlockOffsetX));
	int32_t blockOffsetY = std::max(0, std::min(blockSizeY -1, uBlockOffsetY));

	olc::Sprite* newImageSprite = new olc::Sprite(pOriginalImage->sprite->width / blockSizeX, pOriginalImage->sprite->height / blockSizeY);

	for (int x = 0; x < newImageSprite->width; x++)
	{
		for (int y = 0 ; y < newImageSprite->height; y++)
		{
			newImageSprite->SetPixel(x, y, pOriginalSprite->GetPixel(x * blockSizeX + blockOffsetX, y * blockSizeY + blockOffsetY));
		}
	}

	pCurrentImage = new olc::Decal(newImageSprite);
}

void ImageProcessor::Convolute()
{
	olc::Sprite* newImageSprite = pOriginalSprite->Duplicate();
	double dMax = 0.;
	bool bKernelDisplayed = false;
	//For now, only do clamping
	newImageSprite->SetSampleMode(olc::Sprite::NORMAL);
	for (int x = 0; x < newImageSprite->width; x++)
	{
		for (int y = 0; y < newImageSprite->height; y++)
		{
			double sum[3] = { 0, 0, 0 };
			for (int dx = 0; dx < kernel.uSize; dx++)
			{
				for (int dy = 0; dy < kernel.uSize; dy++)
				{
					int kx = x - kernel.uSize / 2 + dx;
					int ky = y - kernel.uSize / 2 + dy;
					sum[0] += pOriginalSprite->GetPixel(kx, ky).r * kernel.matrix[dy * kernel.uSize + dx];
					sum[1] += pOriginalSprite->GetPixel(kx, ky).g * kernel.matrix[dy * kernel.uSize + dx];
					sum[2] += pOriginalSprite->GetPixel(kx, ky).b * kernel.matrix[dy * kernel.uSize + dx];

					if (sum[2] > dMax) dMax = sum[2];
				}
			}
			
			//uint8_t greySum = uint8_t(sum[0] + sum[1] + sum[2] / 3.);
			newImageSprite->SetPixel(x, y, olc::Pixel(uint8_t(sum[0]), uint8_t(sum[1]), uint8_t(sum[2])));
		}
	}

	pCurrentImage = new olc::Decal(newImageSprite);

}

float ImageProcessor::FBM(const olc::vd2d& px, uint8_t uNumOctaves, float h, std::function<float(const olc::vi2d& px)> funcNoise)
{
	float G = exp2(-h);
	float f = 1.0;
	float a = 1.0;
	float t = 0.0;
	for (int i = 0; i < uNumOctaves; i++)
	{
		t += a * funcNoise(f * px);
		f *= 2.0;
		a *= G;
	}
	return t;
}

olc::vd2d ImageProcessor::hash(const olc::vd2d& px)
{
	const olc::vd2d k = olc::vd2d(0.3183099, 0.3678794);
	olc::vd2d x = px;
	x = x * k + olc::vd2d(k.y, k.x);

	olc::vd2d vTemp;
	std::modf(x.x * x.y * (x.x + x.y), &vTemp.x);
	std::modf(x.x * x.y * (x.x + x.y), &vTemp.y);

	std::modf(16.0 * k.x * vTemp.x, &vTemp.x);
	std::modf(16.0 * k.y * vTemp.y, &vTemp.y);

	return { -1.0 + 2.0 * k.x, -1.0 + 2.0 * k.y };
}

float ImageProcessor::GradientNoise2D(const olc::vd2d& px, olc::vd2d& outVDerivatives)
{
	olc::vd2d i(std::floor(px.x), std::floor(px.y));
	olc::vd2d f;
	std::modf(px.x, &f.x);
	std::modf(px.y, &f.y);

	olc::vd2d u = f * f * f * (f * (f * 6.0 - olc::vd2d(15.0, 15.0)) + olc::vd2d(10.0, 10.0));
	olc::vd2d du = 30.0 * f * f * (f * (f - olc::vd2d(2.0,2.0)) + olc::vd2d(1.0,1.0));

	olc::vd2d ga = hash(i + olc::vd2d(0.0, 0.0));
	olc::vd2d gb = hash(i + olc::vd2d(1.0, 0.0));
	olc::vd2d gc = hash(i + olc::vd2d(0.0, 1.0));
	olc::vd2d gd = hash(i + olc::vd2d(1.0, 1.0));

	float va = ga.dot(f - olc::vd2d(0.0, 0.0));
	float vb = gb.dot(f - olc::vd2d(1.0, 0.0));
	float vc = gc.dot(f - olc::vd2d(0.0, 1.0));
	float vd = gd.dot(f - olc::vd2d(1.0, 1.0));

	outVDerivatives = ga + u.x * (gb - ga) + u.y * (gc - ga) + u.x * u.y * (ga - gb - gc + gd) +
		du * (olc::vd2d(u.y, u.x) * (va - vb - vc + vd) + olc::vd2d(vb, vc) - olc::vd2d(va, va));

	float result = va + u.x * (vb - va) + u.y * (vc - va) + u.x * u.y * (va - vb - vc + vd);
	return result;
}



olc::vi2d ImageProcessor::ProjectMouseOnImage()
{

	return olc::vi2d();
}

bool Box::IsInside(const olc::vd2d& point) const
{

	return  ((point.x <= A.x && point.x >= B.x) || (point.x >= A.x && point.x <= B.x)) &&
			((point.y <= A.y && point.y >= B.y) || (point.y >= A.y && point.y <= B.y));
}


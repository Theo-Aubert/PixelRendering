#include "../Public/ImageProcessor.h"

bool ImageProcessor::OnUserCreate()
{
	pOriginalSprite	= new olc::Sprite(RESSOURCE_PATH + "ile-des-morts.jpg");	
	pOriginalImage	= new olc::Decal(pOriginalSprite);
	pCurrentImage = pOriginalImage;

	//double randomkernel[9] = 
	kernel = Kernel();
	kernel.matrix = new double[9];

	kernel.matrix[0] = 1./9;
	kernel.matrix[1] = 1./9;
	kernel.matrix[2] = 1./9;
	kernel.matrix[3] = 1./9;
	kernel.matrix[4] = 1./9;
	kernel.matrix[5] = 1./9;
	kernel.matrix[6] = 1./9;
	kernel.matrix[7] = 1./9;
	kernel.matrix[8] = 1./9;

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

	if (GetKey(olc::ENTER).bReleased) Convolute();

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

			if (!bKernelDisplayed)
			{
				std::cout << "Kernel \n";
				std::cout << kernel.matrix[0 * kernel.uSize + 0] << " ";
				std::cout << kernel.matrix[0 * kernel.uSize + 1] << " ";
				std::cout << kernel.matrix[0 * kernel.uSize + 2] << " ";
				std::cout << "\n";
				std::cout << kernel.matrix[1 * kernel.uSize + 0] << " ";
				std::cout << kernel.matrix[1 * kernel.uSize + 1] << " ";
				std::cout << kernel.matrix[1 * kernel.uSize + 2] << " ";
				std::cout << "\n";
				std::cout << kernel.matrix[2 * kernel.uSize + 0] << " ";
				std::cout << kernel.matrix[2 * kernel.uSize + 1] << " ";
				std::cout << kernel.matrix[2 * kernel.uSize + 2] << " ";
				std::cout << "\n";

				bKernelDisplayed = true;

			}
			
			uint8_t greySum = uint8_t(sum[0] + sum[1] + sum[2] / 3.);
			newImageSprite->SetPixel(x, y, olc::Pixel(uint8_t(sum[0]), uint8_t(sum[1]), uint8_t(sum[2])));
			//newImageSprite->SetPixel(x, y, olc::Pixel(greySum, greySum, greySum));
			//std::cout << std::to_string(uint8_t(sum[0])) << "|" << std::to_string(uint8_t(sum[1])) << "|" << std::to_string(uint8_t(sum[2])) <<"\n";
		}
	}

	std::cout << "Max value : " << dMax;
	pCurrentImage = new olc::Decal(newImageSprite);

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


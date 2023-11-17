#include "../Public/ImageProcessor.h"

bool ImageProcessor::OnUserCreate()
{
	pOriginalSprite	= new olc::Sprite(RESSOURCE_PATH + "elden-ring.jpg");	
	pOriginalImage	= new olc::Decal(pOriginalSprite);
	pCurrentImage = pOriginalImage;

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
	//Handle Zoom and paning
	

	//Zoom In Box
	//Project Mouse Coords in image

	DrawPartialDecal(vImgPos, pCurrentImage, /*vImgPos + */vSourcePos, olc::vf2d(520, 520), olc::vf2d(float(iZoomLevel), float(iZoomLevel)) / fShrinkToFitScale);
	//DrawDecal(vImgPos, pCurrentImage, olc::vf2d(float(iZoomLevel), float(iZoomLevel)) / fShrinkToFitScale);
	DrawRect(vPxMainViewportPos			, vPxMainViewportSize);
	DrawRect(vPxKernelViewportPos		, vPxKernelViewportSize);
	DrawRect(vPxImgSelectionViewportPos	, vPxImgSelectionViewportSize);

	DrawString({ 15,15 }, absMousePos.str());
	DrawString({ 15,25 }, std::to_string(iZoomLevel));
	DrawString({ 15,35 }, std::to_string(fShrinkToFitScale));


	return true;
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

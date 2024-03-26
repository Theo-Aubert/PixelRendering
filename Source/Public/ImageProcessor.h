#pragma once

#include "../Externals/olcPixelGameEngine.h"
#include "Constants.h"

//App Viewports dimensions
#define MAIN_VIEWPORT Box{{0.3, 0.01},{0.99,0.99}}
#define KERNEL_VIEWPORT Box{{0.01, 0.7},{0.29,0.99}}
#define IMAGE_SELECTION_VIEWPORT Box{{0.01, 0.01},{0.29,0.69}}

#define ZOOM_BOX_SIZE olc::vi2d(256,256)

struct Kernel
{
	uint8_t uSize = 3;
	double* matrix = nullptr;
};
struct Box
{

	olc::vd2d A;
	olc::vd2d B;

	//Check if the passed in vector is contained inside the box
	bool IsInside(const olc::vd2d& point) const;
};
// Override base class with your custom functionality
class ImageProcessor : public olc::PixelGameEngine
{
public:
	ImageProcessor()
	{
		// Name your application
		sAppName = "Image Processor";
	}

public:
	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

private:

	void EditKernelValue();

	void DisplayKernel();

	/*---------------------------*/
	/*	Processing utilities     */	
	/*---------------------------*/


	//Downscale using all pixels of a chunk to compute the new value
	void Downscale(int32_t uBlockSizeX, int32_t uBlockSizeY);

	//Downscale sampling one pixel for each chunk 
	void Downscale(int32_t uBlockSizeX, int32_t uBlockSizeY, int32_t uBlockOffsetX, int32_t uBlockOffsetY);

	void Convolute();

	float FBM(const olc::vd2d& px, uint8_t uNumOctaves, float h, std::function<float(const olc::vi2d& px)> funcNoise);

	olc::vd2d hash(const olc::vd2d& px);

	float GradientNoise2D(const olc::vd2d& px, olc::vd2d& outVDerivatives);

	olc::vi2d ProjectMouseOnImage();

    HANDLE BasicFileOpen()
    {
		OPENFILENAME ofn;       // common dialog box structure
		char szFile[260];       // buffer for file name
		HWND hwnd;              // owner window
		HANDLE hf;              // file handle

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL; // hwnd;
		ofn.lpstrFile = LPWSTR(szFile);
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = LPCWSTR("All\0*.*\0Text\0*.TXT\0");
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Display the Open dialog box. 

		if (GetOpenFileName(&ofn) == TRUE)
			hf = CreateFile(ofn.lpstrFile,
				GENERIC_READ,
				0,
				(LPSECURITY_ATTRIBUTES)NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				(HANDLE)NULL);
        return hf;
    }

	uint8_t iZoomLevel = 1;
	float fShrinkToFitScale = 1.f;
	olc::vf2d vUVOffset = { 0.f, 0.f };
	olc::vf2d vSourcePos = { 0.f, 0.f };
	olc::vi2d vPxMainViewportPos; //Up Left corner, pixel wise
	olc::vi2d vPxMainViewportSize; //Dimensions of the box, pixel wise

	olc::vi2d vPxKernelViewportPos; //Up Left corner, pixel wise
	olc::vi2d vPxKernelViewportSize; //Dimensions of the box, pixel wise

	olc::vi2d vPxImgSelectionViewportPos; //Up Left corner, pixel wise
	olc::vi2d vPxImgSelectionViewportSize; //Dimensions of the box, pixel wise

	std::vector<olc::Sprite*> vpImages; //Convoluted and filtered images
	olc::Sprite* pOriginalSprite;
	olc::Decal* pOriginalImage; //
	olc::Decal* pCurrentImage; //Displayed on main viewport

	//Convolution parameters;
	Kernel kernel;

};
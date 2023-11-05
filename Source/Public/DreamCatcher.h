#pragma once

#include "../Externals/olcPixelGameEngine.h"
#include "Constants.h"

class DreamCatcher :public olc::PixelGameEngine
{
public:

	class Trinket
	{

	public:

		Trinket() {};

		Trinket(Trinket* pParent);

		Trinket(std::shared_ptr<Trinket> pParent);


		~Trinket();

		void SetRenderer(olc::PixelGameEngine* pRenderer);

		olc::PixelGameEngine* GetRenderer() { return m_pRenderer; }

		virtual void AddChild(std::shared_ptr<Trinket> pChild);

		virtual void Update(float fElapsedTime);

		virtual void Draw(float fElapsedTime);

	public:

		olc::vd2d dPos;
	protected:



		std::shared_ptr<Trinket> pParent = nullptr;
		std::vector<std::shared_ptr<Trinket>> vChildren;
		olc::PixelGameEngine* m_pRenderer;

	private:
		
	};

	class Circle : public Trinket
	{
	public:

		virtual void Update(float fElapsedTime) override;

		virtual void Draw(float fElapsedTime) override;

		double dRadius;
		bool bFilled = false;
		olc::Pixel Color = olc::WHITE;
		double dAngularSpeed;

	protected:

		
		double dAngle;
	};

	DreamCatcher()
	{
		// Name your application
		sAppName = "Mezmerizing Dream Catcher";
	}

public:
	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

	std::shared_ptr<Trinket> pRoot;

	olc::vd2d center = { 0.5, 0.5 };
	olc::vd2d pos = center;
	double radius = 0.25;
	double angle = 0.0;
	double anuglarSpeed = PI / 16.;
};


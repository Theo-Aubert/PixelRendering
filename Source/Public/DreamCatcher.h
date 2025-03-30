#pragma once

#include "PixelApp.h"
#include "AppManager.h"
#include "Constants.h"

class DreamCatcher :public PixelApp
{
public:

	class Trinket
	{

	public:

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

		struct sCircleChild
		{
			double dAngle;
			double dRadiusOffset;
		};

		virtual void Update(float fElapsedTime) override;

		virtual void Draw(float fElapsedTime) override;

		virtual void AddChild(std::shared_ptr<Trinket> pChild) override;

		virtual void AddChild(std::shared_ptr<Trinket> pChild, double dAngle = 0.0, double dRadiusOffset = 0.0);

		double dRadius;
		bool bFilled = false;
		olc::Pixel Color = olc::WHITE;
		double dAngularSpeed;

	protected:

		std::map< std::shared_ptr<Trinket>, sCircleChild> mapChildren;
		
		double m_dAngle;
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
};
REGISTER_CLASS(DreamCatcher)


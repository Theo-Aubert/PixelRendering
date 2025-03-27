#pragma once

#include "PixelApp.h"
#include "AppManager.h"

enum ParticleType : uint32_t
{
	EMPTY,
	SAND,
	WATER,
	TRANSIENT
};

struct Particle
{
	ParticleType type = ParticleType::EMPTY;
	float lifeTime = 0.f;
	olc::vf2d velocity;
	olc::Pixel color = olc::BLACK;
	bool bUpdated = false;
};

// Override base class with your custom functionality
class SandSimulation : public PixelApp
{
public:
	SandSimulation()
	{
		// Name your application
		sAppName = "Sand Simulation";
	}

public:
	bool OnUserCreate() override;
	

	bool OnUserUpdate(float fElapsedTime) override;

private :

	bool IsEmpty(Particle** DataFrame, int x, int y);

	bool IsLiquid(Particle** DataFrame, int x, int y);

	olc::Pixel InitParticleColor(ParticleType particleType);

	bool UpdateTransient(int x, int y);

	bool UpdateSand(int x, int y);

	bool UpdateWater(int x, int y);

	bool ApplyBrush(Particle** DataFrame, int x, int y, uint8_t radius, ParticleType type);


	ParticleType currentSpawnParticle = ParticleType::TRANSIENT;
	Particle** state;
	Particle** output;

	uint8_t brushSize = 5;
};
REGISTER_CLASS(SandSimulation)


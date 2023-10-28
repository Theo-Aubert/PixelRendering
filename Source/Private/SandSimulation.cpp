#include "../Public/SandSimulation.h"

bool SandSimulation::OnUserCreate()
{

	// Called once at the start, so create things here

	output = new Particle*[int(ScreenWidth() * ScreenHeight())];
	state = new Particle*[int(ScreenWidth() * ScreenHeight())];

	memset(output, 0, int(ScreenWidth() * ScreenHeight()) * sizeof(Particle*));
	memset(state, 0, int(ScreenWidth() * ScreenHeight()) * sizeof(Particle*));

	for (int x = 0; x < ScreenWidth(); x++)
		for (int y = 0; y < ScreenHeight(); y++)
		{
			state[y * ScreenWidth() + x] = nullptr;
			output[y * ScreenWidth() + x] = nullptr;
		}

	return true;

}

bool SandSimulation::OnUserUpdate(float fElapsedTime)
{
	if (GetKey(olc::Key::SPACE).bHeld)
	{
		return true;
	}

	if (GetKey(olc::Key::C).bReleased)
	{
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
			{
				state[y * ScreenWidth() + x] = nullptr;
				output[y * ScreenWidth() + x] = nullptr;
			}
	}

	Clear(olc::BLACK);

	if (GetKey(olc::Key::E).bReleased)
	{
		currentSpawnParticle = EMPTY;
	}

	if (GetKey(olc::Key::R).bReleased)
	{
		currentSpawnParticle = TRANSIENT;
	}

	if (GetKey(olc::Key::S).bReleased)
	{
		currentSpawnParticle = SAND;
	}

	if (GetKey(olc::Key::W).bReleased)
	{
		currentSpawnParticle = WATER;
	}

	if (GetMouse(olc::Mouse::LEFT).bHeld)
	{
		switch (currentSpawnParticle)
		{
		case EMPTY :
			state[GetMouseY() * ScreenWidth() + GetMouseX()] = nullptr;
			break;
		case SAND :
		case WATER :
		case TRANSIENT:

			ApplyBrush(state, GetMouseX(), GetMouseY(), brushSize, currentSpawnParticle);

			/*if (!state[GetMouseY() * ScreenWidth() + GetMouseX()])
			{
				Particle* particle = new Particle();
				particle->type = currentSpawnParticle;
				particle->lifeTime = 10.f;
				particle->velocity = olc::vf2d();
				particle->color = InitParticleColor(currentSpawnParticle);
				particle->bUpdated = false;

				state[GetMouseY() * ScreenWidth() + GetMouseX()] = particle;
			}*/
			break;
		}
	}

	// Called once per frame, draws random coloured pixels
	for (int x = 0; x < ScreenWidth(); x++)
		for (int y = 0; y < ScreenHeight(); y++)
		{
			Particle* p = state[y * ScreenWidth() + x];
			if (p)
			{
				switch (p->type)

				{
				case ParticleType::SAND:
					UpdateSand(x,y);
					break;
				case ParticleType::WATER:
					UpdateWater(x, y);
					break;
				case ParticleType::TRANSIENT:
					UpdateTransient(x, y);
					break;
				case ParticleType::EMPTY:
				default:
					break;
				}
				Draw(x, y, p->color);
			}

		}

	memcpy(state, output, int(ScreenWidth() * ScreenHeight()) * sizeof(Particle*));

	//Clear output frame
	for (int x = 0; x < ScreenWidth(); x++)
		for (int y = 0; y < ScreenHeight(); y++)
		{
			output[y * ScreenWidth() + x] = nullptr;
		}
	return true;

}

bool SandSimulation::IsEmpty(Particle** DataFrame, int x, int y)
{
	bool bIsInFrame = (x >= 0 && x < ScreenWidth() && y >= 0 && y < ScreenHeight());
	return bIsInFrame && DataFrame[y * ScreenWidth() + x] == nullptr;
}

bool SandSimulation::IsLiquid(Particle** DataFrame, int x, int y)
{
	bool bIsInFrame = (x >= 0 && x < ScreenWidth() && y >= 0 && y < ScreenHeight());
	return bIsInFrame && DataFrame[y * ScreenWidth() + x] && DataFrame[y * ScreenWidth() + x]->type == WATER;
}

olc::Pixel SandSimulation::InitParticleColor(ParticleType particleType)
{
	olc::Pixel result = olc::BLACK;

	switch (particleType)
	{
	case EMPTY:
		break;
	case SAND:
		result = olc::Pixel(rand() % 156 + 100, rand() % 156 + 100, 0);
		break;
	case WATER:
		result = olc::Pixel(0, 0, 255, 100);
		break;
	case TRANSIENT:
		result = olc::DARK_GREY;
		break;
	default:
		break;
	}

	return result;
}

bool SandSimulation::UpdateTransient(int x, int y)
{
	output[y * ScreenWidth() + x] = state[y * ScreenWidth() + x];
	return true;
}

bool SandSimulation::UpdateSand(int x, int y)
{

	Particle* p = state[y * ScreenWidth() + x];

	if (IsEmpty(state, x, y + 1))
	{
		output[(y + 1) * ScreenWidth() + x] = p;
	}
	else if (IsLiquid(state, x, y + 1))
	{
		output[(y + 1) * ScreenWidth() + x] = p;

		if (IsLiquid(state,x, y-1 ))
			output[y * ScreenWidth() + x] = state[(y + 1) * ScreenWidth() + x];
		
	}
	else if(IsEmpty(state, x + 1, y + 1))
	{
		output[(y + 1) * ScreenWidth() + x + 1] = p;
	}
	else if (IsLiquid(state, x + 1, y + 1))
	{
		output[(y + 1) * ScreenWidth() + x + 1] = p;

		if (IsLiquid(state,x+1, y-1))
			output[y * ScreenWidth() + x] = state[(y + 1) * ScreenWidth() + x + 1];
		
	}
	else if (IsEmpty(state, x - 1, y + 1))
	{
		output[(y + 1) * ScreenWidth() + x -1] = p;	
	}
	else if (IsLiquid(state, x - 1, y + 1))
	{
		output[(y + 1) * ScreenWidth() + x - 1] = p;

		if (IsLiquid(state, x -1, y - 1))
			output[y * ScreenWidth() + x] = state[(y + 1) * ScreenWidth() + x - 1];
		
	}
	else
	{
		output[y * ScreenWidth() + x] = p;
	}

	return true;
}

bool SandSimulation::UpdateWater(int x, int y)
{

	Particle* p = state[y * ScreenWidth() + x];

	if (IsEmpty(state, x, y + 1) && IsEmpty(output, x, y + 1))
	{
		output[(y + 1) * ScreenWidth() + x] = p;
	}
	else if (IsEmpty(state, x + 1, y + 1) && IsEmpty(output, x + 1, y + 1))
	{
		output[(y + 1) * ScreenWidth() + x + 1] = p;
	}
	else if (IsEmpty(state, x - 1, y + 1) && IsEmpty(output, x - 1, y + 1))
	{
		output[(y + 1) * ScreenWidth() + x - 1] = p;
	}
	
	else if (p->velocity.x >= 0 && IsEmpty(state, x + 1, y) && IsEmpty(output, x + 1, y))
	{
		output[y * ScreenWidth() + x + 1] = p;
		p->velocity = olc::vf2d(1.f, 0);
	}
	else if (IsEmpty(state, x - 1, y) && IsEmpty(output, x - 1, y))
	{
		output[y * ScreenWidth() + x - 1] = p;
		p->velocity = olc::vf2d(- 1.f, 0);
	}
	else if(IsEmpty(output, x, y))
	{
		output[y * ScreenWidth() + x] = p;
	}

	return true;
}

bool SandSimulation::ApplyBrush(Particle** DataFrame, int x, int y, uint8_t radius, ParticleType type)
{
	// Thanks to IanM-Matrix1 #PR121
	if (radius < 0 || x < -radius || y < -radius || x - ScreenWidth() > radius || y - ScreenHeight() > radius)
		return false;

	if (radius > 0)
	{
		int x0 = 0;
		int y0 = radius;
		int d = 3 - 2 * radius;

		auto drawline = [&](int sx, int ex, int y)
		{
			for (int x = sx; x <= ex; x++)
			{
				if (!DataFrame[y * ScreenWidth() + x])
				{
					Particle* particle = new Particle();
					particle->type = type;
					particle->lifeTime = 10.f;
					particle->velocity = olc::vf2d();
					particle->color = InitParticleColor(type);
					particle->bUpdated = false;

					DataFrame[y * ScreenWidth() + x] = particle;
				}
			}
				
		};

		while (y0 >= x0)
		{
			drawline(x - y0, x + y0, y - x0);
			if (x0 > 0)	drawline(x - y0, x + y0, y + x0);

			if (d < 0)
				d += 4 * x0++ + 6;
			else
			{
				if (x0 != y0)
				{
					drawline(x - x0, x + x0, y - y0);
					drawline(x - x0, x + x0, y + y0);
				}
				d += 4 * (x0++ - y0--) + 10;
			}
		}
	}
	else
	{
		if (!DataFrame[y * ScreenWidth() + x])
		{
			Particle* particle = new Particle();
			particle->type = type;
			particle->lifeTime = 10.f;
			particle->velocity = olc::vf2d();
			particle->color = InitParticleColor(type);
			particle->bUpdated = false;

			DataFrame[y * ScreenWidth() + x] = particle;
		}
	}

	return true;
}

#include "..\Public\DreamCatcher.h"

void  DreamCatcher::Circle::Update(float fElapsedTime)
{
	for (auto& child : vChildren)
	{
		m_pRenderer->DrawCircle(child->dPos.x * m_pRenderer->GetDrawTargetWidth(), child->dPos.y * m_pRenderer->GetDrawTargetHeight(), 10, olc::YELLOW);
		child->dPos.x = dPos.x + cos(dAngle) * dRadius;
		child->dPos.y = dPos.y + sin(dAngle) * dRadius;
    	dAngle += dAngularSpeed * fElapsedTime;

		child->Update(fElapsedTime);
	}
}

void  DreamCatcher::Circle::Draw   (float fElapsedTime)
{
	Trinket::Draw(fElapsedTime);

	//DO STUFF
	if (bFilled)
	{
		GetRenderer()->FillCircle((int32_t)(dPos.x * GetRenderer()->GetDrawTargetWidth()), (int32_t)(dPos.y * GetRenderer()->GetDrawTargetHeight()), (int32_t)(dRadius * GetRenderer()->GetDrawTargetWidth()), Color);
	}
	else
	{
		GetRenderer()->DrawCircle((int32_t)(dPos.x * GetRenderer()->GetDrawTargetWidth()), (int32_t)(dPos.y * GetRenderer()->GetDrawTargetHeight()), (int32_t)(dRadius * GetRenderer()->GetDrawTargetWidth()), Color);
	}
}

DreamCatcher::Trinket::~Trinket()
{
	for (auto& child : vChildren)
	{
		child.reset();
	}
}

void DreamCatcher::Trinket::SetRenderer(olc::PixelGameEngine* pRenderer)
{
	m_pRenderer = pRenderer;
}

void DreamCatcher::Trinket::AddChild(std::shared_ptr<Trinket> pChild)
{
	pChild->SetRenderer(m_pRenderer);

	vChildren.push_back(pChild);
}

void DreamCatcher::Trinket::Update(float fElapsedTime)
{
	for (auto const& child : vChildren)
	{
 		child->Update(fElapsedTime);
	}
}

void DreamCatcher::Trinket::Draw(float fElapsedTime)
{

	for (auto const& child : vChildren)
	{


		child->Draw(fElapsedTime);
	}
}

bool DreamCatcher::OnUserCreate()
{
	pRoot = std::make_shared<Trinket>();

	pRoot->SetRenderer(this);

	std::shared_ptr<Circle> Circle0 = std::make_shared<Circle>();
	std::shared_ptr<Circle> Circle1 = std::make_shared<Circle>();

	Circle0->dPos = { 0.5, 0.5 };
	Circle0->dRadius = 0.25;
	Circle0->dAngularSpeed = PI / 16.;

	Circle1->dPos = { (Circle0->dPos.x + Circle0->dRadius), 0.5 };
	Circle1->dRadius = 0.1;
	Circle1->Color = olc::RED;

	pRoot->AddChild(Circle0);
	Circle0->AddChild(Circle1);

	

	return true;
}

bool DreamCatcher::OnUserUpdate(float fElapsedTime)
{

	/*if (!GetKey(olc::SPACE).bReleased)
	{
		return true;
	}*/

	Clear(olc::BLACK);

	pos.x = 0.5 + cos(angle) * radius;
	pos.y = 0.5 + sin(angle) * radius;

	DrawCircle((int32_t)(pos.x * GetDrawTargetWidth()), (int32_t)(pos.y * GetDrawTargetHeight()), (int32_t)(radius * GetDrawTargetWidth()), olc::GREEN);

	angle += anuglarSpeed * fElapsedTime;

	if (pRoot)
	{
		pRoot->Update(fElapsedTime);
		pRoot->Draw(fElapsedTime);
	}
	return true;
}

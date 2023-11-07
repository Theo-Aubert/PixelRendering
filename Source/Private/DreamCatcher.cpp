#include "..\Public\DreamCatcher.h"

void  DreamCatcher::Circle::Update(float fElapsedTime)
{
	if (dAngularSpeed != 0.0)
	{
		for (auto& child : vChildren)

		{
			child->dPos.x = dPos.x + cos(m_dAngle + mapChildren[child].dAngle) * (dRadius + mapChildren[child].dRadiusOffset);
			child->dPos.y = dPos.y + sin(m_dAngle + mapChildren[child].dAngle) * (dRadius + mapChildren[child].dRadiusOffset);
    		m_dAngle += dAngularSpeed * fElapsedTime;

			child->Update(fElapsedTime);
		}
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

void DreamCatcher::Circle::AddChild(std::shared_ptr<Trinket> pChild)
{
	Trinket::AddChild(pChild);
}

void DreamCatcher::Circle::AddChild(std::shared_ptr<Trinket> pChild, double dAngle, double dRadiusOffset)
{
	Trinket::AddChild(pChild);

	if (!pChild)
	{
		return;
	}


	if (!mapChildren.contains(pChild))
	{
		sCircleChild c(dAngle, dRadiusOffset);
		mapChildren.emplace(pChild, c);
	}
	else
	{
		mapChildren[pChild].dAngle			= dAngle;
		mapChildren[pChild].dRadiusOffset	= dRadiusOffset;

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
	if (pChild)
	{

		pChild->SetRenderer(m_pRenderer);

		vChildren.push_back(pChild);
	}
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

	std::shared_ptr<Circle> MainCircle		= std::make_shared<Circle>();
	std::shared_ptr<Circle> InnerLeftCircle = std::make_shared<Circle>();
	std::shared_ptr<Circle> OuterLeftCircle = std::make_shared<Circle>();
	std::shared_ptr<Circle> InnerRightCircle= std::make_shared<Circle>();
	std::shared_ptr<Circle> OuterRightCircle= std::make_shared<Circle>();
	std::shared_ptr<Circle> BlueBall		= std::make_shared<Circle>();
	std::shared_ptr<Circle> GreenBall		= std::make_shared<Circle>();


	MainCircle->dPos = { 0.5, 0.5 };
	MainCircle->dRadius = 0.25;
	MainCircle->dAngularSpeed = PI / 8.0;

	InnerLeftCircle->dPos = { (MainCircle->dPos.x + MainCircle->dRadius), 0.5 };
	InnerLeftCircle->dRadius = 0.1;
	InnerLeftCircle->dAngularSpeed = PI / 16.0;
	InnerLeftCircle->Color = olc::RED;

	OuterLeftCircle->dPos = { (MainCircle->dPos.x + MainCircle->dRadius), 0.5 };
	OuterLeftCircle->dRadius = 0.11;
	OuterLeftCircle->dAngularSpeed = PI / 16.0;
	OuterLeftCircle->Color = olc::RED;

	InnerRightCircle->dPos = { (MainCircle->dPos.x - MainCircle->dRadius), 0.5 };
	InnerRightCircle->dRadius = 0.1;
	InnerRightCircle->dAngularSpeed = - PI / 16.0;
	InnerRightCircle->Color = olc::RED;

	OuterRightCircle->dPos = { (MainCircle->dPos.x - MainCircle->dRadius), 0.5 };
	OuterRightCircle->dRadius = 0.11;
	OuterRightCircle->dAngularSpeed = - PI / 16.0;
	OuterRightCircle->Color = olc::RED;

	BlueBall->dRadius = 0.05;
	BlueBall->Color = olc::BLUE;

	GreenBall->dRadius = 0.05;
	GreenBall->Color = olc::GREEN;



	pRoot->AddChild(MainCircle);
	MainCircle->AddChild(InnerLeftCircle, 0.0);
	MainCircle->AddChild(OuterLeftCircle, 0.0);
	MainCircle->AddChild(InnerRightCircle, PI);
	MainCircle->AddChild(OuterRightCircle, PI);

	OuterLeftCircle->AddChild(BlueBall, 0.0);
	OuterRightCircle->AddChild(GreenBall, PI);

	SetDrawTarget(nullptr);
	

	return true;
}

bool DreamCatcher::OnUserUpdate(float fElapsedTime)
{

	/*if (!GetKey(olc::SPACE).bReleased)
	{
		return true;
	}*/

	Clear(olc::BLACK);

	if (pRoot)
	{
		pRoot->Update(fElapsedTime);
		pRoot->Draw(fElapsedTime);
	}
	return true;
}

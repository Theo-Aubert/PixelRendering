#include "../Public/ProceduralAnim.h"

void ProceduralEntity::ProceduralJoint::AddChild(ProceduralJoint* pChild)
{
	if(!pChild)
	{
		return;
	}

	if(pChild->GetParent() != this)
	{
		return;
	}

	m_arrChildrenJoints.push_back(pChild);
}

void ProceduralEntity::ProceduralJoint::MoveJoint(const olc::vf2d& vNewPos, const ProceduralJoint* pPropagator)
{
	m_vPosition = vNewPos;

	if (m_pParentJoint)
	{
		if (m_pParentJoint != pPropagator)
		{
			olc::vf2d vDir = m_pParentJoint->GetPosition() - m_vPosition;
			vDir = m_vPosition + vDir.norm() * m_fRadius;
			m_pParentJoint->MoveJoint(vDir, this);
		}

	}


	for (auto pChild : m_arrChildrenJoints)
	{
		if (pChild && pChild != pPropagator)
		{
			olc::vf2d vDir = pChild->GetPosition() - m_vPosition;
			vDir = m_vPosition + vDir.norm() * m_fRadius;
			pChild->MoveJoint(vDir, this);
		}
	}
}

ProceduralEntity::ProceduralEntity(olc::vf2d& HeadPos, std::vector<float>& joints)
{
	if(joints.size() == 0)
	{
		//create Head and stop here
		pHeadJoint = new ProceduralJoint(nullptr, HeadPos, 0.f);
	}

	else
	{
		//Create Head
		pHeadJoint = new ProceduralJoint(nullptr, HeadPos,joints[0]);

		if (!pHeadJoint)
		{
			return;
		}

		//Add chidlren
		ProceduralJoint* pParent = pHeadJoint;
		for (int i = 1; i < joints.size(); ++i)
		{
			ProceduralJoint* pChild = new ProceduralJoint(pParent, pParent->GetPosition() + olc::vf2d(pParent->GetRadius(), pParent->GetRadius()), joints[i]);

			if (!pChild)
			{
				return;
			}

			pParent->AddChild(pChild);
			pParent = pChild;
		}
	}
}

void ProceduralEntity::Move(const olc::vf2d& vNewPos)
{
	if (!pHeadJoint)
	{
		return;
	}

	pHeadJoint->MoveJoint(vNewPos, nullptr);

}

bool ProceduralAnim::OnUserCreate()
{
	olc::vf2d vCenterScreen = GetScreenSize() / 2;
	std::vector<float> arrJointRadius = { 16.f, 16.f, 24.f, 45.f, 40.f, 15.f, 15.f, 10.f, 2.f };
	pMainEntity = new ProceduralEntity(vCenterScreen, arrJointRadius);
	return pMainEntity;
}

bool ProceduralAnim::OnUserUpdate(float fElapsedTime)
{
	Clear(olc::BLACK);

	if (GetMouse(olc::Mouse::LEFT).bHeld)
	{
		vMouseCommand = GetMousePos();
		bHasReceivedCommand = true;
	}

	if(bHasReceivedCommand && (pMainEntity->GetHead()->GetPosition() - vMouseCommand).mag() > 1 )
	{
		olc::vf2d vTarget = vMouseCommand - pMainEntity->GetHead()->GetPosition();
		vTarget = vTarget.norm() * fSpeed * fElapsedTime;
		vTarget = pMainEntity->GetHead()->GetPosition() + vTarget;
		pMainEntity->Move(vTarget);
	}

	DrawProceduralEntity(pMainEntity);

	return true;
}

void ProceduralAnim::DrawProceduralEntity(ProceduralEntity* pEntity)
{
	if (pEntity)
	{
		DrawCircle(pEntity->GetHead()->GetPosition(), pEntity->GetHead()->GetRadius());
		DrawProceduralJoint(pEntity->GetHead());
	}
}

void ProceduralAnim::DrawProceduralJoint(ProceduralEntity::ProceduralJoint* pJoint)
{
	if (!pJoint)
	{
		return;
	}

	for (ProceduralEntity::ProceduralJoint* pChild : pJoint->GetChildren())
	{
		DrawLine(pJoint->GetPosition(), pChild->GetPosition());
		DrawCircle(pChild->GetPosition(), 1);
		DrawCircle(pChild->GetPosition(), pChild->GetRadius());
		DrawProceduralJoint(pChild);
	}
}

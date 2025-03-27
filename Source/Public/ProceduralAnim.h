#include "PixelApp.h"
#include "AppManager.h"
#include "Constants.h"
#include <deque>

#pragma once

class ProceduralEntity
{
public:

	class ProceduralJoint
	{
	public:

		ProceduralJoint() {};

		ProceduralJoint(ProceduralJoint* pParent, olc::vf2d position, float radius)
		{
			m_pParentJoint = pParent;
			m_vPosition = position;
			m_fRadius = radius;
		}

		~ProceduralJoint()
		{
			delete(m_pParentJoint);
			m_pParentJoint = nullptr;
		}

		void AddChild(ProceduralJoint* pChild);

		void MoveJoint(const olc::vf2d& vNewPos, const ProceduralJoint* pPropagator);

		inline olc::vf2d GetPosition() { return m_vPosition; };
		inline float GetRadius() { return m_fRadius; };
		inline const ProceduralJoint* GetParent() { return m_pParentJoint; };
		inline const std::vector<ProceduralJoint*> GetChildren() { return m_arrChildrenJoints; };

	private:
		ProceduralJoint* m_pParentJoint = nullptr;

		std::vector<ProceduralJoint*> m_arrChildrenJoints;

		olc::vf2d m_vPosition;
		float m_fRadius = 0.f;
	};



	ProceduralEntity(olc::vf2d& HeadPos, std::vector<float>& joints);

	void Move(const olc::vf2d& vNewPos);

	ProceduralJoint* GetHead() { return pHeadJoint; };

private:

	ProceduralJoint* pHeadJoint = nullptr;


};
class ProceduralAnim : public PixelApp
{

public:

	ProceduralAnim()
	{
		sAppName = "ProceduralAnim";
	}

	//--- BEGIN GameEngine App interface
	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;
	//---END GameEngine app interface

private:

	void DrawProceduralEntity(ProceduralEntity* pEntity);

	void DrawProceduralJoint(ProceduralEntity::ProceduralJoint* pJoint);

	ProceduralEntity* pMainEntity = nullptr;

	olc::vi2d vMouseCommand;
	float fSpeed = 150.f;
	bool bHasReceivedCommand = false;
};
REGISTER_CLASS(ProceduralAnim)
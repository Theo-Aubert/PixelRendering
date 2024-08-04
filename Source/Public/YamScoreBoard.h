#pragma once

#include "../Externals/olcPixelGameEngine.h"
#include "Constants.h"
#include <map>
#include <set>
#include <memory>

enum EValues : uint8_t
{
	As,
	Deux,
	Trois,
	Quatre,
	Cinq,
	Six,
	Brelan,
	Petite_Suite,
	Grande_Suite,
	Full,
	Carre,
	Yams,
	Name
};

static std::map<EValues, std::string> mapScoreLiterals
{
	std::make_pair(As,			"As"),
	std::make_pair(Deux,		"Deux"),
	std::make_pair(Trois,		"Trois"),
	std::make_pair(Quatre,		"Quatre"),
	std::make_pair(Cinq,		"Cinq"),
	std::make_pair(Six,			"Six"),
	std::make_pair(Brelan,		"Brelan"),
	std::make_pair(Petite_Suite,"Petite Suite"),
	std::make_pair(Grande_Suite,"Grande Suite"),
	std::make_pair(Full,		"Full"),
	std::make_pair(Carre,		"Carre"),
	std::make_pair(Yams,		"YAM's"),

};

static std::string strST1	= "Sous-Total 1";
static std::string strBonus = "Bonus (>63)";
static std::string strST2	= "Sous-Total 2";
static std::string strTT	= "TOTAL";


class Player
{

public:

	void InitScoreState();

	inline std::map <EValues, std::pair<bool, uint16_t>>& GetScoreState() { return m_mapScoreSate;  }

	inline std::string GetName() { return strName; }
	inline void SetName(const std::string& strNewName) { strName = strNewName;  }

	inline void SetAnchor(olc::vi2d& vAnchor) { m_vAnchor = vAnchor; }
private :

	std::string strName = "NewPlayer";
	std::map <EValues, std::pair<bool, uint16_t>> m_mapScoreSate;

	olc::vi2d m_vAnchor;
};


#define ScoreValueWidth 220
#define ScoreValueHeight 40
#define LiteralOffset 10
#define LiteralSize 2

// Override base class with your custom functionality
class YamScoreBoard : public olc::PixelGameEngine
{
	

public:

	YamScoreBoard()
	{
		// Name your application
		sAppName = "Yam's Score Board";
	}
	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

	virtual void OnTextEntryComplete(const std::string& strText) override;

private:

	void DrawScoreColumn(olc::vi2d& vPos);

	void DrawPlayerColumn(std::shared_ptr<Player> pPlayer, olc::vi2d& vPos);

	uint16_t ComputeST1(std::shared_ptr<Player> pPlayer);
	bool HasBonus(std::shared_ptr<Player> pPlayer);
	uint16_t ComputeST2(std::shared_ptr<Player> pPlayer);
	uint16_t ComputeTT(std::shared_ptr<Player> pPlayer);

	uint8_t m_iNumPlayers = 3;
	std::vector<std::shared_ptr<Player>> m_arrPlayers;

	bool bEntryMode = false;
	olc::vi2d vEntryAnchor;

	std::shared_ptr<Player> m_pCurrentWritingPlayer = nullptr;
	EValues eCurrentWritingValue;
};
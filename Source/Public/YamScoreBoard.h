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

	inline std::map <EValues, std::pair<bool, uint8_t>>& GetScoreState() { return m_mapScoreSate;  }

	inline std::string GetName() { return strName; }
	inline void SetName(const std::string& strNewName) { strName = strNewName;  }

	inline void SetAnchor(olc::vi2d& vAnchor) { m_vAnchor = vAnchor; }

	inline void IncrTotalScore(uint16_t iScoreToAdd) { m_iTotalScore += iScoreToAdd; }
	inline uint64_t GetTotalScore() const { return m_iTotalScore;}
	
private :

	std::string strName = "NewPlayer";
	std::map <EValues, std::pair<bool, uint8_t>> m_mapScoreSate;

	uint64_t m_iTotalScore = 0;

	olc::vi2d m_vAnchor;
};

#define MAX_PLAYERS 5

//Set up Interface
static const olc::vf2d vFrameAnchorPercentage(.35f,.25f);
static const olc::vf2d vFrameSizePercentage(.30f, .5f);

#define NUM_PLAYERS_TITTLE_TEXT "Nombre de joueurs"
#define NUM_PLAYERS_TITTLE_SIZE 3
#define PLAYER_NAME_SIZE 2
#define NUM_MAX_PLAYERS 5

#define NUM_ROUNDS_TEXT "Nombre de parties"
#define NUM_ROUNDS_TEXT_SIZE 3
#define NUM_ROUND_MAX 12

//Grid Parameters
#define GridHeightPercentage .25
#define ScoreValueWidth 220
#define ScoreValueHeight 40


//Text Entry Parameters
#define LiteralOffset olc::vi2d(10, .3 * ScoreValueHeight)
#define LiteralSize 2

// Override base class with your custom functionality
class YamScoreBoard : public olc::PixelGameEngine
{
	

public:
	enum EGamePhase
	{
		SetUp,
		Round,
		End
	};

	struct GameState
	{
		EGamePhase eCurrentPhase = SetUp;
		std::vector<std::shared_ptr<Player>> arrPlayers;

		size_t GetNumPlayers() { return arrPlayers.size();}
		size_t AddPlayer();// {arrPlayers.emplace_back(std::make_shared<Player>()); return GetNumPlayers(); }
		size_t RemovePlayer();

		uint8_t iNumRounds = 1;
		uint8_t GetNumRounds() { return iNumRounds;}
		uint8_t IncrNumRounds();
		uint8_t DecrNumRounds();
	};

	struct RoundState
	{
		uint8_t idxFirstPlayer	 = 0;
		uint8_t idxCurrentPlayer = 0;
		uint8_t idxBestPlayer	 = 0;
	};

	struct RenderState
	{

		olc::vi2d vMidSCreen;

		//Set up phase UI Info
		olc::vi2d vSetUpFrameAnchor;
		olc::vi2d vSetUpFrameSize;
		int iNumPlayerTextOffsetY = 0;
		int iNumPlayerValueOffsetY = 0;
		
		bool bShowTotals = true;
		olc::vi2d vEntryAnchor;

		std::vector<std::vector<olc::vi2d>> m_matAnchors; //Compute once left top corner for each score cell
		
	};

	class TextButtonWidget
	{
	public:
		TextButtonWidget() {};
		
		TextButtonWidget(olc::PixelGameEngine* renderer, const olc::vi2d& pos, const olc::vi2d& size, const std::string& text) :
			m_ParentRenderer(renderer),
			m_vPos(pos),
			m_vSize(size),
			m_strText(text)
		{}
		
		bool IsHovered() const;

		bool OnClicked() const;

		void Draw();
		
		olc::Pixel cBackgroundColor = olc::BLACK;
		olc::Pixel cBorderColor		= olc::WHITE;
		olc::Pixel cHoveredColor	= olc::WHITE;
		olc::Pixel cTextColor		= olc::WHITE;
		olc::Pixel cHoveredTextColor= olc::BLACK;

		uint32_t iTextScale = 1;

	private:

		olc::PixelGameEngine* m_ParentRenderer = nullptr;
		olc::vi2d m_vPos;
		olc::vi2d m_vSize;
		std::string m_strText;
	};

	YamScoreBoard()
	{
		// Name your application
		sAppName = "Yam's Score Board";
	}
	bool OnUserCreate() override;

	bool OnUserUpdate(float fElapsedTime) override;

	virtual void OnTextEntryComplete(const std::string& strText) override;

private:

	void DrawSetUpPhase(float fElapsedTime);
	void DrawRoundPhase(float fElapsedTime);
	void DrawEndPhase(float fElapsedTime);
	
	void DrawScoreColumn(olc::vi2d& vPos);

	void DrawPlayerColumn(std::shared_ptr<Player> pPlayer, olc::vi2d& vPos);

	uint16_t ComputeST1(std::shared_ptr<Player> pPlayer);
	bool HasBonus(std::shared_ptr<Player> pPlayer);
	uint16_t ComputeST2(std::shared_ptr<Player> pPlayer);
	uint16_t ComputeTT(std::shared_ptr<Player> pPlayer);

	RenderState m_RenderState;
	GameState m_GameState;
	RoundState m_CurrentRoundState;
	
	olc::vi2d vEntryAnchor;

	std::shared_ptr<Player> m_pCurrentWritingPlayer = nullptr;
	EValues eCurrentWritingValue;

	TextButtonWidget LaunchButton;
};
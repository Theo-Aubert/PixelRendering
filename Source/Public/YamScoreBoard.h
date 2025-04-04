#pragma once

#include "PixelApp.h"
#include "AppManager.h"
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

	inline void IncrTotalScore(uint64_t iScoreToAdd) { m_iTotalScore += iScoreToAdd; }
	inline void IncrVictoryCount() { m_iVictoryCount++; }
	inline uint64_t GetTotalScore() const { return m_iTotalScore;}
	inline uint8_t GetVictoryCount() const {return m_iVictoryCount;}
	inline std::map<int, uint16_t>& GetRoundScores() { return  m_mapRoundScores; }
	
private :

	std::string strName = "NewPlayer";
	std::map <EValues, std::pair<bool, uint8_t>> m_mapScoreSate;

	std::map<int, uint16_t> m_mapRoundScores;
	uint8_t  m_iVictoryCount = 0;
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
#define PLAYER_NAME_MAX_CHAR 12

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
class YamScoreBoard : public PixelApp
{
	

public:
	enum EGamePhase
	{
		SetUp,
		Round,
		ScoreTable,
		End
	};

	//struct RoundState;

	struct GameState
	{
		EGamePhase eCurrentPhase = SetUp;
		std::vector<std::shared_ptr<Player>> arrPlayers;

		size_t GetNumPlayers() { return arrPlayers.size();}
		size_t AddPlayer();
		size_t RemovePlayer();
		void InitGame();

		uint8_t iNumRounds = 1;
		uint8_t GetNumRounds() { return iNumRounds;}
		uint8_t IncrNumRounds();
		uint8_t DecrNumRounds();

		void StartNewRound();
		void EndRound();
		uint8_t idxRound = 1;
		bool bIsComplete = false;

		 //RoundState& refCurrentRound;

	private :
		void InitRoundScores(std::shared_ptr<Player> pPlayer);
	};

	struct RoundState
	{
		GameState* gameState = nullptr;
		
		uint8_t idxFirstPlayer	 = 0;
		uint8_t idxCurrentPlayer = 0;
		uint8_t idxBestPlayer	 = 0;
		uint8_t idxTurn			 = 1;

		std::pair<EValues, uint8_t> bufferScore = std::make_pair(Name, 0);

		void NextPlayerTurn();

		bool bIsComplete = false;
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
		bool bSHowHints = true;
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
		
		inline void SetText(const std::string& strText) { m_strText = strText; }

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

	class ListWidget
	{
	public:
		ListWidget() {};

		ListWidget(olc::PixelGameEngine* renderer, const olc::vi2d& pos, const olc::vi2d& size, std::vector<std::string>& arrEntries) :
			m_ParentRenderer(renderer),
			m_vPos(pos),
			m_vSize(size),
			m_arrEntries(arrEntries)
		{}

		olc::Pixel cBackgroundColor = olc::BLACK;
		olc::Pixel cBorderColor = olc::RED;
		olc::Pixel cHoveredColor = olc::WHITE;
		olc::Pixel cTextColor = olc::WHITE;
		olc::Pixel cHoveredTextColor = olc::BLACK;

		uint32_t iTextScale = LiteralSize;
		
		bool IsHovered() const;

		bool OnClicked(std::string& strOutEntry) const;


		void Draw();

	private:

		olc::PixelGameEngine* m_ParentRenderer = nullptr;
		olc::vi2d m_vPos;
		olc::vi2d m_vSize;
		std::vector<std::string> m_arrEntries;
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
	void DrawScoreTable(float fElapsedTime);
	void DrawEndPhase(float fElapsedTime);


	void DrawScoreColumn(olc::vi2d& vPos);
	void DrawPlayerColumn(int idxPlayer, olc::vi2d& vPos, bool bIsCurrentPlayer);
	bool ComputeHints(std::map<EValues, uint8_t>& mapHints, int idxPlayer);
	void ClickValueCell();
	void NextPlayerTurn();
	void Reset();

	//Score Computation functions
	static uint16_t ComputeST1(std::shared_ptr<Player> pPlayer);
	static bool HasBonus(std::shared_ptr<Player> pPlayer);
	static uint16_t ComputeST2(std::shared_ptr<Player> pPlayer);
	static uint64_t ComputeTT(std::shared_ptr<Player> pPlayer);
	int ComputeGlobalBestPlayerIdx();

	void DrawBestPlayer();

	RenderState m_RenderState;
	GameState m_GameState;
	RoundState m_CurrentRoundState;
	
	bool bIsSelectingScore = false;
	bool bIsViewingScores = false;
	olc::vi2d vEntryAnchor;
	olc::vi2d m_vHoveredCell;
	olc::vi2d m_vMainAnchor;

	std::shared_ptr<Player> m_pCurrentWritingPlayer = nullptr;
	EValues eCurrentWritingValue;

	TextButtonWidget LaunchButton;
	TextButtonWidget NextButton;
	TextButtonWidget TurnCount;
	TextButtonWidget RoundCount;
	TextButtonWidget NextRoundButton;
	TextButtonWidget EndGameButton;
	TextButtonWidget BackToMenuButton;
	TextButtonWidget QuitRoundButton;
	ListWidget ValueWidget;

	olc::Sprite* m_pCrownSprite = nullptr;
	olc::Decal* m_pCrownDecal = nullptr;
};
REGISTER_CLASS(YamScoreBoard)
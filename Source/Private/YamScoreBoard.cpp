#include "../Public/YamScoreBoard.h"
#include <cmath>
void Player::InitScoreState()
{
	m_mapScoreSate.clear();

	for (uint8_t e = EValues::As; e != EValues::Name; e++)
	{
		m_mapScoreSate.emplace(std::make_pair(static_cast<EValues>(e),std::make_pair(false, 0)));
	}

}

size_t YamScoreBoard::GameState::AddPlayer()
{
	if(arrPlayers.size() < NUM_MAX_PLAYERS)
	{
		std::shared_ptr<Player> pPlayer = std::make_shared<Player>();
		arrPlayers.emplace_back(pPlayer);
	}

	return GetNumPlayers();
}

size_t YamScoreBoard::GameState::RemovePlayer()
{
	if(arrPlayers.size() >= 2)
	{
		arrPlayers.pop_back();
	}

	return GetNumPlayers();
}

void YamScoreBoard::GameState::InitGame()
{
	for (auto player : arrPlayers)
	{
		player->InitScoreState();
		InitRoundScores(player);
	}
}

uint8_t YamScoreBoard::GameState::IncrNumRounds()
{
	if(iNumRounds < NUM_ROUND_MAX)
	{
		iNumRounds++;
	}

	return iNumRounds;
}

uint8_t YamScoreBoard::GameState::DecrNumRounds()
{
	if(iNumRounds > 1)
	{
		iNumRounds--;
	}

	return iNumRounds;
}

void YamScoreBoard::GameState::StartNewRound()
{
}

void YamScoreBoard::GameState::EndRound()
{
	uint16_t iBestRoundScore = 0;
	std::shared_ptr<Player> pBestPlayer = nullptr;
	for (auto player : arrPlayers)
	{
		player->GetRoundScores()[idxRound] = ComputeTT(player);
		player->IncrTotalScore(ComputeTT(player));

		if (player->GetRoundScores()[idxRound] > iBestRoundScore)
		{
			pBestPlayer = player;
			iBestRoundScore = player->GetRoundScores()[idxRound];
		}
	}

	pBestPlayer->IncrVictoryCount();

	++idxRound;

	if (idxRound > GetNumRounds())
	{
		bIsComplete = true;
	}
}

void YamScoreBoard::GameState::InitRoundScores(std::shared_ptr<Player> pPlayer)
{
	auto& mapRoundScores = pPlayer->GetRoundScores();

	mapRoundScores.clear();

	for (int i = 1; i <= iNumRounds; i++)
	{
		mapRoundScores.emplace(std::make_pair(i, 0));
	}
}

void YamScoreBoard::RoundState::NextPlayerTurn()
{
	if (bufferScore.first == Name)
		return;

	gameState->arrPlayers[idxCurrentPlayer]->GetScoreState()[bufferScore.first].first = true;
	gameState->arrPlayers[idxCurrentPlayer]->GetScoreState()[bufferScore.first].second = bufferScore.second;

	idxCurrentPlayer++;
	idxCurrentPlayer = idxCurrentPlayer % gameState->GetNumPlayers();

	//Compute best player index
	int idxNewBestPlayer = 0;
	uint64_t iBestScore = 0;
	uint64_t score = 0;
	for (int i = 0; i < gameState->arrPlayers.size() ; ++i)
	{
		
		std::shared_ptr player = gameState->arrPlayers[i];
		score = ComputeTT(player);
		if (score > iBestScore)
		{
			iBestScore = score;
			idxBestPlayer = i;
		}
	}

	if (idxCurrentPlayer == idxFirstPlayer)
	{
		++idxTurn;
	}

	if (idxTurn > 12)
	{
		bIsComplete = true;
	}
	//Reset buffer score with invalid value category so we can detect the uninitialized state
	bufferScore = std::make_pair(Name, 0);
}

bool YamScoreBoard::TextButtonWidget::IsHovered() const
{
	if (!m_ParentRenderer)
	{
		return false;
	}

	if(m_ParentRenderer->GetMousePos().x > m_vPos.x && m_ParentRenderer->GetMousePos().x < m_vPos.x + m_vSize.x
		&& m_ParentRenderer->GetMousePos().y > m_vPos.y && m_ParentRenderer->GetMousePos().y < m_vPos.y + m_vSize.y)
	{
		return true;		
	}

	return false;
}

bool YamScoreBoard::TextButtonWidget::OnClicked() const
{
	return IsHovered() && m_ParentRenderer->GetMouse(0).bReleased;
}

void YamScoreBoard::TextButtonWidget::Draw()
{
	if (!m_ParentRenderer)
	{
		return;
	}

	olc::vi2d vTextSize = m_ParentRenderer->GetTextSize(m_strText);
	olc::vi2d vScaledTextSize = vTextSize * iTextScale;

	olc::vi2d vTextPos = m_vPos + ((m_vSize / 2) - (vScaledTextSize / 2));
	
	if(IsHovered())
	{
		m_ParentRenderer->FillRect(m_vPos, m_vSize, cHoveredColor);
		m_ParentRenderer->DrawRect(m_vPos, m_vSize, cBorderColor);
		m_ParentRenderer->DrawString(vTextPos, m_strText, cHoveredTextColor, iTextScale);
		
	}
	else
	{
		m_ParentRenderer->FillRect(m_vPos, m_vSize, cBackgroundColor);
		m_ParentRenderer->DrawRect(m_vPos, m_vSize, cBorderColor);
		m_ParentRenderer->DrawString(vTextPos, m_strText, cTextColor, iTextScale);
	}
}

bool YamScoreBoard::OnUserCreate()
{
	//First player is always here
	m_GameState.AddPlayer();
	m_CurrentRoundState.gameState = &m_GameState;
	
	m_RenderState.vSetUpFrameAnchor = olc::vi2d(ScreenWidth()* vFrameAnchorPercentage.x, ScreenHeight()* vFrameAnchorPercentage.y);
	m_RenderState.vSetUpFrameSize = olc::vi2d(ScreenWidth() * vFrameSizePercentage.x, ScreenHeight()* vFrameSizePercentage.y);

	olc::vi2d vButtonSize = GetTextSize(std::string("Lancer la partie")) * 3;
	vButtonSize += olc::vi2d(150, 50);

	olc::vi2d vButtonPos = olc::vi2d((ScreenWidth() * .5) - (vButtonSize.x * .5), ScreenHeight() * .85); 
	LaunchButton = TextButtonWidget(this,vButtonPos, vButtonSize, std::string("Lancer la partie"));
	LaunchButton.iTextScale = 3;

	vButtonSize = GetTextSize(std::string("Suivant")) * 3;
	vButtonSize += olc::vi2d(150, 50);
	vButtonPos = olc::vi2d((ScreenWidth() * .5) - (vButtonSize.x * .5), ScreenHeight() * .90);
	NextButton = TextButtonWidget(this, vButtonPos, vButtonSize, std::string("Suivant"));
	NextButton.cBackgroundColor = olc::DARK_CYAN;
	NextButton.cBorderColor = olc::WHITE;
	NextButton.cHoveredColor = olc::DARK_GREEN;
	NextButton.cTextColor = olc::WHITE;
	NextButton.cHoveredTextColor = olc::YELLOW;
	NextButton.iTextScale = 3;

	m_pCrownSprite = new olc::Sprite(RESSOURCE_PATH + "crown-pixel-8-bit.png");

	if (!m_pCrownSprite)
	{
		std::cout << "Failed to load crown sprite\n";
	}

	m_pCrownDecal = new olc::Decal(m_pCrownSprite);

	
	vButtonSize = GetTextSize(std::string("Partie Suivante")) * 3;
	vButtonSize += olc::vi2d(150, 50);
	vButtonPos = olc::vi2d((ScreenWidth() * .5) - (vButtonSize.x * .5), ScreenHeight() * .80);
	NextRoundButton = TextButtonWidget(this, vButtonPos, vButtonSize, std::string("Partie Suivante"));
	NextRoundButton.cBackgroundColor = olc::DARK_CYAN;
	NextRoundButton.cBorderColor = olc::WHITE;
	NextRoundButton.cHoveredColor = olc::CYAN;
	NextRoundButton.cTextColor = olc::WHITE;
	NextRoundButton.cHoveredTextColor = olc::BLACK;
	NextRoundButton.iTextScale = 3;

	vButtonSize = GetTextSize(std::string("Fin de partie")) * 3;
	vButtonSize += olc::vi2d(150, 50);
	vButtonPos = olc::vi2d((ScreenWidth() * .85) - (vButtonSize.x * .5), ScreenHeight() * .80);
	EndGameButton = TextButtonWidget(this, vButtonPos, vButtonSize, std::string("Fin de partie"));
	EndGameButton.cBackgroundColor = olc::DARK_GREEN;
	EndGameButton.cBorderColor = olc::WHITE;
	EndGameButton.cHoveredColor = olc::DARK_RED;
	EndGameButton.cTextColor = olc::WHITE;
	EndGameButton.cHoveredTextColor = olc::BLACK;
	EndGameButton.iTextScale = 3;

	vButtonSize = GetTextSize(std::string("Retour au Menu")) * 3;
	vButtonSize += olc::vi2d(150, 50);
	vButtonPos = olc::vi2d((ScreenWidth() * .5) - (vButtonSize.x * .5), ScreenHeight() * .80);
	BackToMenuButton = TextButtonWidget(this, vButtonPos, vButtonSize, std::string("Retour au Menu"));
	/*EndGameButton.cBackgroundColor = olc::BLA;
	EndGameButton.cBorderColor = olc::WHITE;
	EndGameButton.cHoveredColor = olc::DARK_RED;
	EndGameButton.cTextColor = olc::WHITE;
	EndGameButton.cHoveredTextColor = olc::BLACK;*/
	BackToMenuButton.iTextScale = 3;

	vButtonSize = GetTextSize(std::string("Quitter")) * 2;
	vButtonSize += olc::vi2d(150, 50);
	vButtonPos = olc::vi2d((ScreenWidth() * .90) - (vButtonSize.x * .5), ScreenHeight() * .05);
	QuitRoundButton = TextButtonWidget(this, vButtonPos, vButtonSize, std::string("Retour au Menu"));
	QuitRoundButton.cBackgroundColor = olc::DARK_RED;
	QuitRoundButton.cBorderColor = olc::WHITE;
	QuitRoundButton.cHoveredColor = olc::RED;
	QuitRoundButton.cTextColor = olc::BLACK;
	QuitRoundButton.cHoveredTextColor = olc::WHITE;
	QuitRoundButton.iTextScale = 2;

	return true;
}

bool YamScoreBoard::OnUserUpdate(float fElapsedTime)
{

	if(GetKey(olc::ESCAPE).bReleased )
	{
		return false;
	}

	switch (m_GameState.eCurrentPhase)
	{
	case SetUp:
		DrawSetUpPhase(fElapsedTime);

		if (!IsTextEntryEnabled())
		{
			if (GetKey(olc::RIGHT).bReleased)
				m_GameState.IncrNumRounds();

			if (GetKey(olc::LEFT).bReleased)
				m_GameState.DecrNumRounds();

			if (GetKey(olc::UP).bReleased)
				m_GameState.AddPlayer();

			if (GetKey(olc::DOWN).bReleased)
				m_GameState.RemovePlayer();

			bool bLaunched = LaunchButton.OnClicked() || GetKey(olc::L).bReleased;
			if (bLaunched)
			{
				m_GameState.InitGame();

				//m_GameState.eCurrentPhase = ScoreTable;
				m_GameState.eCurrentPhase = Round;
				olc::vi2d vButtonSize = GetTextSize(std::string("Tour :\n\n12 / 12\n")) * 3;
				TurnCount = TextButtonWidget(this, olc::vi2d(ScreenWidth() * .85, ScreenHeight() * .5), vButtonSize, "Tour :\n\n1 / 12");
				TurnCount.iTextScale = 3;

				vButtonSize = GetTextSize(std::string("Partie  :\n\n12 / 12\n")) * 3;
				RoundCount = TextButtonWidget(this, olc::vi2d(ScreenWidth() * .85, ScreenHeight() * .75), vButtonSize, "Partie :\n\n1 / " +std::to_string(m_GameState.GetNumRounds()));
				RoundCount.iTextScale = 3;

				//TurnCount.SetText("Tour :\n\n" + std::to_string(m_CurrentRoundState.idxTurn) + " / 12");
				//RoundCount.SetText("Partie :\n\n" + std::to_string(m_GameState.idxRound) + " / " + std::to_string(m_GameState.GetNumRounds()));
			}
		}
		return true;
		break;
	case Round :
		DrawRoundPhase(fElapsedTime);
		return true;
		break;

	case ScoreTable:
		DrawScoreTable(fElapsedTime);
		return true;
		break;
	case End:
		DrawEndPhase(fElapsedTime);
		break;
	default:
		break;
	}
	
	return true;
}

void YamScoreBoard::OnTextEntryComplete(const std::string& strText)
{
	
	if (m_pCurrentWritingPlayer)
	{
		if (eCurrentWritingValue == Name)
		{
			m_pCurrentWritingPlayer->SetName(strText.substr(0,PLAYER_NAME_MAX_CHAR));
		}
		else
		{
			//Brelan case, nothing else to handle here
			int value = std::atoi(strText.c_str());
			value = std::clamp(value, 0, 30);
			m_CurrentRoundState.bufferScore = std::make_pair(eCurrentWritingValue, value);
		}
	}
}

void YamScoreBoard::DrawSetUpPhase(float fElapsedTime)
{
	Clear(olc::VERY_DARK_BLUE);

	std::string strMainTittle = "YAM'S GAMING";
	olc::vi2d vMainTittleSize = GetTextSize(strMainTittle) * 6;
	olc::vi2d vMainTittlePos = olc::vi2d((ScreenWidth() /2) - (vMainTittleSize.x / 2), (ScreenHeight() * .15) - (vMainTittleSize.y / 2));

	DrawString(vMainTittlePos + olc::vi2d(5,5) ,strMainTittle,olc::DARK_YELLOW, 6);
	DrawString(vMainTittlePos,strMainTittle,olc::WHITE, 6);
	
	DrawRect(m_RenderState.vSetUpFrameAnchor, m_RenderState.vSetUpFrameSize);

	std::string strNumPlayersTittle(NUM_PLAYERS_TITTLE_TEXT);
	olc::vi2d vTittleSize = GetTextSize(strNumPlayersTittle) * NUM_PLAYERS_TITTLE_SIZE;
	DrawString(olc::vi2d((ScreenWidth()/2) - (vTittleSize.x /2), ScreenHeight() /4 + 25), strNumPlayersTittle, olc::WHITE, NUM_PLAYERS_TITTLE_SIZE);

	//Left Triangle
	
	olc::vi2d vLeftT1 = olc::vi2d((ScreenWidth()/2) - (vTittleSize.x /4), (ScreenHeight() /4) + 25+ vTittleSize.y * 1.5 + 25);
	olc::vi2d vLeftT2 = olc::vi2d((ScreenWidth()/2) - (vTittleSize.x /4), (ScreenHeight() /4) + 25 + vTittleSize.y * 1.5 + 55);
	olc::vi2d vLeftT3 = olc::vi2d((ScreenWidth()/2) - (vTittleSize.x /4) - 25, (ScreenHeight() /4) + 25 + vTittleSize.y * 1.5 + 40);

	//box
	olc::vi2d vLeftArrowBoxPos = olc::vi2d((ScreenWidth()/2) - (vTittleSize.x /4) - 25, (ScreenHeight() /4) + 25+ vTittleSize.y * 1.5 + 25);
	olc::vi2d vLeftArrowBoxSize = olc::vi2d(25, 30);

	bool bIsHovered = GetMousePos().x > vLeftArrowBoxPos.x && GetMousePos().x < vLeftArrowBoxPos.x + vLeftArrowBoxSize.x
					&& GetMousePos().y > vLeftArrowBoxPos.y && GetMousePos().y < vLeftArrowBoxPos.y + vLeftArrowBoxSize.y;

	if(m_GameState.GetNumPlayers() != 1)
		FillTriangle(vLeftT1,vLeftT2,vLeftT3, bIsHovered ? olc::WHITE : olc::DARK_GREY );

	if(bIsHovered && !IsTextEntryEnabled() && GetMouse(0).bReleased)
	{
		m_GameState.RemovePlayer();
	}

	//Right Triangle
	olc::vi2d vRightT1 = olc::vi2d((ScreenWidth()/2) + (vTittleSize.x /4), (ScreenHeight() /4) + 25 + vTittleSize.y * 1.5 + 25);
	olc::vi2d vRightT2 = olc::vi2d((ScreenWidth()/2) + (vTittleSize.x /4), (ScreenHeight() /4) + 25 + vTittleSize.y * 1.5 + 55);
	olc::vi2d vRightT3 = olc::vi2d((ScreenWidth()/2) + (vTittleSize.x /4) + 25, (ScreenHeight() /4) + 25 +  vTittleSize.y * 1.5 + 40);

	//box
	olc::vi2d vRightArrowBoxPos = olc::vi2d((ScreenWidth()/2) + (vTittleSize.x /4), (ScreenHeight() /4) + 25+ vTittleSize.y * 1.5 + 25);
	olc::vi2d vRightArrowBoxSize = olc::vi2d(25, 30);

	bIsHovered = GetMousePos().x > vRightArrowBoxPos.x && GetMousePos().x < vRightArrowBoxPos.x + vRightArrowBoxSize.x
					&& GetMousePos().y > vRightArrowBoxPos.y && GetMousePos().y < vRightArrowBoxPos.y + vRightArrowBoxSize.y;

	if(m_GameState.GetNumPlayers() != NUM_MAX_PLAYERS)
		FillTriangle(vRightT1,vRightT2,vRightT3, bIsHovered ? olc::WHITE : olc::DARK_GREY);

	if(bIsHovered && !IsTextEntryEnabled()&& GetMouse(0).bReleased)
	{
		m_GameState.AddPlayer();
	}

	DrawString(olc::vi2d(ScreenWidth() /2 - 10, (ScreenHeight() /4) + 25 + vTittleSize.y * 1.5 + 30),std::to_string(m_GameState.GetNumPlayers()), olc::WHITE, NUM_PLAYERS_TITTLE_SIZE);

	int hName = GetTextSize("New").y * PLAYER_NAME_SIZE;
	for(size_t i = 0; i < m_GameState.GetNumPlayers(); ++i)
	{
		olc::vi2d vPlayerNameSize = GetTextSize(m_GameState.arrPlayers[i]->GetName()) * PLAYER_NAME_SIZE;
		olc::vi2d vPos = olc::vi2d(ScreenWidth()/2 - (vPlayerNameSize.x /2),(ScreenHeight() * .4)  + i * (vPlayerNameSize.y + 15));

		int hName = vPlayerNameSize.y;
		DrawString(olc::vi2d(ScreenWidth()/2 - (vPlayerNameSize.x /2),(ScreenHeight() * .4)  + i * (vPlayerNameSize.y + 15)), m_GameState.arrPlayers[i]->GetName(), olc::DARK_YELLOW, PLAYER_NAME_SIZE);
		
		bIsHovered = GetMousePos().x > vPos.x && GetMousePos().x < vPos.x + vPlayerNameSize.x && GetMousePos().y > vPos.y && GetMousePos().y < vPos.y + vPlayerNameSize.y;

		if(bIsHovered)
		{
			DrawRect(vPos - olc::vi2d(5,5), vPlayerNameSize + olc::vi2d(10,10), olc::WHITE);
			if(GetMouse(0).bReleased && !IsTextEntryEnabled())
			{
				TextEntryEnable(true);
				m_pCurrentWritingPlayer = m_GameState.arrPlayers[i];
				eCurrentWritingValue = Name;
				vEntryAnchor = vPos;
				m_GameState.arrPlayers[i]->SetName("");
			}
		}
		
	}

	if(IsTextEntryEnabled())
	{
		DrawString(vEntryAnchor, TextEntryGetString(), olc::YELLOW, PLAYER_NAME_SIZE);
		DrawRect(vEntryAnchor - olc::vi2d(5,5), olc::vi2d(((ScreenWidth() /2) - vEntryAnchor.x) * 2 + 10,hName + 10), olc::YELLOW);
	}

	std::string strNumRoundsTittle(NUM_ROUNDS_TEXT);
	olc::vi2d vRoundTextSize = GetTextSize(strNumRoundsTittle) *  NUM_ROUNDS_TEXT_SIZE;
	
	DrawString(olc::vi2d(ScreenWidth()/2 - (vRoundTextSize.x/2), ScreenHeight() * 2.5 / 4), strNumRoundsTittle, olc::WHITE, NUM_ROUNDS_TEXT_SIZE);

	olc::vi2d vrLeftT1 = olc::vi2d((ScreenWidth()/2) - (vTittleSize.x /4), (ScreenHeight() * 2.5/4) + 25+ vTittleSize.y * 1.5 + 25);
	olc::vi2d vrLeftT2 = olc::vi2d((ScreenWidth()/2) - (vTittleSize.x /4), (ScreenHeight()*2.5/4) + 25 + vTittleSize.y * 1.5 + 55);
	olc::vi2d vrLeftT3 = olc::vi2d((ScreenWidth()/2) - (vTittleSize.x /4) - 25, (ScreenHeight() *2.5/4) + 25 + vTittleSize.y * 1.5 + 40);

	//box
	olc::vi2d vrLeftArrowBoxPos = olc::vi2d((ScreenWidth()/2) - (vTittleSize.x /4) - 25, (ScreenHeight() * 2.5 /4) + 25+ vTittleSize.y * 1.5 + 25);
	olc::vi2d vrLeftArrowBoxSize = olc::vi2d(25, 30);

	bIsHovered = GetMousePos().x > vrLeftArrowBoxPos.x && GetMousePos().x < vrLeftArrowBoxPos.x + vrLeftArrowBoxSize.x
					&& GetMousePos().y > vrLeftArrowBoxPos.y && GetMousePos().y < vrLeftArrowBoxPos.y + vrLeftArrowBoxSize.y;

	if(m_GameState.GetNumRounds() != 1)
		FillTriangle(vrLeftT1,vrLeftT2,vrLeftT3, bIsHovered ? olc::WHITE : olc::DARK_GREY );

	if(bIsHovered && !IsTextEntryEnabled() && GetMouse(0).bReleased)
	{
		m_GameState.DecrNumRounds();
	}

	//Right Triangle
	olc::vi2d vrRightT1 = olc::vi2d((ScreenWidth()/2) + (vTittleSize.x /4), (ScreenHeight() *2.5/4) + 25 + vTittleSize.y * 1.5 + 25);
	olc::vi2d vrRightT2 = olc::vi2d((ScreenWidth()/2) + (vTittleSize.x /4), (ScreenHeight() *2.5/4) + 25 + vTittleSize.y * 1.5 + 55);
	olc::vi2d vrRightT3 = olc::vi2d((ScreenWidth()/2) + (vTittleSize.x /4) + 25, (ScreenHeight() *2.5/4) + 25 +  vTittleSize.y * 1.5 + 40);

	//box
	olc::vi2d vrRightArrowBoxPos = olc::vi2d((ScreenWidth()/2) + (vTittleSize.x /4), (ScreenHeight() *2.5 /4) + 25+ vTittleSize.y * 1.5 + 25);
	olc::vi2d vrRightArrowBoxSize = olc::vi2d(25, 30);

	bIsHovered = GetMousePos().x > vrRightArrowBoxPos.x && GetMousePos().x < vrRightArrowBoxPos.x + vrRightArrowBoxSize.x
					&& GetMousePos().y > vrRightArrowBoxPos.y && GetMousePos().y < vrRightArrowBoxPos.y + vrRightArrowBoxSize.y;

	if(m_GameState.GetNumRounds() != NUM_ROUND_MAX)
		FillTriangle(vrRightT1,vrRightT2,vrRightT3,  bIsHovered ? olc::WHITE : olc::DARK_GREY);

	if(bIsHovered && !IsTextEntryEnabled() && GetMouse(0).bReleased)
	{
		m_GameState.IncrNumRounds();
	}

	DrawString(olc::vi2d((ScreenWidth() /2) - 10, (ScreenHeight() *2.5/4) + 25 + vTittleSize.y * 1.5 + 30),std::to_string(m_GameState.GetNumRounds()), olc::WHITE, NUM_PLAYERS_TITTLE_SIZE);
	olc::vi2d vButtonAnchor(ScreenWidth() * .15, ScreenHeight() * .75);
	olc::vi2d vButtonSize(ScreenWidth() * .05, ScreenWidth() * .05);
	
	bool bRectHovered = /*shorcut*/ GetKey(olc::D).bHeld  ||
						/*button*/	GetMousePos().x > vButtonAnchor.x && GetMousePos().x < vButtonAnchor.x + vButtonSize.x &&  GetMousePos().y > vButtonAnchor.y && GetMousePos().y < vButtonAnchor.y + vButtonSize.y;
	
	//FillRect(vButtonAnchor, vButtonSize, bRectHovered ? olc::GREEN : olc::RED);
	LaunchButton.Draw();
}

void YamScoreBoard::DrawRoundPhase(float fElapsedTime)
{
	Clear(olc::VERY_DARK_BLUE);

	//Shortcuts (avoid conflict when typing
	if (!IsTextEntryEnabled())
	{
		if (GetKey(olc::T).bReleased)
		{
			m_RenderState.bShowTotals = !m_RenderState.bShowTotals;
		}

		if (GetKey(olc::H).bReleased)
		{
			m_RenderState.bSHowHints = !m_RenderState.bSHowHints;
		}

		if (GetKey(olc::S).bPressed)
		{
			m_GameState.eCurrentPhase = ScoreTable;
			bIsViewingScores = true;
		}
	}


	m_vMainAnchor = olc::vi2d((ScreenWidth() / 2) - (m_GameState.GetNumPlayers() + 1) * ScoreValueWidth / 2, ScreenHeight() / 4);
	//m_vHoveredCell = olc::vi2d(std::clamp(((GetMousePos().x - m_vMainAnchor.x) / ScoreValueWidth) - 1, 0, (int)(m_GameState.GetNumPlayers() - 1)), std::clamp(((GetMousePos().y - m_vMainAnchor.y) / ScoreValueHeight), 0, 14));
	m_vHoveredCell = olc::vi2d(((GetMousePos().x - m_vMainAnchor.x) / ScoreValueWidth) - 1, ((GetMousePos().y - m_vMainAnchor.y) / ScoreValueHeight));
	DrawScoreColumn(m_vMainAnchor);

	for (uint8_t iPlayers = 0; iPlayers < m_GameState.GetNumPlayers(); ++iPlayers)
	{

		std::shared_ptr<Player> pPlayer = m_GameState.arrPlayers[iPlayers];

		olc::vi2d vPlayerAnchor = m_vMainAnchor + olc::vi2d(ScoreValueWidth * (iPlayers + 1), -ScoreValueHeight);
		pPlayer->SetAnchor(vPlayerAnchor);
		DrawPlayerColumn(iPlayers, vPlayerAnchor, m_CurrentRoundState.idxCurrentPlayer == iPlayers);
	}

	NextButton.Draw();
	TurnCount.Draw();
	RoundCount.Draw();
	QuitRoundButton.Draw();

	if (m_GameState.GetNumPlayers() > 1)
	{
		DrawBestPlayer();
	}

	if (!bIsSelectingScore && !IsTextEntryEnabled() && (GetKey(olc::SPACE).bReleased || NextButton.OnClicked()))
	{
		m_CurrentRoundState.NextPlayerTurn();


		if (m_CurrentRoundState.bIsComplete)
		{
			m_GameState.EndRound();

			if (!m_GameState.bIsComplete)
			{

				uint8_t iNewFirstPlayer = ++m_CurrentRoundState.idxFirstPlayer % m_GameState.GetNumPlayers();
				m_CurrentRoundState = RoundState();
				m_CurrentRoundState.bIsComplete = false;
				m_CurrentRoundState.idxFirstPlayer = iNewFirstPlayer;
				m_CurrentRoundState.idxCurrentPlayer = m_CurrentRoundState.idxFirstPlayer;
				m_CurrentRoundState.idxBestPlayer = m_CurrentRoundState.idxFirstPlayer;
				m_CurrentRoundState.gameState = &m_GameState;

				for (auto player : m_GameState.arrPlayers)
				{
					player->InitScoreState();
				}
			}
			m_GameState.eCurrentPhase = ScoreTable;
		}

		TurnCount.SetText("Tour :\n\n" + std::to_string(m_CurrentRoundState.idxTurn) + " / 12");
		RoundCount.SetText("Partie :\n\n" + std::to_string(m_GameState.idxRound) + " / " + std::to_string(m_GameState.GetNumRounds()));



	}

	if (QuitRoundButton.OnClicked() || (!IsTextEntryEnabled() && GetKey(olc::Q).bReleased))
	{
		Reset();
	}
}

void YamScoreBoard::DrawScoreTable(float fElapsedTime)
{
	Clear(olc::VERY_DARK_GREEN);

	if (GetKey(olc::S).bReleased && bIsViewingScores)
	{
		m_GameState.eCurrentPhase = Round;
		bIsViewingScores = false;
	}

	std::string strMainTittle = "SCORE";
	olc::vi2d vMainTittleSize = GetTextSize(strMainTittle) * 6;
	olc::vi2d vMainTittlePos = olc::vi2d((ScreenWidth() / 2) - (vMainTittleSize.x / 2), (ScreenHeight() * .15) - (vMainTittleSize.y / 2));

	DrawString(vMainTittlePos + olc::vi2d(5, 5), strMainTittle, olc::DARK_RED, 6);
	DrawString(vMainTittlePos, strMainTittle, olc::WHITE, 6);

	int iCurrentRound = bIsViewingScores? m_GameState.idxRound : m_GameState.idxRound - 1; //idxRound has been incremented before drawing this if the round is finished
	for (int i = 1; i <= m_GameState.GetNumRounds(); ++i)
	{
		if (i < iCurrentRound)
		{
			FillRect(m_vMainAnchor + olc::vi2d(0, ScoreValueHeight * i), olc::vi2d(ScoreValueWidth, ScoreValueHeight), olc::GREY);
		}

		if (i == iCurrentRound)
		{
			FillRect(m_vMainAnchor + olc::vi2d(0, ScoreValueHeight * i), olc::vi2d(ScoreValueWidth, ScoreValueHeight), olc::DARK_CYAN);
		}
		DrawRect(m_vMainAnchor + olc::vi2d(0, ScoreValueHeight * i), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(m_vMainAnchor + olc::vi2d(LiteralOffset.x, LiteralOffset.y + ScoreValueHeight * i), "Partie " + std::to_string(i), i == iCurrentRound ? olc::BLACK : olc::WHITE, LiteralSize);
	}

	DrawRect(m_vMainAnchor + olc::vi2d(0, ScoreValueHeight * (m_GameState.GetNumRounds() + 1)), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	DrawString(m_vMainAnchor + olc::vi2d(LiteralOffset.x, LiteralOffset.y + ScoreValueHeight * (m_GameState.GetNumRounds() + 1)), "TOTAL", olc::YELLOW, LiteralSize);

	if (m_GameState.GetNumPlayers() > 1)
	{

		DrawRect(m_vMainAnchor + olc::vi2d(0, ScoreValueHeight * (m_GameState.GetNumRounds() + 2)), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(m_vMainAnchor + olc::vi2d(LiteralOffset.x, LiteralOffset.y + ScoreValueHeight * (m_GameState.GetNumRounds() + 2)), "Victoires", olc::CYAN, LiteralSize);	
	}

	for (int idx = 1;  idx < m_GameState.arrPlayers.size() + 1; ++idx)
	{
		DrawRect(m_vMainAnchor + olc::vi2d(ScoreValueWidth * idx, 0), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(m_vMainAnchor + olc::vi2d(ScoreValueWidth * idx + LiteralOffset.x, LiteralOffset.y), m_GameState.arrPlayers[idx - 1]->GetName(), olc::YELLOW, LiteralSize);

		for (int i = 1; i <= m_GameState.GetNumRounds(); ++i)
		{
			if (i < iCurrentRound)
			{
				FillRect(m_vMainAnchor + olc::vi2d(ScoreValueWidth * idx, ScoreValueHeight * i), olc::vi2d(ScoreValueWidth, ScoreValueHeight), olc::GREY);
			}

			if (i == iCurrentRound)
			{
				FillRect(m_vMainAnchor + olc::vi2d(ScoreValueWidth * idx, ScoreValueHeight * i), olc::vi2d(ScoreValueWidth, ScoreValueHeight), olc::DARK_CYAN);
				uint64_t iCurrentSCore = bIsViewingScores ? ComputeTT(m_GameState.arrPlayers[idx - 1]) : m_GameState.arrPlayers[idx - 1]->GetRoundScores()[i];
				DrawString(m_vMainAnchor + olc::vi2d(ScoreValueWidth * idx + LiteralOffset.x, LiteralOffset.y + ScoreValueHeight * i), std::to_string(iCurrentSCore), olc::BLACK, LiteralSize);
			}
			else
			{
				DrawString(m_vMainAnchor + olc::vi2d(ScoreValueWidth * idx + LiteralOffset.x, LiteralOffset.y + ScoreValueHeight * i), std::to_string(m_GameState.arrPlayers[idx - 1]->GetRoundScores()[i]), olc::WHITE, LiteralSize);
			}

			DrawRect(m_vMainAnchor + olc::vi2d(ScoreValueWidth * idx, ScoreValueHeight * i), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
			
		}

		uint64_t iTotal = bIsViewingScores ? m_GameState.arrPlayers[idx - 1]->GetTotalScore() + ComputeTT(m_GameState.arrPlayers[idx - 1]) : m_GameState.arrPlayers[idx - 1]->GetTotalScore();
		DrawRect(m_vMainAnchor + olc::vi2d(ScoreValueWidth * idx, ScoreValueHeight * (m_GameState.GetNumRounds() + 1)), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(m_vMainAnchor + olc::vi2d(ScoreValueWidth * idx + LiteralOffset.x, LiteralOffset.y + ScoreValueHeight * (m_GameState.GetNumRounds() + 1)), std::to_string(iTotal), olc::YELLOW, LiteralSize);

		if (m_GameState.GetNumPlayers() > 1)
		{

			DrawRect(m_vMainAnchor + olc::vi2d(ScoreValueWidth * idx, ScoreValueHeight * (m_GameState.GetNumRounds() + 2)), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
			DrawString(m_vMainAnchor + olc::vi2d(ScoreValueWidth * idx + LiteralOffset.x, LiteralOffset.y + ScoreValueHeight * (m_GameState.GetNumRounds() + 2)), std::to_string(m_GameState.arrPlayers[idx - 1]->GetVictoryCount()), olc::CYAN, LiteralSize);
		}
	}

	if (m_GameState.GetNumPlayers() > 1)
	{

		int idxBestPlayer = ComputeGlobalBestPlayerIdx();
		olc::vi2d vCrownPos = m_vMainAnchor + olc::vi2d((idxBestPlayer + 1.5) * ScoreValueWidth - (m_pCrownSprite->width * .1) / 2, -2 * ScoreValueHeight);
		DrawDecal(vCrownPos, m_pCrownDecal, { 0.1,0.1 });
	}

	if (!bIsViewingScores)
	{
		if (!m_GameState.bIsComplete)
		{
			NextRoundButton.Draw();
			bool bIsNextRoundCliked = NextRoundButton.OnClicked();
			if (bIsNextRoundCliked || GetKey(olc::SPACE).bReleased)
			{
				m_GameState.eCurrentPhase = Round;
			}
		}

		
		EndGameButton.Draw();
		bool bIsEndGameClicked = EndGameButton.OnClicked();

		if (bIsEndGameClicked || GetKey(olc::Q).bReleased)
		{
			m_GameState.eCurrentPhase = End;
		}
	}
}

void YamScoreBoard::DrawEndPhase(float fElapsedTime)
{
	Clear(olc::VERY_DARK_MAGENTA);

	std::string strMainTittle = "PARTIE TERMINEE";
	olc::vi2d vMainTittleSize = GetTextSize(strMainTittle) * 6;
	olc::vi2d vMainTittlePos = olc::vi2d((ScreenWidth() / 2) - (vMainTittleSize.x / 2), (ScreenHeight() * .15) - (vMainTittleSize.y / 2));

	DrawString(vMainTittlePos + olc::vi2d(5, 5), strMainTittle, olc::BLACK, 6);
	DrawString(vMainTittlePos, strMainTittle, olc::YELLOW, 6);

	BackToMenuButton.Draw();

	if (BackToMenuButton.OnClicked() || GetKey(olc::SPACE).bReleased)
	{
		Reset();
	}

}

void YamScoreBoard::ClickValueCell()
{
	
	olc::vi2d vBoxAnchor = olc::vi2d(m_vMainAnchor.x + ScoreValueWidth * (m_vHoveredCell.x + 1), m_vMainAnchor.y + ScoreValueHeight * (m_vHoveredCell.y + 1));
	olc::vi2d vBoxSize = olc::vi2d(ScoreValueWidth, ScoreValueHeight * 7);
	std::vector<std::string> values;
	values.emplace_back("");

	switch (eCurrentWritingValue)
	{
		case As:
		case Deux:
		case Trois:
		case Quatre:
		case Cinq:
		case Six:

			vBoxSize = olc::vi2d(ScoreValueWidth, ScoreValueHeight * 7);

			for (int i = 0; i < 6; ++i)
			{
				uint8_t v = (eCurrentWritingValue + 1) * i;
				values.emplace_back(std::to_string(v));
			}

			ValueWidget = ListWidget(this, vBoxAnchor, olc::vi2d(ScoreValueWidth, ScoreValueHeight), values);
			bIsSelectingScore = true;
			
			break;

		case Brelan:
			TextEntryEnable(true);
			m_pCurrentWritingPlayer = m_GameState.arrPlayers[m_CurrentRoundState.idxCurrentPlayer];
			vEntryAnchor = olc::vi2d(m_vMainAnchor.x + ScoreValueWidth * (m_vHoveredCell.x + 1) + LiteralOffset.x, m_vMainAnchor.y + ScoreValueHeight * (m_vHoveredCell.y) + LiteralOffset.y);
			break;

		case Petite_Suite:
		case Grande_Suite:
			vBoxSize = olc::vi2d(ScoreValueWidth, ScoreValueHeight * 3);
			values.emplace_back(std::to_string(0));
			values.emplace_back(std::to_string(25));

			ValueWidget = ListWidget(this, vBoxAnchor, olc::vi2d(ScoreValueWidth, ScoreValueHeight), values);
			bIsSelectingScore = true;
			break;

		case Full:
			vBoxSize = olc::vi2d(ScoreValueWidth, ScoreValueHeight * 3);
			values.emplace_back(std::to_string(0));
			values.emplace_back(std::to_string(30));

			ValueWidget = ListWidget(this, vBoxAnchor, olc::vi2d(ScoreValueWidth, ScoreValueHeight), values);
			bIsSelectingScore = true;
			break;
		case Carre:
			vBoxSize = olc::vi2d(ScoreValueWidth, ScoreValueHeight * 3);
			values.emplace_back(std::to_string(0));
			values.emplace_back(std::to_string(40));

			ValueWidget = ListWidget(this, vBoxAnchor, olc::vi2d(ScoreValueWidth, ScoreValueHeight), values);
			bIsSelectingScore = true;
			break;
		case Yams:
			vBoxSize = olc::vi2d(ScoreValueWidth, ScoreValueHeight * 3);
			values.emplace_back(std::to_string(0));
			values.emplace_back(std::to_string(50));

			ValueWidget = ListWidget(this, vBoxAnchor, olc::vi2d(ScoreValueWidth, ScoreValueHeight), values);
			bIsSelectingScore = true;
			break;

		default: break;
	}

}

void YamScoreBoard::Reset()
{
	//First player is always here
	m_GameState = GameState();
	m_CurrentRoundState = RoundState();

	m_GameState.AddPlayer();
	m_CurrentRoundState.gameState = &m_GameState;
}

int YamScoreBoard::ComputeGlobalBestPlayerIdx()
{
	uint64_t iBestScore = 0;
	uint64_t iCurrentScore = 0;
	int iBestPlayer = 0;
	for (int i = 0; i < m_GameState.arrPlayers.size(); ++i)
	{
		iCurrentScore = bIsViewingScores ? m_GameState.arrPlayers[i]->GetTotalScore() + ComputeTT(m_GameState.arrPlayers[i]) : m_GameState.arrPlayers[i]->GetTotalScore();
		if (iCurrentScore > iBestScore)
		{
			iBestPlayer = i;
			iBestScore = iCurrentScore;
		}
		else if (iCurrentScore == iBestScore)
		{
			if (m_GameState.arrPlayers[i]->GetVictoryCount() > m_GameState.arrPlayers[iBestPlayer]->GetVictoryCount())
			{
				iBestPlayer = i;
				iBestScore = iCurrentScore;
			}
		}
	}
	return iBestPlayer;
}

void YamScoreBoard::NextPlayerTurn()
{
	//Dump buffer in player score (if one has been filled)
	if (m_CurrentRoundState.bufferScore.first != Name)
	{
		m_GameState.arrPlayers[m_CurrentRoundState.idxCurrentPlayer]->GetScoreState()[m_CurrentRoundState.bufferScore.first].first = true;
		m_GameState.arrPlayers[m_CurrentRoundState.idxCurrentPlayer]->GetScoreState()[m_CurrentRoundState.bufferScore.first].second = m_CurrentRoundState.bufferScore.second;
		m_CurrentRoundState.NextPlayerTurn();
	}
}


void YamScoreBoard::DrawScoreColumn(olc::vi2d& vPos)
{
	uint8_t iRowCounter = 0;


	for (uint8_t e = EValues::As; e != EValues::Brelan; e++)
	{
		if (bIsSelectingScore && e == eCurrentWritingValue)
		{
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), olc::WHITE);
			DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), mapScoreLiterals[static_cast<EValues>(e)],olc::BLACK, LiteralSize);
		}
		else
		{
			DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
			DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), mapScoreLiterals[static_cast<EValues>(e)], olc::WHITE, LiteralSize);
		}
		iRowCounter++;
	}

	if(m_RenderState.bShowTotals)
	{
		
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), strST1, olc::DARK_GREY, LiteralSize);
		iRowCounter++;

		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), strBonus, olc::DARK_MAGENTA, LiteralSize);
		iRowCounter++;
	}
	else
	{
		iRowCounter += 2;
	}

	for (uint8_t e = EValues::Brelan; e != EValues::Name; e++)
	{
		if (bIsSelectingScore && e == eCurrentWritingValue)
		{
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), olc::WHITE);
			DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), mapScoreLiterals[static_cast<EValues>(e)], olc::BLACK, LiteralSize);
		}
		else
		{
			DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
			DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), mapScoreLiterals[static_cast<EValues>(e)], olc::WHITE, LiteralSize);
		}
		iRowCounter++;
	}

	if(m_RenderState.bShowTotals)
	{
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), strST2, olc::DARK_GREY, LiteralSize);
		iRowCounter++;

		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), strTT, olc::GREEN, LiteralSize);
		iRowCounter++;
	}

}

void YamScoreBoard::DrawPlayerColumn(int idxPlayer, olc::vi2d& vPos, bool bIsCurrentPlayer)
{
	olc::Pixel nameColor = olc::VERY_DARK_YELLOW;
	olc::Pixel lineColor = olc::WHITE;
	olc::Pixel backgroundColor = olc::VERY_DARK_CYAN;
	olc::Pixel valueColor = olc::WHITE;
	olc::Pixel stColor = olc::DARK_GREY;
	olc::Pixel bonusColor = olc::DARK_MAGENTA;
	olc::Pixel ttColor = olc::GREEN;
	olc::Pixel hoveredColor = olc::GREY;

	if (bIsCurrentPlayer)
	{
		nameColor = olc::YELLOW;
		//lineColor	 = olc::YELLOW;
		valueColor = olc::BLACK;
		stColor = olc::GREY;
		bonusColor = olc::MAGENTA;
	}

	std::shared_ptr<Player> pPlayer = m_GameState.arrPlayers[idxPlayer];

	uint8_t iRowCounter = 0;
	uint8_t iHoveredRow = (GetMousePos().y - vPos.y) / ScoreValueHeight;

	if (bIsCurrentPlayer)
		FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), backgroundColor);
	DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), pPlayer->GetName(), nameColor, LiteralSize);
	iRowCounter++;

	std::map<EValues, uint8_t> mapHints;
	bool bBonusReachable = false;
	if (bIsCurrentPlayer && m_RenderState.bSHowHints)
	{
		bBonusReachable= ComputeHints(mapHints, idxPlayer);
	}

	for (uint8_t e = EValues::As; e != EValues::Brelan; e++)
	{
		if (bIsCurrentPlayer)
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), (m_vHoveredCell.x == idxPlayer && iHoveredRow == iRowCounter && !pPlayer->GetScoreState()[static_cast<EValues>(e)].first) ? hoveredColor : backgroundColor);
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));

		if (pPlayer->GetScoreState()[static_cast<EValues>(e)].first)
			DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(pPlayer->GetScoreState()[static_cast<EValues>(e)].second), valueColor, LiteralSize);
		else
		{
			if (bIsCurrentPlayer && m_RenderState.bSHowHints && mapHints.contains(static_cast<EValues>(e)))
			{
				if (bBonusReachable)
				{

					olc::Pixel color;

					switch (mapHints[static_cast<EValues>(e)])
					{
					case 0:
					case 1:
						color = olc::MAGENTA;
						break;
					case 2:
					case 3:
						color = olc::DARK_MAGENTA;
						break;
					case 4:
					case 5:
						color = olc::VERY_DARK_MAGENTA;
						break;
					case 6:
						color = olc::VERY_DARK_RED;
						break;
					default:
						break;
					}

					DrawString(vPos + olc::vi2d(ScoreValueWidth - 4 * LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(mapHints[static_cast<EValues>(e)] * (e +1)), color, LiteralSize);
				}
				else
				{
					DrawString(vPos + olc::vi2d(ScoreValueWidth - 4 * LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), "--", olc::BLACK, LiteralSize);
				}
			}
			
		}
		
		iRowCounter++;
	}

	if (m_RenderState.bShowTotals)
	{
		if (bIsCurrentPlayer)
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), backgroundColor);
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(ComputeST1(pPlayer)), stColor, LiteralSize);
		iRowCounter++;

		if (bIsCurrentPlayer)
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), backgroundColor);
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		uint8_t iBonusValue = HasBonus(pPlayer) ? 35 : 0;
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(iBonusValue), bonusColor, LiteralSize);
		iRowCounter++;
	}
	else
	{
		iRowCounter += 2;
	}


	for (uint8_t e = EValues::Brelan; e != EValues::Name; e++)
	{
		if (bIsCurrentPlayer)
		{
			if (e == eCurrentWritingValue && IsTextEntryEnabled())
				FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), olc::BLACK);
			else
				FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), (m_vHoveredCell.x == idxPlayer && iHoveredRow == iRowCounter && !pPlayer->GetScoreState()[static_cast<EValues>(e)].first) ? hoveredColor : backgroundColor);

		}
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		if (pPlayer->GetScoreState()[static_cast<EValues>(e)].first)
			DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(pPlayer->GetScoreState()[static_cast<EValues>(e)].second), valueColor, LiteralSize);
		iRowCounter++;
	}

	if (m_RenderState.bShowTotals)
	{
		if (bIsCurrentPlayer)
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), backgroundColor);
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(ComputeST2(pPlayer)), stColor, LiteralSize);
		iRowCounter++;

		if (bIsCurrentPlayer)
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), backgroundColor);
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(ComputeTT(pPlayer)), ttColor, LiteralSize);
		iRowCounter++;
	}

	if (bIsCurrentPlayer && m_vHoveredCell.x == idxPlayer && !bIsSelectingScore && !IsTextEntryEnabled() && GetMouse(0).bReleased)
	{
		//Avoid sub totals cells
		if (m_vHoveredCell.y != 6 && m_vHoveredCell.y != 7)
		{
			eCurrentWritingValue = m_vHoveredCell.y < 6 ? static_cast<EValues>(m_vHoveredCell.y) : static_cast<EValues>(m_vHoveredCell.y - 2);
			if (!pPlayer->GetScoreState()[eCurrentWritingValue].first)
			{
				ClickValueCell();
			}
		}
	}

	//Draw Temporary Score
	if (bIsCurrentPlayer && m_CurrentRoundState.bufferScore.first != Name && !(IsTextEntryEnabled() && m_CurrentRoundState.bufferScore.first == Brelan))/*currently writing brelan value */
	{
		int iRow = m_CurrentRoundState.bufferScore.first < Brelan ? m_CurrentRoundState.bufferScore.first + 1 : m_CurrentRoundState.bufferScore.first + 3;
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * iRow + LiteralOffset.y), std::to_string(m_CurrentRoundState.bufferScore.second), olc::YELLOW, LiteralSize);
	}

	if (bIsCurrentPlayer &&  IsTextEntryEnabled())
	{
		DrawString(vEntryAnchor, TextEntryGetString(), olc::WHITE, LiteralSize);
	}

	if (bIsCurrentPlayer && bIsSelectingScore)
	{
		ValueWidget.Draw();

		std::string strOut;

		if (ValueWidget.OnClicked(strOut))
		{
			bIsSelectingScore = false;

			if (!strOut.empty())
			{
				m_CurrentRoundState.bufferScore = std::make_pair(eCurrentWritingValue, std::atoi(strOut.c_str()));
			}
		}
	}

	
	
}

uint16_t YamScoreBoard::ComputeST1(std::shared_ptr<Player> pPlayer)
{
	auto& ScoreState = pPlayer->GetScoreState();

	uint8_t score = 0;
	for (uint8_t e = EValues::As; e != EValues::Brelan; e++)
	{
	
		score += ScoreState[static_cast<EValues>(e)].second;
	}
	return score;
}

uint64_t YamScoreBoard::ComputeTT(std::shared_ptr<Player> pPlayer)
{
	return ComputeST1(pPlayer) + ComputeST2(pPlayer) + (HasBonus(pPlayer) ? 35 : 0);
}

void YamScoreBoard::DrawBestPlayer()
{

	olc::vi2d vCrownPos = m_vMainAnchor + olc::vi2d((m_CurrentRoundState.idxBestPlayer + 1.5) * ScoreValueWidth - (m_pCrownSprite->width *.1 )/ 2, -3.5 * ScoreValueHeight);

	olc::Pixel color = m_CurrentRoundState.idxBestPlayer == m_CurrentRoundState.idxCurrentPlayer ? olc::GREEN : olc::WHITE;

	DrawDecal(vCrownPos, m_pCrownDecal, { 0.1,0.1 }, color);
}

bool YamScoreBoard::ComputeHints(std::map<EValues, uint8_t>& mapHints, int idxPlayer)
{
	mapHints.clear();
	std::shared_ptr<Player> pPlayer = m_GameState.arrPlayers[idxPlayer];

	int iSumAlreadyScored = 0;
	int iMostLikelyHint = 0;
	int iMaxReachable = 0;
	bool bIsStarted = false;

	for (uint8_t e = EValues::As; e != EValues::Brelan; e++)
	{
		if (pPlayer->GetScoreState()[static_cast<EValues>(e)].first)
		{
			iSumAlreadyScored += pPlayer->GetScoreState()[static_cast<EValues>(e)].second;
			bIsStarted = true;
		}
		else
		{
			//start with most likely combination 
			mapHints.emplace(std::make_pair(static_cast<EValues>(e), 3));
			iMostLikelyHint += (e+ 1)* 3;
			iMaxReachable += (e + 1) * 5;
		}
	}

	int iToObtain = 63 - iSumAlreadyScored;

	//Display Target Combination when no value cell has been scored yet 
	if (!bIsStarted)
	{
		return true;
	}

	//Easy Case, goal is already reached
	if (iToObtain <= 0)
	{

		for (auto it = mapHints.begin(); it != mapHints.end(); ++it)
		{
			it->second = 0;
		}
		/*for (auto pair : mapHints)
		{
			pair.second = 0;
		}* DOESNT WORK, CANT TELL WHY*/
		return true;
	}
	else
	{
		//63 points target is unreachable
		if (iMaxReachable < iToObtain)
		{
			return false;
		}
		//The only way to score bonus is to mark a Yams on every value
		else if (iMaxReachable == iToObtain)
		{
			for (auto it = mapHints.begin(); it != mapHints.end(); ++it)
			{
				it->second = 5;
			}
			return true;
		}

		//Score to obtain match 3xValue combination
		else if (iToObtain == iMostLikelyHint )
		{
			return true;
		}
		//Score to obtain is lesser than 3xValue combination
		else if (iToObtain < iMostLikelyHint)
		{
			bool bIsReached = false;
			while (!bIsReached)
			{
				int iSkippedValues = 0;
				//Start with higher value : if reducing one value doesn't make target score reachable, a decomposition with lower values (and thus a more likely combination) still might exist
				for (auto it = mapHints.rbegin(); it != mapHints.rend(); ++it)
				{
					int iN = it->second  - 1;
					int iReduce = it->first +1;
					if (iMostLikelyHint - iReduce < iToObtain)
					{
						iSkippedValues++;
					}
					else
					{
						iMostLikelyHint = iMostLikelyHint - iReduce;
						it->second = iN;
					}
				}

				bIsReached = iSkippedValues == mapHints.size();
			}

			return true;
		}
		else if (iToObtain > iMostLikelyHint)
		{
			bool bIsReached = false;
			while (!bIsReached)
			{
				for (auto it = mapHints.begin(); it != mapHints.end(); ++it)
				{
					int iN = it->second + 1;
					int iAugment = it->first +1 ;
					if (iMostLikelyHint + iAugment >= iToObtain)
					{
						bIsReached = true;
						it->second = iN;
					}
				}

				if (!bIsReached)
				{
					for (auto it = mapHints.begin(); it != mapHints.end(); ++it)
					{
						
						it->second++;
						iMostLikelyHint += it->second * (it->first + 1);
					}
				}
			}
			return true;
		}
	}
	return false;
}

bool YamScoreBoard::HasBonus(std::shared_ptr<Player> pPlayer)
{
	return ComputeST1(pPlayer) > 63;
}

uint16_t YamScoreBoard::ComputeST2(std::shared_ptr<Player> pPlayer)
{
	auto& ScoreState = pPlayer->GetScoreState();

	uint8_t score = 0;
	for (uint8_t e = EValues::Brelan; e != EValues::Name; e++)
	{
		score += ScoreState[static_cast<EValues>(e)].second;
	}
	return score;
}

bool YamScoreBoard::ListWidget::IsHovered() const
{
	if (m_ParentRenderer->GetMousePos().x > m_vPos.x && m_ParentRenderer->GetMousePos().x < m_vPos.x + m_vSize.x
		&& m_ParentRenderer->GetMousePos().y > m_vPos.y && m_ParentRenderer->GetMousePos().y < m_vPos.y + m_vSize.y * m_arrEntries.size())
	{
		return true;
	}

	return false;
}

bool YamScoreBoard::ListWidget::OnClicked(std::string& strOutEntry) const
{
	if (!IsHovered())
	{
		return false;
	}

	int idxEntry = (m_ParentRenderer->GetMousePos().y - m_vPos.y) / m_vSize.y;


	if (idxEntry < m_arrEntries.size() && m_ParentRenderer->GetMouse(0).bReleased)
	{
		strOutEntry = m_arrEntries[idxEntry];
		return true;
	}

	return false;
}

void YamScoreBoard::ListWidget::Draw()
{
	int idxEntry = -1;
	if (IsHovered())
	{
		idxEntry = (m_ParentRenderer->GetMousePos().y - m_vPos.y) / m_vSize.y;
	}

	for (int i = 0; i < m_arrEntries.size(); ++i)
	{
		bool bIsHovered = i == idxEntry;

		m_ParentRenderer->FillRect(olc::vi2d(m_vPos.x, m_vPos.y + i * m_vSize.y), m_vSize, bIsHovered ? cHoveredColor : cBackgroundColor);
		m_ParentRenderer->DrawString(m_vPos + olc::vi2d(LiteralOffset.x, i * m_vSize.y + LiteralOffset.y), m_arrEntries[i], bIsHovered ? cHoveredTextColor : cTextColor, iTextScale);
	}

	m_ParentRenderer->DrawRect(m_vPos, olc::vi2d(m_vSize.x, m_vSize.y * m_arrEntries.size()), cBorderColor);
}

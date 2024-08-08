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
		pPlayer->InitScoreState();
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
	default:
		break;
	}
	
	Clear(olc::VERY_DARK_BLUE);

	//Shortcuts (avoid conflict when typing
	if(!IsTextEntryEnabled())
	{
		if(GetKey(olc::T).bReleased)
		{
			m_RenderState.bShowTotals = !m_RenderState.bShowTotals;
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
	

	if (m_GameState.GetNumPlayers() > 1)
	{
		DrawBestPlayer();
	}

	if (!bIsSelectingScore && !IsTextEntryEnabled() && (GetKey(olc::SPACE).bReleased || NextButton.OnClicked()))
	{
		m_CurrentRoundState.NextPlayerTurn();
		

		if (m_CurrentRoundState.bIsComplete)
		{
			++m_GameState.idxRound;
			if (m_GameState.idxRound > m_GameState.GetNumRounds())
			{
				return false;
			}

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

		TurnCount.SetText("Tour :\n\n" + std::to_string(m_CurrentRoundState.idxTurn) + " / 12");
		RoundCount.SetText("Partie :\n\n" + std::to_string(m_GameState.idxRound) + " / " + std::to_string(m_GameState.GetNumRounds()));

		

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
			m_CurrentRoundState.bufferScore = std::make_pair(eCurrentWritingValue, std::atoi(strText.c_str()));
			//m_CurrentRoundState.NextPlayerTurn();
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

	bool bIsLaunched = LaunchButton.OnClicked();
	if(bIsLaunched && !IsTextEntryEnabled())
	{
		m_GameState.eCurrentPhase = Round;
	}
}

void YamScoreBoard::ClickValueCell()
{
	
	olc::vi2d vBoxAnchor;
	olc::vi2d vBoxSize;
	std::vector<std::string> values;

	switch (eCurrentWritingValue)
	{
		case As:
		case Deux:
		case Trois:
		case Quatre:
		case Cinq:
		case Six:

			
			vBoxAnchor = olc::vi2d(m_vMainAnchor.x + ScoreValueWidth * (m_vHoveredCell.x + 1), m_vMainAnchor.y + ScoreValueHeight * (m_vHoveredCell.y +1));
			vBoxSize = olc::vi2d(ScoreValueWidth, ScoreValueHeight * 7);

			
			values.emplace_back("");

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
			m_CurrentRoundState.bufferScore = std::make_pair(eCurrentWritingValue, 25);
			break;

		case Full:
			m_CurrentRoundState.bufferScore = std::make_pair(eCurrentWritingValue, 30);
			break;
		case Carre:
			m_CurrentRoundState.bufferScore = std::make_pair(eCurrentWritingValue, 40);
			break;
		case Yams:
			m_CurrentRoundState.bufferScore = std::make_pair(eCurrentWritingValue, 50);
			break;

		default: break;
	}

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
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), mapScoreLiterals[static_cast<EValues>(e)], olc::WHITE, LiteralSize);
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

	for (uint8_t e = EValues::As; e != EValues::Brelan; e++)
	{
		if (bIsCurrentPlayer)
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), (m_vHoveredCell.x == idxPlayer && iHoveredRow == iRowCounter && !pPlayer->GetScoreState()[static_cast<EValues>(e)].first) ? hoveredColor : backgroundColor);
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

	if (bIsCurrentPlayer && m_vHoveredCell.x == idxPlayer && !bIsSelectingScore && GetMouse(0).bReleased)
	{
		eCurrentWritingValue = m_vHoveredCell.y < 6 ? static_cast<EValues>(m_vHoveredCell.y) : static_cast<EValues>(m_vHoveredCell.y - 2);
		if (!pPlayer->GetScoreState()[eCurrentWritingValue].first)
		{
			ClickValueCell();
		}
	}

	//Draw Temporary Score
	if (bIsCurrentPlayer && m_CurrentRoundState.bufferScore.first != Name && !(IsTextEntryEnabled() && m_CurrentRoundState.bufferScore.first == Brelan))/*currently writing brelan value */
	{
		int iRow = m_CurrentRoundState.bufferScore.first < Brelan ? m_CurrentRoundState.bufferScore.first + 1 : m_CurrentRoundState.bufferScore.first + 3;
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * iRow + LiteralOffset.y), std::to_string(m_CurrentRoundState.bufferScore.second), olc::RED, LiteralSize);
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

uint16_t YamScoreBoard::ComputeTT(std::shared_ptr<Player> pPlayer)
{
	return ComputeST1(pPlayer) + ComputeST2(pPlayer) + (HasBonus(pPlayer) ? 35 : 0);
}

void YamScoreBoard::DrawBestPlayer()
{

	olc::vi2d vCrownPos = m_vMainAnchor + olc::vi2d((m_CurrentRoundState.idxBestPlayer + 1.5) * ScoreValueWidth - (m_pCrownSprite->width *.1 )/ 2, -3.5 * ScoreValueHeight);

	olc::Pixel color = m_CurrentRoundState.idxBestPlayer == m_CurrentRoundState.idxCurrentPlayer ? olc::GREEN : olc::WHITE;

	DrawDecal(vCrownPos, m_pCrownDecal, { 0.1,0.1 }, color);
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

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
	idxCurrentPlayer = (idxCurrentPlayer + 1) % gameState->GetNumPlayers();
}

bool YamScoreBoard::TextButtonWidget::IsHovered() const
{
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
	olc::vi2d vTextSize = m_ParentRenderer->GetTextSize(m_strText);
	olc::vi2d vScaledTextSize = vTextSize * iTextScale;

	//Scale down text if too big
	// float wRatio = vScaledTextSize.x / (float)m_vSize.x;
	// float hRatio = vScaledTextSize.y / (float)m_vSize.y;
	//
	// uint32_t wScale = iTextScale;
	// uint32_t hScale = iTextScale;
	//
	// if(wRatio > 1)
	// {
	// 	wScale = m_vSize.x / vTextSize.x;
	// }
	//
	// if(hRatio > 1)
	// {
	// 	hScale = m_vSize.y / vTextSize.y;
	// }
	//
	// iTextScale = std::min(wScale,hScale);
	// vScaledTextSize = vTextSize * iTextScale;

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

			if(GetKey(olc::L).bReleased)
				m_GameState.eCurrentPhase = Round;
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
		if (GetKey(olc::R).bReleased)
		{
			for (uint8_t iPlayers = 0; iPlayers < m_GameState.GetNumPlayers() ; ++iPlayers)
			{

				m_GameState.arrPlayers[iPlayers]->InitScoreState();
			}
		
		}

		if(GetKey(olc::T).bReleased)
		{
			m_RenderState.bShowTotals = !m_RenderState.bShowTotals;
		}
	}
	

	olc::vi2d vAnchor = olc::vi2d((ScreenWidth() / 2) - (m_GameState.GetNumPlayers() + 1) * ScoreValueWidth / 2, ScreenHeight() / 4);
	DrawScoreColumn(vAnchor);

	for (uint8_t iPlayers = 0; iPlayers < m_GameState.GetNumPlayers(); ++iPlayers)
	{
		
		std::shared_ptr<Player> pPlayer = m_GameState.arrPlayers[iPlayers];

		olc::vi2d vPlayerAnchor = vAnchor + olc::vi2d(ScoreValueWidth * (iPlayers + 1), -ScoreValueHeight);
		pPlayer->SetAnchor(vPlayerAnchor);
		DrawPlayerColumn(pPlayer, vPlayerAnchor, m_CurrentRoundState.idxCurrentPlayer == iPlayers);

		if (GetMousePos().x > vPlayerAnchor.x && GetMousePos().x < vPlayerAnchor.x + ScoreValueWidth && GetMousePos().y > (vPlayerAnchor.y)  && GetMousePos().y < vPlayerAnchor.y + ScoreValueHeight * 15)
		{
			uint8_t row = ((GetMouseY() - vPlayerAnchor.y) / ScoreValueHeight);
			int iTempY = vPlayerAnchor.y + row * ScoreValueHeight;

			//Dodge Total Cases

			if (row != 7 && row != 8)
			{
				olc::vi2d vHighlight = { vPlayerAnchor.x + 2, iTempY + 2 };
				DrawRect(vHighlight, olc::vi2d(ScoreValueWidth - 4, ScoreValueHeight - 4), olc::DARK_GREEN);
			}

			if (GetMouse(0).bReleased)
			{
				TextEntryEnable(true);
				vEntryAnchor = olc::vi2d(vPlayerAnchor.x + LiteralOffset.x, iTempY + ScoreValueHeight * .3);
				m_pCurrentWritingPlayer = pPlayer;

				if (row == 0)
				{
					eCurrentWritingValue = Name;
					pPlayer->SetName("");
				}
				else
				{
					eCurrentWritingValue = row < 7 ? static_cast<EValues>(row - 1) : static_cast<EValues>(row - 3);

				}
			}

		}

		
	}

	if (IsTextEntryEnabled())
	{
		DrawString(vEntryAnchor, TextEntryGetString(), olc::DARK_GREY, LiteralSize);
	}
	
	return true;
}

void YamScoreBoard::OnTextEntryComplete(const std::string& strText)
{
	
	if (m_pCurrentWritingPlayer)
	{
		if (eCurrentWritingValue == Name)
		{
			m_pCurrentWritingPlayer->SetName(strText);
		}
		else
		{
			m_pCurrentWritingPlayer->GetScoreState()[eCurrentWritingValue].first = true;
			m_pCurrentWritingPlayer->GetScoreState()[eCurrentWritingValue].second = std::atoi(strText.c_str());
			m_CurrentRoundState.NextPlayerTurn();
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
			if(GetMouse(0).bReleased)
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
	if(bIsLaunched)
	{
		m_GameState.eCurrentPhase = Round;
	}
}

void YamScoreBoard::DrawScoreColumn(olc::vi2d& vPos)
{
	uint8_t iRowCounter = 0;

	for (uint8_t e = EValues::As; e != EValues::Brelan; e++)
	{
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), mapScoreLiterals[static_cast<EValues>(e)], olc::WHITE, LiteralSize);
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
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)),strTT, olc::GREEN, LiteralSize);
		iRowCounter++;
	}

}

void YamScoreBoard::DrawPlayerColumn(std::shared_ptr<Player> pPlayer, olc::vi2d& vPos, bool bIsCurrentPlayer)
{
	olc::Pixel nameColor = olc::VERY_DARK_YELLOW;
	olc::Pixel lineColor = olc::WHITE;
	olc::Pixel backgroundColor = olc::VERY_DARK_CYAN;
	olc::Pixel valueColor = olc::WHITE;
	olc::Pixel stColor = olc::DARK_GREY;
	olc::Pixel bonusColor = olc::DARK_MAGENTA;
	olc::Pixel ttColor = olc::GREEN;

	if(bIsCurrentPlayer)
	{
		nameColor = olc::YELLOW;
		//lineColor = olc::YELLOW;
		valueColor = olc::BLACK;
		stColor = olc::GREY;
		bonusColor = olc::MAGENTA;
	}
	
	uint8_t iRowCounter = 0;

	if(bIsCurrentPlayer)
		FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), backgroundColor);
	DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), pPlayer->GetName(), nameColor, LiteralSize);
	iRowCounter++;

	for (uint8_t e = EValues::As; e != EValues::Brelan; e++)
	{
		if(bIsCurrentPlayer)
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), backgroundColor);
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));

		if(pPlayer->GetScoreState()[static_cast<EValues>(e)].first)
			DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(pPlayer->GetScoreState()[static_cast<EValues>(e)].second), valueColor, LiteralSize);
		iRowCounter++;
	}

	if(m_RenderState.bShowTotals)
	{
		if(bIsCurrentPlayer)
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), backgroundColor);
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(ComputeST1(pPlayer)), stColor, LiteralSize);
		iRowCounter++;

		if(bIsCurrentPlayer)
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
		if(bIsCurrentPlayer)
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), backgroundColor);
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		if (pPlayer->GetScoreState()[static_cast<EValues>(e)].first)
			DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(pPlayer->GetScoreState()[static_cast<EValues>(e)].second), valueColor, LiteralSize);
		iRowCounter++;
	}

	if(m_RenderState.bShowTotals)
	{
		if(bIsCurrentPlayer)
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), backgroundColor);
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(ComputeST2(pPlayer)), stColor, LiteralSize);
		iRowCounter++;

		if(bIsCurrentPlayer)
			FillRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight), backgroundColor);
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset.x, ScoreValueHeight * (iRowCounter + .3)), std::to_string(ComputeTT(pPlayer)), ttColor, LiteralSize);
		iRowCounter++;
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

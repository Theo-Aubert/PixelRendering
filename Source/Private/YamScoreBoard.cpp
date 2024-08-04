#include "../Public/YamScoreBoard.h"

void Player::InitScoreState()
{
	m_mapScoreSate.empty();

	for (uint8_t e = EValues::As; e != EValues::Name; e++)
	{
		m_mapScoreSate.emplace(std::make_pair(static_cast<EValues>(e),std::make_pair(false, 0)));
	}

}

bool YamScoreBoard::OnUserCreate()
{
	for (uint8_t p = 0 ; p < m_iNumPlayers; ++p)
	{
		auto player = std::make_shared<Player>();

		if (!player)
		{
			continue;
		}

		player->InitScoreState();
		m_arrPlayers.emplace_back(player);
	}

	return true;
}

bool YamScoreBoard::OnUserUpdate(float fElapsedTime)
{

	Clear(olc::VERY_DARK_BLUE);

	if (GetKey(olc::R).bReleased)
	{
		for (uint8_t iPlayers = 0; iPlayers < m_iNumPlayers; ++iPlayers)
		{

			m_arrPlayers[iPlayers]->InitScoreState();
		}
		
	}

	olc::vi2d vAnchor = olc::vi2d(ScreenWidth() / 4, ScreenHeight() / 4);
	DrawScoreColumn(vAnchor);

	for (uint8_t iPlayers = 0; iPlayers < m_iNumPlayers; ++iPlayers)
	{
		olc::vi2d vPlayerAnchor = vAnchor + olc::vi2d(ScoreValueWidth * (iPlayers + 1), -ScoreValueHeight);
		m_arrPlayers[iPlayers]->SetAnchor(vPlayerAnchor);
		DrawPlayerColumn(m_arrPlayers[iPlayers], vPlayerAnchor);

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
				vEntryAnchor = olc::vi2d(vPlayerAnchor.x + LiteralOffset, iTempY + ScoreValueHeight * .3);
				m_pCurrentWritingPlayer = m_arrPlayers[iPlayers];

				if (row == 0)
				{
					eCurrentWritingValue = Name;
					m_arrPlayers[iPlayers]->SetName("");
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

		}
	}
}

void YamScoreBoard::DrawScoreColumn(olc::vi2d& vPos)
{
	uint8_t iRowCounter = 0;

	for (uint8_t e = EValues::As; e != EValues::Brelan; e++)
	{
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), mapScoreLiterals[static_cast<EValues>(e)], olc::WHITE, LiteralSize);
		iRowCounter++;
	}

	DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), strST1, olc::DARK_GREY, LiteralSize);
	iRowCounter++;

	DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), strBonus, olc::DARK_MAGENTA, LiteralSize);
	iRowCounter++;

	for (uint8_t e = EValues::Brelan; e != EValues::Name; e++)
	{
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), mapScoreLiterals[static_cast<EValues>(e)], olc::WHITE, LiteralSize);
		iRowCounter++;
	}

	DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), strST2, olc::DARK_GREY, LiteralSize);
	iRowCounter++;

	DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)),strTT, olc::GREEN, LiteralSize);
	iRowCounter++;

}

void YamScoreBoard::DrawPlayerColumn(std::shared_ptr<Player> pPlayer, olc::vi2d& vPos)
{
	uint8_t iRowCounter = 0;

	DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), pPlayer->GetName(), olc::DARK_YELLOW, LiteralSize);
	iRowCounter++;

	for (uint8_t e = EValues::As; e != EValues::Brelan; e++)
	{
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));

		if(pPlayer->GetScoreState()[static_cast<EValues>(e)].first)
			DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), std::to_string(pPlayer->GetScoreState()[static_cast<EValues>(e)].second), olc::WHITE, LiteralSize);
		iRowCounter++;
	}

	DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), std::to_string(ComputeST1(pPlayer)), olc::DARK_GREY, LiteralSize);
	iRowCounter++;

	DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	uint8_t iBonusValue = HasBonus(pPlayer) ? 35 : 0;
	DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), std::to_string(iBonusValue), olc::DARK_MAGENTA, LiteralSize);
	iRowCounter++;

	for (uint8_t e = EValues::Brelan; e != EValues::Name; e++)
	{
		DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
		if (pPlayer->GetScoreState()[static_cast<EValues>(e)].first)
			DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), std::to_string(pPlayer->GetScoreState()[static_cast<EValues>(e)].second), olc::WHITE, LiteralSize);
		iRowCounter++;
	}

	DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), std::to_string(ComputeST2(pPlayer)), olc::DARK_GREY, LiteralSize);
	iRowCounter++;

	DrawRect(vPos + olc::vi2d(0, ScoreValueHeight * iRowCounter), olc::vi2d(ScoreValueWidth, ScoreValueHeight));
	DrawString(vPos + olc::vi2d(LiteralOffset, ScoreValueHeight * (iRowCounter + .3)), std::to_string(ComputeTT(pPlayer)), olc::GREEN, LiteralSize);
	iRowCounter++;
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

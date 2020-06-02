#include "ScoreHandler.h"

void ScoreHandler::readFromFile(std::string fileName)
{
	scores.clear();

	std::wifstream scoreFile;
	scoreFile.open(fileName); 
	std::wstring line;
	if (scoreFile.is_open())
	{
		while (getline(scoreFile, line))
		{
			std::wstring wsTmp(line.begin(), line.end());
			scores.push_back(stoi(line));
		}
	}
	sortScore();
}



void ScoreHandler::writeToFile(std::string fileName, int score, int time)
{
	std::ofstream scoreFile;
	scoreFile.open(fileName);

	finalScore = scoreMultiplier(score, time);
	scores.push_back(finalScore);
	sortScore();

	for (int i = std::min(10, (int)scores.size() - 1); i >= 0; i--)
	{
		scoreFile << scores.at(i) << std::endl;
	}

	
	scoreFile.close();
}

void ScoreHandler::sortScore()
{
	//for (int i = 0; i < scores.size(); i++)
	//{
		std::sort(scores.begin(), scores.end());
	//}
}


int ScoreHandler::scoreMultiplier(int score, int time)
{
	
	if (time == 0)
		finalScore = 0;
	else
		finalScore = (score / time) * multiplier;
	return finalScore;
}


#include "ScoreHandler.h"

void ScoreHandler::readFromFile(std::string fileName)
{
	
	std::wifstream scoreFile;
	scoreFile.open(fileName); 
	std::wstring line;
	if (scoreFile.is_open())
	{

		while (getline(scoreFile, line))
		{

			std::wstring wsTmp(line.begin(), line.end());
			scores.push_back(line);
		}
	}
	sortScore();
}

void ScoreHandler::writeToFile(std::string fileName, int score, int time)
{
	std::ofstream scoreFile;
	scoreFile.open(fileName,std::ios::app);
	
	finalScore = scoreMultiplier(score, time);
	scoreFile << finalScore << std::endl;;
	//scoreFile << std::endl;
	
	scoreFile.close();
}

void ScoreHandler::sortScore()
{
	for (int i = 1; i < scores.size(); i++)
	{
		std::sort(scores.begin(), scores.end());
	}
}


int ScoreHandler::scoreMultiplier(int score, int time)
{
	
		
	finalScore = (score / time) * multiplier;
	return finalScore;
}


#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <vector>
#include <DirectXMath.h>
class ScoreHandler
{
private:
	int multiplier = 10;
	int finalScore;
public:

	int amountOfRowsToShow = 10;
	std::vector<std::wstring> scores;
	void readFromFile(std::string fileName);
	void writeToFile(std::string fileName, int score, int time);
	void sortScore();
	int scoreMultiplier(int score, int time);
};


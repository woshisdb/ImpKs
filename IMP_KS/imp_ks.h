#pragma once
#include <iostream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <regex> 

#include "basicStruct.h"
using namespace std;

class ImpKs
{

public:
	void lexicalAnalysis(const string& code, vector<Statements>& statements);
	string getLabelCode(vector<Statements>& statements);
	string getFirstOrderLogic(const vector<Statements>& statements, vector<vector<FirstOrderLogical>>& fols);
	string getKripkeStructureJson(vector<vector<FirstOrderLogical>>& fols);
	void run(string code);


};

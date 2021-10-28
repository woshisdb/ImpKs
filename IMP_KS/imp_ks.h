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
	string getFirstOrderLogic(vector<Statements>& statements, vector<vector<FirstOrderLogical>>& lgss);
	void getKripkeStructure(vector<vector<FirstOrderLogical>>& lgss);
	string getDrawJson(vector<vector<FirstOrderLogical>>& lgss);
	void run(string code);


};

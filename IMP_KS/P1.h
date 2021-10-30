#pragma once
#include<iostream>
#include <cassert>
#include"basic_method.h"
#include"basicStruct.h"
using namespace std;
class P1
{
public:
	vector<string> parseCoProcesses(const string &text) {
		vector<string> processes;
		vector<string> processTags;  //代码段标签

		std::regex re("cobegin(.+)coend");
		std::smatch m;
		if (std::regex_search(text, m, re)) {
			string processTmp = string(m[1]);
			processTmp = remove(processTmp, " ");
			processTags = split(processTmp, "||");
		}


		//如果没有并行程序，则整个输入就是一个单线程执行的程序
		if (processTags.empty()) {
			processes.push_back(text);
			return processes;
		}

		//如果有并行程序，则解析出各个并行程序段
		for (auto& v : processTags) {
			cout << v;
			std::regex re(v + "::([^:]+)");
			std::smatch m;
			if (std::regex_search(text, m, re)) {
				string split = string(m[1]);
				//split = split.left(split.lastIndexOf(';') + 1);//
				split = split.substr(0, lastIndexOf(split, ';') + 1);//

				split = trimmed(split);
				//processes << split;
				processes.push_back(split);
				//return processes;
			}
		}

		return processes;
	}

	bool parseStatements(const string& input, Statements& statements);



	//解析 wait 语句
	Statement parseWait(const string& input) {
		Statement sm;
		string condition;
		std::regex re("wait\\((.+)\\)");
		std::smatch m;
		if (std::regex_search(input, m, re)) {
			condition = trimmed(string(m[1]));

			sm.type = StatementType::Wait;
			sm.condition = condition;
		}
		return sm;
	}



	//解析顺序语句
	Statements parseSequence(const string &input) {
		Statements sms;
		string inputTrimmed = trimmed(input);
		//vector<string> list = inputTrimmed.split(';', Qt::SkipEmptyParts);
		vector<string> list = split(inputTrimmed, ";");
		for (auto& v : list) {
			Statement sm;
			v = trimmed(v);
			//顺序语句里面可能会包含wait语句
			if (v.find("wait") != -1) {
				sm = parseWait(input);
			}
			else {
				sm.type = StatementType::Squence;
				sm.seqBody = v;
			}
			sms.push_back(sm);
		}
		return sms;
	}


	//解析 if 语句
	Statement parseIf(const string& input) {
		Statement sm;
		string inputNew = input;
		//inputNew.remove('\n');
		inputNew = remove(inputNew, "\n");
		string condition, ifBody, elseBody;

		if (input.find("else") != -1) {
			// QRegularExpression re("if(.+)then(.+)else(.+)endif;", QRegularExpression::MultilineOption);
			std::regex re("if(.+)then(.+)else(.+)endif;"); // TODO: multiline
			std::smatch m;
			assert(std::regex_search(inputNew, m, re));
			condition = trimmed(string(m[1]));
			ifBody = trimmed(string(m[2]));
			elseBody = trimmed(string(m[3]));
		}
		else {
			std::regex re("if(.+)then(.+)endif"); // TODO: multiline
			std::smatch m;
			assert(std::regex_search(inputNew, m, re));
			condition = trimmed(string(m[1]));
			ifBody = trimmed(string(m[2]));
		}

		sm.type = StatementType::If;
		sm.condition = condition;
		parseStatements(ifBody, sm.ifBody);
		parseStatements(elseBody, sm.elseBody);

		return sm;
	}

	//解析while语句
	Statement parseWhile(const string& input) {
		string inputNew = input;
		//inputNew.remove('\n');
		inputNew = remove(inputNew, "\n");
		Statement sm;
		string condition, body;

		std::regex re("while(.+)do(.+)end");
		std::smatch m;
		if (std::regex_search(inputNew, m, re)) {
			condition = trimmed(string(m[1]));
			body = trimmed(string(m[2]));
			sm.type = StatementType::While;
			sm.condition = condition;
			parseStatements(body, sm.whileBody);
		}

		return sm;
	}



};

bool P1::parseStatements(const string& input, Statements& statements) {
	int s = 0;
	int e = 0;
	while (s < input.length())
	{
		int pos1 = input.find("if", s);
		int pos2 = input.find("while", s);
		int pos;
		if (pos1 == -1)
		{
			pos = pos2;
		}
		else if(pos2==-1)
		{
			pos = pos1;
		}
		else
		{
			pos = min(pos1, pos2);
		}
		if (pos1==-1&&pos2==-1)//都没找到
		{
			e = input.length();
			string inputSplit = input.substr(s, e - s);
			auto states_tmp = parseSequence(inputSplit);
			statements.insert(statements.end(), states_tmp.begin(), states_tmp.end());
			s = e;
		}
		else {
			if (pos > s) {
				auto states_tmp = parseSequence(input.substr(s, pos - s));
				statements.insert(statements.end(), states_tmp.begin(), states_tmp.end());
			}
			s = pos;
			if (input.at(pos) == 'i') {
				e = input.find("endif");
				e += 6;
				statements.push_back(parseIf(input.substr(s, e - s)));
			}
			else {
				e = input.find("endwhile");
				e += 9;
				statements.push_back(parseWhile(input.substr(s, e - s)));
			}
			s = e;
		}
	}
	return true;
}
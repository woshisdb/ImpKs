#pragma once
#include<iostream>
#include <cassert>
#include"basic_method.h"
#include"basicStruct.h"
using namespace std;
class P1
{
public:
	//�����������Ĵ����
    //���֮��һ������Σ�˵��û�в������˻������߳�ִ��
	vector<string> parseCoProcesses(const string &text) {
		vector<string> processes;
		vector<string> processTags;  //����α�ǩ

		std::regex re("cobegin(.+)coend");
		std::smatch m;
		if (std::regex_search(text, m, re)) {
			string processTmp = string(m[1]);
			processTmp = remove(processTmp, " ");
			processTags = split(processTmp, "||");
		}


		//���û�в��г����������������һ�����߳�ִ�еĳ���
		if (processTags.empty()) {
			processes.push_back(text);//?
			return processes;//?
		}

		//����в��г�����������������г����
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



	//���� wait ���
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



	//����˳�����
	Statements parseSequence(const string &input) {
		Statements sms;
		string inputTrimmed = trimmed(input);
		//vector<string> list = inputTrimmed.split(';', Qt::SkipEmptyParts);
		vector<string> list = split(inputTrimmed, ";");
		for (auto& v : list) {
			Statement sm;
			v = trimmed(v);
			//˳�����������ܻ����wait���
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


	//���� if ���
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

	//����while���
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
		//int pos = input.indexOf(QRegularExpression("(if|while)"), s);
		//int pos = input.find(QRegularExpression("(if|while)"), s);
		int pos = input.find("if", s);
		if (pos == -1) {
			pos = input.find("while", s);
		}
		if (-1 == pos) {
			e = input.length();
			string inputSplit = input.substr(s, e - s);
			auto states_tmp = parseSequence(inputSplit);
			statements.insert(statements.end(), states_tmp.begin(), states_tmp.end());
			s = e;
		}
		else {
			if (pos > s) {
				//statements. merge( parseSequence(input.mid(s, pos - s)) );
				auto states_tmp = parseSequence(input.substr(s, pos - s));
				statements.insert(statements.end(), states_tmp.begin(), states_tmp.end());
			}
			s = pos;
			if (input.at(pos) == 'i') {
				//e = input.indexOf("endif");
				e = input.find("endif");
				e += 6;
				statements.push_back(parseIf(input.substr(s, e - s)));
			}
			else {
				//e = input.indexOf("endwhile");
				e = input.find("endwhile");
				e += 9;
				statements.push_back(parseWhile(input.substr(s, e - s)));
			}
			s = e;
		}
	}
	return true;
}
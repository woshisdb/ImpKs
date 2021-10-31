#pragma once
#include<iostream>
#include <cassert>
#include"basic_method.h"
#include"basicStruct.h"
#include<stack>
using namespace std;
class P1
{
public:
	vector<int> finds(string &now, string aim)
	{
		vector<int> res;
		string rep = "";
		for (int i = 0; i < aim.length(); i++)
		{
			rep += ".";
		}
		for (int i = 0; i < now.length(); i++)
		{
			int temp = now.find(aim, i);
			if (temp != -1)
			{
				now.replace(temp, aim.length(), rep);
				res.push_back(temp);
			}
		}
		return res;
	}
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
				split = split.substr(0, lastIndexOf(split, ';') + 1);//

				split = trimmed(split);
				processes.push_back(split);
			}
		}

		return processes;
	}

	bool parseStatements(const string input, Statements& statements);



	//解析 wait 语句
	Statement parseWait(const string input) {
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
	Statements parseSequence(const string input) {
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
	int modif(string nnw)
	{
		 vector<int> r=finds(nnw,"endif");
		 vector<int> l = finds(nnw, "if");
		 stack<int> s;
		 int u = 0, v = 0;
		 do
		 {
			 if (u < l.size())//如果数值
			 {
				 if (l[u] < r[v])
				 {
					 s.push(l[u]);
					 u++;
				 }
				 else
				 {
					 int templ = s.top();
					 s.pop();
					 int tempr = r[v];
					 string uv = "";
					 for (int i = 0; i < tempr - templ; i++)
					 {
						 uv += ".";
					 }
					 nnw.replace(templ,tempr-templ,uv);
					 v++;
				 }
			 }
			 else
			 {
				 if (s.empty())
					 break;
				 int templ = s.top();
				 s.pop();
				 int tempr = r[v];
				 string uv = "";
				 for (int i = 0; i < tempr - templ; i++)
				 {
					 uv += ".";
				 }
				 nnw.replace(templ, tempr - templ, uv);
				 v++;
			 }

		 } while (!s.empty());
		 return nnw.find("else");
	}

	//解析 if 语句
	Statement parseIf(const string input) {
		Statement sm;
		string inputNew = input;
		//inputNew.remove('\n');
		inputNew = remove(inputNew, "\n");
		string condition, ifBody, elseBody;
		/*
		if (input.find("else") != -1) {///////////////////不对啊？？？？？？？？？？？？？？？？？？？
			std::regex re("if(.+?)then(.+)else(.+)endif;"); // TODO: multiline
			std::smatch m;
			assert(std::regex_search(inputNew, m, re));
			condition = trimmed(string(m[1]));
			ifBody = trimmed(string(m[2]));
			elseBody = trimmed(string(m[3]));
		}
		*/
		if (input.find("else") != -1) {///////////////////不对啊？？？？？？？？？？？？？？？？？？？
			std::regex re("if(.+?)then(.+)endif;"); // TODO: multiline
			std::smatch m;
			assert(std::regex_search(inputNew, m, re));
			condition = trimmed(string(m[1]));
			/////将m[2]中找else
			string nnw = trimmed(string(m[2]));//输出所有的结果
			int els=modif(nnw);
			if (els == -1)
			{
				ifBody = nnw;
			}
			else
			{
				string t1 = nnw.substr(0, els);
				string t2 = nnw.substr(els + 4, nnw.length() - (els + 4));
				ifBody = trimmed(t1);
				elseBody = trimmed(t2);
			}
		}
		else {
			std::regex re("if(.+?)then(.+)endif"); // TODO: multiline
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
	Statement parseWhile(const string input) {
		string inputNew = input;
		//inputNew.remove('\n');
		inputNew = remove(inputNew, "\n");
		Statement sm;
		string condition, body;

		std::regex re("while(.+?)do(.+)end");
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


bool P1::parseStatements(string input, Statements& statements) {
	if (input == "")
		return true;
	int pos1 = input.find("if");
	int pos2 = input.find("while");
	int pos;
	if (pos1 == -1)
	{
		pos = pos2;
	}
	else if (pos2 == -1)
	{
		pos = pos1;
	}
	else
	{
		pos = min(pos1, pos2);
	}
	if (pos1 == -1 && pos2 == -1)//都没有找到while和endwhile
	{
		auto states_tmp = parseSequence(input);
		statements.insert(statements.end(), states_tmp.begin(), states_tmp.end());
	}
	else
	{
		if (pos != 0)
		{
			string rst = input.substr(0, pos);
			auto states_temps = parseSequence(rst);//前面仍然存在公式
		}
		string bl, el;
		if (input.at(pos) == 'i')//找到if了
		{
			bl = "if";
			el = "endif";
		}
		else//找到while了
		{
			bl = "while";
			el = "endwhile";
		}
		stack<int> aim;
		string see = input.substr(pos,input.length()-pos);//从头开始匹配
		vector<int>r = finds(see, el);
		vector<int>l= finds(see,bl);
		aim.push(l[0]);
		int li=1;
		int ri=0;
		while (!aim.empty())
		{
			if (li < l.size()&&l[li] < r[ri])
			{
				aim.push(l[li]);
				li++;
			}
			else
			{
				aim.pop();
				if (aim.empty())
				{
					string sev= input.substr(pos, input.length() - pos);
					if (bl.at(0) == 'i')
					{
						string u1 = sev.substr(0, r[ri] + 6);
						string u2 = sev.substr(r[ri] + 6, sev.length() - (r[ri] + 6));
						statements.push_back(parseIf(u1));
						parseStatements(u2, statements);
					}
					else
					{
						string u1 = sev.substr(0, r[ri] + 9);
						string u2 = sev.substr(r[ri] + 9, sev.length() - (r[ri] + 9));
						statements.push_back(parseWhile(u1));
						parseStatements(u2, statements);
					}
					//找到了
					return true;
				}
				else
				{
					ri++;
				}
			}
		}

	}
	return true;
}
#pragma once
#include<iostream>
#include"basic_method.h"
using namespace std;
class P1
{
public:
	vector<string> parseCoProcesses(const string &text) {
		vector<string> processes;
		vector<string> processTags;  //����α�ǩ

		std::regex re("cobegin(.+)coend");
		std::smatch m;
		if (std::regex_search(text, m, re)) {
			string processTmp = string(m[1]);
			//processTmp.remove(' ');
			processTmp = remove(processTmp, " ");
			//processTags = processTmp.split("||");
			processTags = split(processTmp, "||");
		}


		//���û�в��г����������������һ�����߳�ִ�еĳ���
		if (processTags.empty()) {
			//return processes << text;  //?
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
};


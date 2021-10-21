#pragma once
#include<iostream>
#include"basic_method.h"
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
			//processTmp.remove(' ');
			processTmp = remove(processTmp, " ");
			//processTags = processTmp.split("||");
			processTags = split(processTmp, "||");
		}


		//如果没有并行程序，则整个输入就是一个单线程执行的程序
		if (processTags.empty()) {
			//return processes << text;  //?
			processes.push_back(text);//?
			return processes;//?
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
};


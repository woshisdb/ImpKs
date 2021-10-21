#pragma once
#include<iostream>
#include <cassert>
#include"basic_method.h"
#include"basicStruct.h"
using namespace std;
class P2 {
	// 将所有语句打上标签
// 一阶逻辑公式需要使用语句标签
// 同时也可以用来展示带标签的代码段
public:
	void labledStatements(const char& prefix, int& index, Statements& sms) {
		if (sms.empty()) {
			return;
		}
		for (auto& v : sms) {
			v.label = formatString("%c%d", prefix, index++);
			if (v.type == StatementType::If) {
				labledStatements(prefix, index, v.ifBody);
				labledStatements(prefix, index, v.elseBody);
			}
			else if (v.type == StatementType::While) {
				labledStatements(prefix, index, v.whileBody);
			}
		}
	}

	//给所有语句打上标签
	void labledStatements(vector<Statements> &smss) {
		char prefix = 'A';
		for (auto& sms : smss) {
			int index = 0;
			labledStatements(prefix, index, sms);
			//prefix = char(prefix.unicode() + 1);
			prefix = char(prefix + 1);
		}
	}

	void statementToList(const Statements &sms, vector<string> &lis, string &space) {//?
		if (sms.empty())
			return;

		string SpaceNew = space + "    ";
		vector<string> ls;
		for (const auto& v : sms) {
			if (v.type == StatementType::Squence) {
				lis.push_back(v.label + ": " + space + v.seqBody + ';');
				//lis << v.label + ": " + space + v.seqBody + ';';
			}
			else if (v.type == StatementType::Wait) {
				lis.push_back(v.label + ": " + space + "wait (" + v.condition + ");");
				//lis << v.label + ": " + space + "wait (" + v.condition + ");";
			}
			else if (v.type == StatementType::If) {
				lis.push_back(v.label + ": " + space + "if " + v.condition + " then");
				//lis << v.label + ": " + space  + "if " + v.condition + " then";
				statementToList(v.ifBody, lis, SpaceNew);
				lis.push_back(space + "   else");
				//lis <<  space + "   else";
				statementToList(v.elseBody, lis, SpaceNew);
				lis.push_back(space + "   endif");
				//lis << space + "   endif";
			}
			else if (v.type == StatementType::While) {
				lis.push_back(v.label + ": " + space + "while " + v.condition + " do");
				//lis << v.label +": " + space + "while " + v.condition + " do";
				statementToList(v.whileBody, lis, SpaceNew);
				lis.push_back(space + "   endwhile;");
				//lis << space + "   endwhile;";
			}
		}
	}

	string out_result(vector<Statements> &statements)
	{//输出带标签的代码

		string label_code;
		label_code.append("Labeled function:\n");
		//ui.outputEdit->append("Labeled function:\n");
		for (const auto& v : statements) {
			vector<string> list;
			string space;
			statementToList(v, list, space);
			//添加一个结束标签
			string prefix = list[0].substr(0, 1);
			list.push_back(prefix + "E:");
			//ui.outputEdit->append(list.join('\n'));
			label_code.append(jointList(list, "\n"));

			label_code.append("\n\n");
		}
		cout << "\n第二步结果：打标签处理后的程序" << endl;
		cout << label_code << endl;
		return label_code;
	}
};
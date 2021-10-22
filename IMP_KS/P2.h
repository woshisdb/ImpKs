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
	int dfs_to_label(Statements &progress,string label,int &foot)
	{
		if (progress.empty())
		{
			return -1;
		}
		else
		{
			for (int i=0;i<progress.size();i++)
			{
				progress[i].label = label + std::to_string(++foot);
				if (!progress[i].ifBody.empty())
				{
					dfs_to_label(progress[i].ifBody,label,++foot);
					dfs_to_label(progress[i].elseBody, label, ++foot);
				}
				if (!progress[i].whileBody.empty())
				{
					dfs_to_label(progress[i].whileBody, label, ++foot);
				}
			}
		}
	}

	void createlabel(vector<Statements> &progress)//所有进程打上标签
	{
		for (int i = 0;i < progress.size();i++)
		{
			string n_label = "L"+ std::to_string(i);
			n_label += "_";
			int root = 0;
			dfs_to_label(progress[i],n_label,root);

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
				statementToList(v.whileBody, lis, SpaceNew);
				lis.push_back(space + "   endwhile;");
				//lis << space + "   endwhile;";
			}
		}
	}


	void dfs_show(Statements progress, string &res,string spac)
	{
		if (progress.empty())
			return;
		for (const auto& v : progress)
		{
			if (v.type == StatementType::Squence) {
				res += v.label + ":" + spac + v.seqBody + ";\n";
			}
			else if (v.type == StatementType::Wait) {
				res += v.label + ": " + spac + "wait (" + v.condition + ");\n";
			}
			else if (v.type == StatementType::If) {
				res += v.label + ": " + "if " + v.condition + " then\n";
				dfs_show(v.ifBody, res, spac+"  ");
				res += "     "+spac+"else\n";
				dfs_show(v.elseBody, res,spac+"  ");
				res += "     " +spac+"endif\n";
			}
			else if (v.type == StatementType::While) {
				res += v.label + ": " +spac+ "while " + v.condition + " do\n";
				dfs_show(v.whileBody,res,spac+"    ");
				res += "     "+spac+"endwhile;\n";
			}
		}
	}
	void show_codes(vector<Statements> &progress,string &res)
	{
		res += "输出打标签的函数：\n";
		for (int i=0;i<progress.size();i++)
		{
			res = res + "P" + to_string(i)+":\n";
			dfs_show(progress[i],res,"");
			res += "L" + to_string(i)+ "E:\n";
			res += "\n";
		}
	}
};
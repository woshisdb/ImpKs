#pragma once
#include<iostream>
#include <cassert>
#include"basic_method.h"
#include"basicStruct.h"
using namespace std;
class P3 {
public:

	//将关联语句转换为逻辑公式
	static FirstOrderLogical toFormula(const Statement& pre, const Statement& post) {
		FirstOrderLogical lg;
		lg.preLable = pre.label;
		lg.postLable = post.label;
		lg.condition = pre.condition;
		lg.opr = pre.seqBody;
		lg.vars = pre.vars;
		return lg;
	}

	//将所有语句转换为逻辑公式
	vector<FirstOrderLogical> toFormula(const Statements& statements, Statement &out) {
		if (statements.empty())
			return vector<FirstOrderLogical>();
		if (out.label.empty()) {
			//string prefix = statements.at(0).label.left(1);
			string prefix = (statements[0]).label.substr(0, 1);
			//split = split.substr(0, lastIndexOf(split, ';') + 1);//
			out.label = prefix + "E";
		}

		vector<FirstOrderLogical> list;
		for (int i = 0; i < statements.size(); ++i) {
			Statement postSm = out;
			//Statement sm=statements[i];
			Statement sm = statements[i];
			if (i + 1 < statements.size()) {
				//postSm = statements.at(i + 1);
				postSm = statements[i + 1];
			}

			if (sm.type == StatementType::If) {
				if (!sm.ifBody.empty()) {
					list.push_back(toFormula(sm, *(sm.ifBody.begin()))); //?
					auto list_tmp = toFormula(sm.ifBody, postSm);
					list.insert(list.end(), list_tmp.begin(), list_tmp.end());
				}
				if (!sm.elseBody.empty()) {
					sm.reversedCondition();
					list.push_back(toFormula(sm, *(sm.elseBody.begin())));
					auto list_tmp = toFormula(sm.elseBody, postSm);
					list.insert(list.end(), list_tmp.begin(), list_tmp.end());
				}
			}
			else if (sm.type == StatementType::While) {
				if (!sm.whileBody.empty()) {
					list.push_back(toFormula(sm, *(sm.whileBody.begin())));
					auto list_tmp = toFormula(sm.whileBody, sm);
					list.insert(list.end(), list_tmp.begin(), list_tmp.end());
				}
				sm.reversedCondition();
				list.push_back(toFormula(sm, postSm));
			}
			else if (sm.type == StatementType::Wait) {
				sm.reversedCondition();
				list.push_back(toFormula(sm, sm));
				sm.reversedCondition();
				list.push_back(toFormula(sm, postSm));
			}
			else {
				list.push_back(toFormula(sm, postSm));
			}
		}
		return list;
	}


	vector<vector<FirstOrderLogical>> to_logic(vector<Statements> statements,string &logic_code)
	{
		logic_code.append("First order logical formula:\n"); 
		//string logic_code;
		vector<vector<FirstOrderLogical>> lgss;
		bool hasPc = statements.size() > 1;
		for (int i = 0; i < statements.size(); ++i) {
			Statement out;
			vector<FirstOrderLogical> formulas = toFormula(statements[i], out);
			lgss.push_back(formulas);
			
			for (const auto& v : formulas) {
				if (hasPc) {
					string pc = formatString("pc%d", i);
					string formulaNew = v.toString();
					formulaNew.replace(formulaNew.find("pc"), 2, pc);
					logic_code.append(formatString("pc=%s and %s", pc.c_str(), formulaNew.c_str()));
				}
				else
					logic_code.append(v.toString());
				logic_code.append("\n");
				
				logic_code.append("{\n");
				logic_code.append("prelabel:"+v.preLable+"\n");
				logic_code.append("postlabel:" + v.postLable + "\n");
				logic_code.append("condition:" + v.condition + "\n");
				logic_code.append("opr:" + v.opr + "\n");
				for (const auto t : v.vars)
				{
					logic_code.append(t.type+"-------");
					logic_code.append(t.name + "=");
					logic_code.append(t.value+";\n");
				}
				logic_code.append("\n}\n");
				
			}
			
		}
		return lgss;
	}
};
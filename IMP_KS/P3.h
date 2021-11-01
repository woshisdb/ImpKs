#pragma once
#include<iostream>
#include <cassert>
#include"basic_method.h"
#include"basicStruct.h"
using namespace std;
class P3 {
public:

	string GetConcurrentFirstOrderLogicFormula(const vector<Statements>& statements, vector<vector<FirstOrderLogical>>& fols)
	{
		string logicFormula = "";
		string beg;
		beg = "pc=l ∧ pc'= ⊥";
		for (int i = 0; i < statements.size(); i++)
		{
			beg += " ∧ pc'" + to_string(i) + "=L" + to_string(i) + "_m";
		}
		beg += "\n";
		logicFormula.append(beg);

		for (int i = 0; i < statements.size(); ++i) {
			Statement out;
			vector<FirstOrderLogical> formulas = toFormula(statements[i], out);
			fols.push_back(formulas);

			for (const auto& v : formulas) {
				string pc = formatString("pc%d", i);
				string formulaNew = v.toString();
				formulaNew.replace(formulaNew.find("pc"), 2, pc);
				formulaNew.replace(formulaNew.find("pc'"), 2, pc);
				formulaNew.append(formatString(" ∧ SAME(PC\{%s})", pc.c_str()));
				logicFormula.append(formulaNew);
				logicFormula.append("\n");
			}
		}

		string end;
		end = "pc=⊥ ∧ pc'=l'";
		for (int i = 0; i < statements.size(); i++)
		{
			end.append(" ∧ pc" + to_string(i) + "=L" + to_string(i) + "E" + " ∧ pc" + to_string(i) + "'" + "=⊥");
		}
		end += "\n";
		logicFormula.append(end);

		return logicFormula;
	}

	string GetFirstOrderLogicFormula(const vector<Statements>& statements, vector<vector<FirstOrderLogical>>& fols)
	{
		string logicFormula = "";
		string beg;
		beg = "pc=L0_m ∧ pc'=L0_1\n";
		logicFormula.append(beg);

		for (int i = 0; i < statements.size(); ++i) {
			Statement out;
			vector<FirstOrderLogical> formulas = toFormula(statements[i], out);
			fols.push_back(formulas);

			for (const auto& v : formulas) {
				logicFormula.append(v.toString());
				logicFormula.append("\n");
			}
		}
		return logicFormula;
	}

private:
	//将关联语句转换为逻辑公式
	FirstOrderLogical toFormula(const Statement& pre, const Statement& post) {
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
			string prefix = (statements[0]).label.substr(0, 2);
			out.label = prefix + "E";
		}

		vector<FirstOrderLogical> list;
		for (int i = 0; i < statements.size(); ++i) {
			Statement postSm = out;
			Statement sm = statements[i];
			if (i + 1 < statements.size()) {
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

};
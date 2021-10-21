#pragma once
#include<iostream>
#include <cassert>
#include<regex>
#include"basic_method.h"
#include"basicStruct.h"
using namespace std;
class P4 {
public:
	void to_label(vector<vector<FirstOrderLogical>> &lgss,vector<string> &pcs, vector<pair<string, string>> &relations, vector<string> &lables, vector<FirstOrderLogical> &lastLgs, vector<string> &states, vector<KsR> &Rs,Variables &vars)
	{
		for (const auto& v : lgss) {
			if (lgss.size() > 1) {
				pcs.push_back("U");
			}
			else {
				pcs.push_back("");
			}
			lastLgs.push_back(FirstOrderLogical());
		}
	}
	void nextVars(const Variables& src, FirstOrderLogical &dst) {
		dst.vars.clear();
		dst.vars = src;
		pair<char, int> pir = dst.assign();

		if (!(pir.first == NULL))
			changeValue(dst.vars, pir.first, pir.second);
	}
	void changeValue(Variables &vars, char var, int value) {
		bool find = false;
		for (auto& v : vars) {
			if (v.name == var) {
				v.value = value;
				find = true;
				break;
			}
		}
		if (!find)
			//vars << Variable{ Variable::Int, var, value };
			vars.push_back(Variable{ Variable::Int, var, value });
	}
	//计算下一步的位置，这里需要考虑条件
	FirstOrderLogical nextStep(vector<FirstOrderLogical> &lgs, FirstOrderLogical &cur) {

		FirstOrderLogical lg;
		if (cur.isNull()) {
			lg.postLable = (*(lgs.begin())).preLable;
			nextVars(cur.vars, lg);
			return lg;
		}

		for (auto& v : lgs) {
			if (v.preLable == cur.postLable) {
				nextVars(cur.vars, v);
				if (v.isConditionOk())
					return v;
			}
		}
		return lg;
	}
	void createKsLables(vector<vector<FirstOrderLogical>>& lgss,
		//定义PC，指向当前执行的程序段
		const vector<string>& pcs,
		vector<pair<string, string>>& relations,
		vector<string>& labels,
		vector<FirstOrderLogical>& lastLgs,
		//当前变量值
		const Variables& vars,
		vector<string> &states,
		vector<KsR> &Rs, int deep = 0) {

		++deep;
		vector<string> tmp = pcs;
		vector<FirstOrderLogical> lastLgsTmp = lastLgs;
		for (int i = 0; i < pcs.size(); ++i) {
			//用户执行完之后恢复
			KsR oneRs;
			//string oldLabel = tmp.join(' ');
			string oldLabel = jointList(tmp, " ");//"""""""""""""""""""""""""""""""""""""
			oneRs.preLabel = oldLabel;

			//只包含空格，则认为是空
			string oldTmp = oldLabel;
			oldTmp = remove(oldTmp, " ");
			//oldTmp.remove(' ');
			if (oldTmp.empty())
				oldLabel.clear();

			//执行完要恢复到上一步的状态
			lastLgsTmp[i].vars = vars;
			FirstOrderLogical lastLg = lastLgsTmp[i];
			string lastArgsStr = lastLgsTmp[i].valueToString();
			oneRs.preVars = lastLgsTmp[i].vars;
			if (!oldLabel.empty() && !lastArgsStr.empty())
				oldLabel += ',' + lastArgsStr;

			if (oldLabel.find("A1") != -1 && !(oldLabel.find("B") != -1)) {
				int a = 10;
			}

			lastLgsTmp[i] = nextStep(lgss[i], lastLgsTmp[i]);

			tmp[i] = lastLgsTmp[i].postLable;
			Variables newVars = lastLgsTmp[i].vars;
			//string newLabel = tmp.join(' ');
			string newLabel = jointList(tmp, " ");

			//收集R变换
			oneRs.postLabel = newLabel;
			oneRs.postVars = lastLgsTmp[i].vars;
			oneRs.opr = lastLgsTmp[i].opr;
			Rs.push_back(oneRs);

			//收集状态S
			string oneState = lastLgsTmp[i].valueToString();

			if (!oneState.empty() && !is_contains_list(states, oneState)) {
				states.push_back(oneState);
			}

			if (!newLabel.empty() && !lastLgsTmp[i].valueToString().empty())
				newLabel += ',' + lastLgsTmp[i].valueToString();

			pair<string, string> r{ oldLabel, newLabel };
			if (is_contains_list(relations, r)) {
				tmp[i] = pcs[i];//改了
				lastLgsTmp[i] = lastLg;
				continue;
			}

			if (!oldLabel.empty() && !newLabel.empty()) {
				//relations << pair<string, string>{ oldLabel, newLabel };
				relations.push_back(pair<string, string>{ oldLabel, newLabel });
			}
			if (!is_contains_list(labels, oldLabel) && !oldLabel.empty()) {
				labels.push_back(oldLabel);
			}

			if (!is_contains_list(labels, newLabel) && !newLabel.empty()) {
				labels.push_back(newLabel);
			}
			createKsLables(lgss, tmp, relations, labels, lastLgsTmp, newVars, states, Rs, deep);
			tmp[i] = pcs[i];
			lastLgsTmp[i] = lastLg;
		}
	}
	void out_result(vector<string> lables, vector<KsR> Rs, string &state_code, string &state_rela)
	{
		//输出所有S状态
		//string state_code;//------------------------输出状态s
		//state_code.append("All States:\n");
		int index = 0;
		for (const auto& v : lables) {
			state_code.append(formatString("S%d:(%s)\n", index++, v.c_str()));

		}
		index = 0;
		for (const auto& v : Rs) {
			
			if (!v.toString().empty())
				state_rela.append(formatString("R%d:= %s\n", index++, v.toString().c_str()));
		}

	}
};
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
	//������һ����λ�ã�������Ҫ��������
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
		//����PC��ָ��ǰִ�еĳ����
		const vector<string>& pcs,
		vector<pair<string, string>>& relations,
		vector<string>& labels,
		vector<FirstOrderLogical>& lastLgs,
		//��ǰ����ֵ
		const Variables& vars,
		vector<string> &states,
		vector<KsR> &Rs, int deep = 0) {

		++deep;
		vector<string> tmp = pcs;
		vector<FirstOrderLogical> lastLgsTmp = lastLgs;
		for (int i = 0; i < pcs.size(); ++i) {
			//�û�ִ����֮��ָ�
			KsR oneRs;
			//string oldLabel = tmp.join(' ');
			string oldLabel = jointList(tmp, " ");//"""""""""""""""""""""""""""""""""""""
			oneRs.preLabel = oldLabel;

			//ֻ�����ո�����Ϊ�ǿ�
			string oldTmp = oldLabel;
			oldTmp = remove(oldTmp, " ");
			//oldTmp.remove(' ');
			if (oldTmp.empty())
				oldLabel.clear();

			//ִ����Ҫ�ָ�����һ����״̬
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

			//�ռ�R�任
			oneRs.postLabel = newLabel;
			oneRs.postVars = lastLgsTmp[i].vars;
			oneRs.opr = lastLgsTmp[i].opr;
			Rs.push_back(oneRs);

			//�ռ�״̬S
			string oneState = lastLgsTmp[i].valueToString();

			if (!oneState.empty() && !is_contains_list(states, oneState)) {
				states.push_back(oneState);
			}

			if (!newLabel.empty() && !lastLgsTmp[i].valueToString().empty())
				newLabel += ',' + lastLgsTmp[i].valueToString();

			pair<string, string> r{ oldLabel, newLabel };
			if (is_contains_list(relations, r)) {
				tmp[i] = pcs[i];//����
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
		//�������S״̬
		//string state_code;//------------------------���״̬s
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
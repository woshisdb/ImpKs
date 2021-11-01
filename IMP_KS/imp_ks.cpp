#define _CRT_SECURE_NO_WARNINGS
#include "imp_ks.h"
#include"P1.h"
#include"P2.h"
#include"P3.h"
#include"P4.h"
#include "basic_method.h"
#include <iostream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <memory>
#include <cstdio>
#include <regex> 
#include <fstream>

using namespace std;

typedef struct val{
	string u;
	string v;
};
typedef struct json_node{
	string all;
	vector<val> vals;
	string no;//pc值
	string to_string()
	{
		string res;
		for (const auto& i : vals)
		{
			res += ",";
			res += i.u + "=" + i.v;
		}
		return res;
	}
};
typedef struct edge {
	string u;
	string v;
};
json_node to_node(string node)
{
	json_node res;
	node=remove(node,")");
	node.erase(0,node.find('(')+1);
	//cout << node << endl;

	vector<string> temp = split(node,",");
	res.no = temp[0];
	for (int i = 1; i < temp.size(); i++)
	{
		val tv;
		vector<string> tt = split(temp[i],"=");
		tv.u = tt[0];
		tv.v = tt[1];
		res.vals.push_back(tv);
	}
	return res;
}
vector<json_node> analy(string head)
{
	vector<json_node> res;
	string now=head+"";
	regex reg("\n");
	sregex_token_iterator pos(now.begin(), now.end(), reg, -1);
	decltype(pos) end;
	for (; pos != end; ++pos)
	{
		json_node temp=to_node(pos->str());
		res.push_back(temp);
	}
	json_node U;
	U.no = "U";
	res.push_back(U);
	return res;
}
edge edg_ju(string ge)
{
	edge res; //node.erase(0, node.find('(') + 1);
	ge=ge.erase(0, ge.find('=') + 1);
	vector<string> temp = split(ge,"->");
	temp[0] = remove(temp[0], "(");
	temp[0] = remove(temp[0], ")");
	temp[1] = remove(temp[1], "(");
	temp[1] = remove(temp[1], ")");
	res.u = (split(temp[0], ",")[0]);
	res.u = res.u.erase(0, res.u.find("=")+1);
	res.v = (split(temp[1], ",")[0]);
	res.v = res.v.erase(0, res.v.find("=") + 1);
	return res;
}
vector<edge> judge(string e)
{
	vector<edge> res;
	vector<string> temp;
	temp = split(e,"\n");
	for (int i=0;i<temp.size();i++)
	{
		res.push_back( edg_ju(temp[i]) );
	}
	return res;
}


void draw_json(vector<json_node> h, vector<edge> eg)
{
	string res="{ \"class\": \"go.GraphLinksModel\",\"nodeKeyProperty\" : \"id\", \"nodeDataArray\": [  \n";
	for (int i = 0; i < h.size(); i++)
	{
		if (i != 0)
		{
			res += ",";
		}
		res =res+ " {\"id\": " +" \" "+ h[i].no+" \" " + "," + " \"text\" : \"" + h[i].no +h[i].to_string()+ " \"}"+"\n";
	}
	res += "],";
	res += " \"linkDataArray\": [  ";
	for (int i = 0; i < eg.size(); i++)
	{
		if (i != 0)
			res += ",";
		res =res+ "{ \"from\":"+" \" "  +eg[i].u + " \" " + ", \"to\":"+ " \" " +eg[i].v+ " \" " +"}\n";
	}
	res += "]\n}";
	cout << res;
}

void ImpKs::lexicalAnalysis(const string& code, vector<Statements>& statements)
{
	//解析出所有程序段
	P1 program_1;
	vector<string> processes = program_1.parseCoProcesses(code);

	for (int i = 0; i < processes.size(); i++) {
		cout << processes[i] << endl;
	}

	for (const auto& v : processes) {
		Statements tmp;//对每个程序p1,p2分别处理
		program_1.parseStatements(v, tmp);
		statements.push_back(tmp);
	}
}

string ImpKs::getLabelCode(vector<Statements>& statements)
{
	//给所有语句打上标签
	P2 program_2;
	program_2.createlabel(statements);
	string label_code;
	program_2.show_codes(statements, label_code);
	return label_code;
}

string ImpKs::getFirstOrderLogic(vector<Statements>& statements, vector<vector<FirstOrderLogical>>& fols)
{
	P3 program_3;
	string logic_code;
	fols = program_3.to_logic(statements, logic_code);
	return logic_code;
}



string ImpKs::getKripkeStructureJson(vector<vector<FirstOrderLogical>>& fols)
{
	P4 program_4;
	string gojs = program_4.change(fols);
	ofstream fout("./view/show_data.js");
	fout << gojs << endl;
	return gojs;
}


//开始执行程序
void ImpKs::run(string code)
{
	cout << "\n第零步：原始IMP程序" << endl;
	cout << code << endl;

	cout << "\n第一步：词法分析" << endl;
	vector<Statements> statements;
	lexicalAnalysis(code, statements);

	cout << "\n第二步：打标签" << endl;
	string label_code = getLabelCode(statements);
	cout << label_code << endl;

	cout << "\n第三步：一阶逻辑公式" << endl;
	vector<vector<FirstOrderLogical>> fols;
	string logic_code = getFirstOrderLogic(statements, fols);
	cout << logic_code << endl;

	cout << "\n第四步：创建Kripke Structure(json数据)" << endl;
	string gojs = getKripkeStructureJson(fols);
	cout << gojs << endl;

	cout << "\n第五步：浏览器自动打开显示状态图" << endl;
	string cmd = "start ./view/stateChart.html";
	const char* command = cmd.c_str();
	system(command);
}
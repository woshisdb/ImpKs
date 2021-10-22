#define _CRT_SECURE_NO_WARNINGS
#include "imp_ks.h"
#include"P1.h"
#include"P2.h"
#include"P3.h"
#include"P4.h"
#include "basic_method.h"
#include "basicStruct.h"
#include <iostream>
#include <cassert>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <memory>
#include <cstdio>
#include <regex> 

using namespace std;

const string g_input[] = {
R"(
a=1;)",

R"(a=0;
b=1;
if a < b then
    a=a+1;
else
    b=b+1;
endif;
a=0;
b=0;)",

R"(x=0;
y=0;
z=0;
x=y+1; 
z=z+2;
while y<2 do
    if x<y then 
        x=x+1; 
    else 
        y=y+1;
    endif;
endwhile;)",

R"(x=0;
y=2;
if x<y then
    while x<2 do
        x=x+1;
    endwhile;
else 
    y=y+1;
endif;)",

R"(cobegin P0||P1 coend;
P0::
t=0;
while true do
    wait(t==0);
    t=1; 
endwhile;

P1::
while true do
    wait(t==1);
    t=0; 
endwhile;)",
};

/**
* \b 一阶逻辑数据结构，可以从一阶逻辑生成KS结构
  \a preLable 前置标签
  \a postLable 后置标签
  \a condition 条件
  \a string opr 操作
  \a vector<Variable> vars;
*/
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

//开始执行程序
void ImpKs::onStart()
{
	string input = g_input[0];//输出结果？
	cout << "\n第零步结果：原始IMP程序" << endl;
	cout << input << endl;

	//解析出所有程序段
	P1 program_0;
	vector<string> processes =program_0.parseCoProcesses(input);
	cout << "\n第零步结果：输出解析程序段" << endl;
	for (int i = 0; i < processes.size(); i++)
	{
		cout << processes[i] << endl;
		cout << "-------------"<<endl;
	}
	vector<Statements> statements;
	for (const auto& v : processes) {
		Statements tmp;//对每个程序p1,p2分别处理
		program_0.parseStatements(v, tmp);
		statements.push_back(tmp);
	}

	//给所有语句打上标签
	P2 program_1;
	program_1.createlabel(statements);
	string label_code;
	program_1.show_codes(statements, label_code);
	//label_code= program_1.out_result(statements);
	cout << label_code << endl;

	//输出逻辑公式
	P3 program_2;
	vector<vector<FirstOrderLogical>> lgss;
	string logic_code;//-------------------------------输出逻辑公式-----------------
	lgss = program_2.to_logic(statements,logic_code);
	cout << "\n第三步结果：逻辑公式" << endl;
	cout << logic_code << endl;
	cout << "************************" << endl;

	P4 program3;

	vector<string> pcs;
	vector<pair<string, string>> relations;
	vector<string> lables;
	vector<FirstOrderLogical> lastLgs;
	vector<string> states;
	vector<KsR> Rs;
	Variables vars;
	program3.to_label(lgss,pcs,relations, lables,lastLgs,states, Rs,vars);
	program3.createKsLables(lgss, pcs, relations, lables, lastLgs, vars, states, Rs);


	string head;
	string eag;
	program3.out_result(lables,Rs,head,eag);
	cout << head << "\n" << eag;
	vector<json_node> h=analy(head);
	vector<edge> eg = judge(eag);
	draw_json(h,eg);
}
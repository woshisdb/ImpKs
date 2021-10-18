#include "imp_ks.h"

#include <iostream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <memory>
#include <cstdio>
// ������ʽ��
#include <regex> 

using namespace std;

template<class nump,class sea>
bool contains_list(nump l, sea aim)//����������Ƿ���aim
{
	bool has = false;
	typename nump::iterator it = l.begin();
	for (; it != l.end(); it++)
	{
		if (aim == *it)
		{
			has = true;
			break;
		}
	}
	return has;
}

string remove(string u, string v)
{
	int pos = u.find(v);
	while (pos != -1)
	{
		u.erase(pos, v.length());
		pos = u.find(v);
	}
	return u;
}

string jointList(list<string> l, string aim)
{
	string ret = *(l.begin());
	list<string>::iterator it = l.begin();
	for (it++; it != l.end(); it++)
	{
		ret += aim;
		ret += *(it);
	}
	return ret;
}
/*
template<class findbas>
findbas find(findbas u, int no)
{
	typename findbas::iterator it = u.begin();
	for (int i = 0; i < no; i++, it++)
	{}
	return *it;
}
*/
/*
string find(list<string> u, int no)
{
	list<string>::iterator it = u.begin();
	for (int i = 0; i < no; i++, it++)
	{
	}
	return *it;
}
list<FirstOrderLogical> find(list<list<FirstOrderLogical>> u, int no)
{
	list<list<FirstOrderLogical>>::iterator it = u.begin();
	for (int i = 0; i < no; i++, it++)
	{
	}
	return *it;
}
FirstOrderLogical find(list<FirstOrderLogical> u, int no)
{
	list<FirstOrderLogical>::iterator it = u.begin();
	for (int i = 0; i < no; i++, it++)
	{
	}
	return *it;
}
*/
template<class container>//һ��
typename container::iterator at(container &u, int no)
{
	typename container::iterator it = u.begin();
	for (int i = 0; i < no; i++, it++)
	{

	}
	return it;
}

int lastIndexOf(string u,char v)
{
	for (int i = u.length - 1; i >= 0; i--)
	{
		if (u[i] == v)
		{
			return i;
		}
	}
	return -1;//�����ҵ�
}

list<string> split(string str,string delim)
{
	list<string> res;
	if ("" == str) return res;
	//�Ƚ�Ҫ�и���ַ�����string����ת��Ϊchar*����  
	char * strs = new char[str.length() + 1]; //��Ҫ����  
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p) {
		string s = p; //�ָ�õ����ַ���ת��Ϊstring����  
		res.push_back(s); //����������  
		p = strtok(NULL, d);
	}
	return res;
}

template<typename ... Args>
static std::string formatString(const std::string &format, Args ... args)
{
	auto size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	std::unique_ptr<char[]> buf(new char[size]);
	std::snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
string trimmed(string origin)
{
	//printf("%d",origin.length());
	int len = 0;
	for (int i = 0; i < origin.length(); i++)
	{
		//cout<<origin[i]<<endl;
		if (origin[i] == '\t' || origin[i] == '\n' || origin[i] == '\v' || origin[i] == '\f' || origin[i] == '\r' || origin[i] == ' ')
		{
			len++;
		}
		else
		{
			break;
		}
	}
	origin.erase(0, len);
	len = 0;
	for (int i = origin.length() - 1; i >= 0; i--)
	{
		//cout<<origin[i]<<endl;
		if (origin[i] == '\t' || origin[i] == '\n' || origin[i] == '\v' || origin[i] == '\f' || origin[i] == '\r' || origin[i] == ' ')
		{
			len++;
		}
		else
		{
			break;
		}
	}
	origin.erase(origin.length() - len, len);
	return origin;
}

/*
* ֧�ֵ��﷨��
* 1.����ֵ���
* a=0;
* 
* 2. �������ʽ
* a=2-1 | 2+1 | 2*1
* 
* 3. �߼����ʽ
* a=true | false | b == c| b <= c| b and c | b or c | not b
* 
* 4. ͬ�����
* cobegin p1||p2 coend;
* 
* 5. ˳�����
* p1;p2
* 
* 6. if ���
* if b then
*	p0;
* else
*	p1;
* endif;
* 
* 7. while ���
* while b do
*	p;
* endwhile;
* 
* 8. wait ���
* wait (b);
* 
* �����Ķ���ֻ����Сд��ĸ
* ����ȡֵ��Χ[0,2]
* boolean ���ͱ���ȡֵ��Χ[0,1]
* 
* �������ȱ�ݣ�
* 1. ��֧�����������.
* 2. ��֧�ָ�������Ƕ��.
* 3. ������Ҫ��ʼֵ
*/


/**
* \b �����������ݣ���ֵ�Ĳ������������Կ��ٽ��в��ԡ�
* \a g_input ����ͨ���޸ĸ�ֵ����Ӹ���Ĳ�����������������
*  ��Ҫ����IMP�﷨����
*/
const string g_input[] = {
R"(a=0;
b=2;
while a<b do
    a=a+1;
endwhile;
a=2;
b=0;)",

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

static const int ModValue = 3;


/**
	\a StatementType ������ͣ�Ŀǰ֧��4�ַ���������ͣ�˳����䣬
	������䣬ѭ����䣬�ȴ���䡣
**/
enum class StatementType {
	Squence = 0,  //˳�����
	If,       //If ���
	While,    //While ���
	Wait      //wait ���
};


/**
	IMP��������
	֧��boolean �� int ����
*/
struct Variable
{
	bool operator==(const Variable& o) const {
		return name == o.name && value == o.value;
	}
	enum Type
	{
		Int = 0,
		Boolean
	};
	string toString(bool isPost = false) const {
		if ( !isPost )
			return formatString("%c=%d", name, value);
		else
			return formatString("%c'=%d", name, value);
	}
	Type  type;
	char name;
	int   value;
};

struct Statement;
using Statements = list<Statement>;
using Variables = list<Variable>;

/**
                                                                      
*/
struct Statement
{
	bool isNull() const {
		return condition.empty() && seqBody.empty();
	}
	void reversedCondition() {
		// �ַ�������
		if (condition.find("not")!=-1) {
			//condition.remove("not");
			condition=remove(condition,"not");
			//condition = condition.trimmed();
		}
		else {
			condition = formatString("not %s", condition);
		}
	}
	StatementType type;  //�������
	string label;       //����ǩ
	string condition;
	string seqBody;
	Statements ifBody;
	Statements elseBody;
	Statements whileBody;
	list<Variable>  vars;
};


/**
* \b һ���߼����ݽṹ�����Դ�һ���߼�����KS�ṹ
  \a preLable ǰ�ñ�ǩ
  \a postLable ���ñ�ǩ
  \a condition ����
  \a string opr ����
  \a list<Variable> vars;
*/
struct FirstOrderLogical
{
public:
	bool isNull() const {
		return preLable.empty() && postLable.empty();
	}

	static string findAssignVariable(const string& statement) {
		QRegularExpression re("(\\w+)\\s*=\\s*");
		QRegularExpressionMatch match = re.match(statement);
		if (match.hasMatch()) {
			string s = match.captured(1);
			return match.captured(1);
		}
		else {
			return string();
		}
	}

	string toString() const {
		if (!condition.empty()) {
			return formatString("pc=%s and pc'=%s and (%s) and SAME(V)", preLable, postLable, condition);
		}
		else {
			string tmp = formatString("pc=%s and pc'=%s and (%s)", preLable, postLable, opr);
			string var = findAssignVariable(opr);
			if (var.empty()) {
				tmp += " and SAME(V)";
			}
			else {
				tmp += formatString(" and SAME(V\\{%s})", var);
			}
			return tmp;
		}
	}

	string valueToString() const {
		string tmp;
		for (const auto &v : vars) {
			if (!tmp.empty()) {
				tmp += ",";
			}
			tmp += formatString("%c=%d", v.name, v.value);
		}
		return tmp;
	}

	pair<char, int> assign()  const {
		if (opr.empty() || !(opr.find('=') != -1)) {
			return pair<char, int>();
		}

		QRegularExpression re("(\\w)\\s*=\\s*(\\w)\\s*([\\*+-])\\s*(\\w)");
		QRegularExpressionMatch match = re.match(opr);
		if (match.hasMatch()) {
			char ret = match.captured(1).at(0);
			char left = match.captured(2).at(0);
			char midOpr = match.captured(3).at(0);
			char right = match.captured(4).at(0);

			int nLeft = getVarValue(left);
			int nRight = getVarValue(right);
			int nRet = 0;
			if (midOpr == '*') {
				nRet = nLeft * nRight % ModValue;
			}
			else if (midOpr == '+') {
				nRet = (nLeft + nRight) % ModValue;
			}
			else if (midOpr == '-') {
				nRet = (nLeft - nRight + 3) % ModValue;
			}
			
			return pair<char, int>(ret, nRet);
		}

		re.setPattern("(\\w+)\\s*=\\s*(\\d+)");
		match = re.match(opr);
		if (match.hasMatch()) {
			return pair<char, int>{ match.captured(1).at(0), match.captured(2).toInt()};
		}
		return pair<char, int>();
	}

	bool hasAssign() const {
		if (!condition.empty() || opr.empty()) {
			return false;
		}
		string var = findAssignVariable(opr);
		if (var.empty())
			return false;

		return true;
	}

	bool isConditionOk() const {
		if (condition.empty())
			return true;
		//if (0 == condition.compare("true", Qt::CaseInsensitive))
		if (condition== "true")
			return true;
		//if (0 == condition.compare("false", Qt::CaseInsensitive))
		if(condition== "false")
			return false;

		bool hasNot = false;
		string conditionNew = condition;
		if (conditionNew.find("not") != -1) {
			hasNot = true;
			//conditionNew.remove("not");
			conditionNew = remove(conditionNew, "not");
			conditionNew = trimmed(conditionNew);
		}
		QRegularExpression re("(\\w)\\s*([><=andotr]+)\\s*(\\w)");
		QRegularExpressionMatch match = re.match(conditionNew);
		assert(match.hasMatch());

		char varLeft = match.captured(1).at(0);
		string midOpr = match.captured(2);
		char varRight = match.captured(3).at(0);

		int left = getVarValue(varLeft);
		int right = getVarValue(varRight);

		bool res = true;
		if (midOpr == ">=")
			res = left >= right;
		else if (midOpr == "==")
			res = left == right;
		else if (midOpr == "<=")
			res = left <= right;
		else if (midOpr == ">")
			res = left > right;
		else if (midOpr == "<")
			res = left < right;
		else if (midOpr == "and")
			res = left && right;
		else if (midOpr == "not")
			res = !right;
		else
			assert(false);

		return hasNot ? !res : res;
	}

	int getVarValue(const char var) const {
		//if (var.isNumber()) 
		if (var>='0'&&var<='9')
		{
			//return formatString("%c", var).toInt();
			return var - '0';
		}

		for (const auto& v : vars) {
			if (v.name == var)
				return v.value;
		}
		return 0;
	}

	string preLable;
	string postLable;
	string condition;
	string opr;
	list<Variable> vars;
};


//KS �е�R�ṹ
struct KsR
{
	string preLabel;
	string postLabel;
	Variables preVars;
	Variables postVars;
	string opr;

	string toString() const {
		if (opr.empty() && preLabel.empty() && postLabel.empty() )
			return string();

		list<string> unknowndVars;
		for (const auto &v : postVars) {
			bool conatins = false;
			for (const auto& vPre : preVars) {
				if (vPre.name == v.name) {
					conatins = true;
					break;
				}
			}
			if (!conatins)
				//unknowndVars.push_back(v.name);
				unknowndVars.push_back(v.name+"");//?��֪������
		}

		string tmp;
		tmp += "(pc=";
		tmp += preLabel.empty() ? "U" : preLabel ;

		for (const auto& v : preVars) {
			tmp += ",";
			tmp += v.toString();
		}

		//�ӽ���δ����ı���
		for (const auto& v : unknowndVars) {
			tmp += ",";
			tmp += formatString("%s=u", v);
		}
		
		tmp += ") -> (pc'=";
		tmp += postLabel.empty() ? "U" : postLabel;

		int n = 0;
		for (const auto& v : postVars) {
			tmp += ",";	
			tmp += v.toString(true);
		}
	
		tmp += ")";

		//ע�͵��任����
		//tmp += "and (";
		//tmp += opr;
		//tmp += ")";
		return tmp;
	}
};



//�����������Ĵ����
//���֮��һ������Σ�˵��û�в������˻������߳�ִ��
list<string> parseCoProcesses( const string &text ) {
	list<string> processes;
	list<string> processTags;  //����α�ǩ
	QRegularExpression re("cobegin(.+)coend");
	QRegularExpressionMatch match = re.match(text);
	if (match.hasMatch()) {
		string processTmp = match.captured(1);
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
		re.setPattern(v + "::([^:]+)");
		match = re.match(text);
		if (match.hasMatch()) {
			string split = match.captured(1);
			//split = split.left(split.lastIndexOf(';') + 1);//
			split = split.substr(0,lastIndexOf(split,';') + 1);//

			split = trimmed(split);
			//processes << split;
			processes.push_back(split);
			return processes;
		}
	}

	return processes;
}

//���� wait ���
Statement parseWait(const string& input) {
	Statement sm;
	string condition;
	QRegularExpression re("wait\\((.+)\\)");
	QRegularExpressionMatch match = re.match(input);
	condition = trimmed(match.captured(1));

	sm.type = StatementType::Wait;
	sm.condition = condition;

	return sm;
}

//����˳�����
Statements parseSequence( const string &input ) {
	Statements sms;
	string inputTrimmed = trimmed(input);
	//list<string> list = inputTrimmed.split(';', Qt::SkipEmptyParts);
	list<string> list = split(inputTrimmed, ";");
	for (auto& v : list) {
		Statement sm;
		v = trimmed(v);
		//˳�����������ܻ����wait���
		if (v.find("wait") != -1) {
			sm = parseWait(input);
		}
		else {
			sm.type = StatementType::Squence;
			sm.seqBody = v;
		}
		sms.push_back(sm);
	}
	return sms;
}

bool parseStatements(const string& input, Statements& statements);

//���� if ���
Statement parseIf(const string& input) {
	Statement sm;
	string inputNew = input;
	//inputNew.remove('\n');
	inputNew = remove(inputNew, "\n");
	string condition, ifBody, elseBody;
	
	if (input.find("else") != -1) {
		QRegularExpression re("if(.+)then(.+)else(.+)endif;", QRegularExpression::MultilineOption);
		QRegularExpressionMatch match = re.match(inputNew);
		Q_ASSERT(match.hasMatch());
		condition = trimmed(match.captured(1));
		ifBody = trimmed(match.captured(2));
		elseBody = trimmed(match.captured(3));
	}
	else {
		QRegularExpression re("if(.+)then(.+)endif");
		QRegularExpressionMatch match = re.match(inputNew);
		condition = match.captured(1).trimmed();
		ifBody = match.captured(2).trimmed();
	}

	sm.type = StatementType::If;
	sm.condition = condition;
	parseStatements(ifBody, sm.ifBody);
	parseStatements(elseBody, sm.elseBody);

	return sm;
}

//����while���
Statement parseWhile(const string& input) {
	string inputNew = input;
	//inputNew.remove('\n');
	inputNew = remove(inputNew, "\n");
	Statement sm;
	string condition, body;
	QRegularExpression re("while(.+)do(.+)end");
	QRegularExpressionMatch match = re.match(inputNew);
	condition = match.captured(1).trimmed();
	body = match.captured(2).trimmed();

	sm.type = StatementType::While;
	sm.condition = condition;
	parseStatements(body, sm.whileBody);

	return sm;
}

//���������ת��Ϊ�߼���ʽ
static FirstOrderLogical toFormula(const Statement& pre, const Statement& post) {
	FirstOrderLogical lg;
	lg.preLable = pre.label;
	lg.postLable = post.label;
	lg.condition = pre.condition;
	lg.opr = pre.seqBody;
	lg.vars = pre.vars;
	return lg;
}

//���������ת��Ϊ�߼���ʽ
list<FirstOrderLogical> toFormula(const Statements& statements, Statement &out = Statement()) {
	if (statements.empty())
		return list<FirstOrderLogical>();
	if (out.label.empty()) {
		//string prefix = statements.at(0).label.left(1);
		string prefix = (*at(statements,0)).label.substr(0, 1);
		//split = split.substr(0, lastIndexOf(split, ';') + 1);//
		out.label = prefix + "E";
	}

	list<FirstOrderLogical> list;
	for (int i = 0; i < statements.size(); ++i) {
		Statement postSm = out;
		//Statement sm=statements[i];
		Statement sm = *at(statements, i);
		if (i + 1 < statements.size()) {
			//postSm = statements.at(i + 1);
			postSm = *at(statements,i + 1);
		}

		if (sm.type == StatementType::If) {
			if (!sm.ifBody.empty()) {
				list.push_back( toFormula(sm, *(sm.ifBody.begin()) ) ); //?
				list .merge( toFormula(sm.ifBody, postSm) );
			}
			if (!sm.elseBody.empty()) {
				sm.reversedCondition();
				list.push_back( toFormula(sm, *(sm.elseBody.begin() ) ));
				list.merge ( toFormula(sm.elseBody, postSm));
			}
		}
		else if (sm.type == StatementType::While) {
			if (!sm.whileBody.empty()) {
				list.push_back( toFormula(sm, *(sm.whileBody.begin() ) ));
				list.merge( toFormula(sm.whileBody, sm));
			}
			sm.reversedCondition();
			list.push_back( toFormula(sm, postSm));
		}
		else if (sm.type == StatementType::Wait) {
			sm.reversedCondition();
			list .push_back( toFormula(sm, sm) );
			sm.reversedCondition();
			list .push_back( toFormula(sm, postSm));
		}
		else {
			list.push_back( toFormula(sm, postSm));
		}
	}
	return list;
}

/*
//���ڻ�ͼ�Ĵ���
class KsGraphicDrawer: public QLabel {
public:
	KsGraphicDrawer( const list<string> &lables, 
		const list<pair<string, string>> & relations ) 
		: _labels(lables)
		, _relations(relations)
	{
	}

protected:
	void paintEvent(QPaintEvent*);

private:
	list<pair<string, string>> _relations;
	list<string> _labels;
	QMap<string, QRectF> _labelsGem;
};

//���Ƽ�ͷ
void calcVertexes(double start_x, double start_y, double end_x, double end_y, double& x1, double& y1, double& x2, double& y2)
{
	double arrow_lenght_ = 6;//��ͷ���ȣ�һ��̶�
	double arrow_degrees_ = 0.5;//��ͷ�Ƕȣ�һ��̶�

	double angle = atan2(end_y - start_y, end_x - start_x) + 3.1415926;

	x1 = end_x + arrow_lenght_ * cos(angle - arrow_degrees_);//��ü�ͷ��1����
	y1 = end_y + arrow_lenght_ * sin(angle - arrow_degrees_);
	x2 = end_x + arrow_lenght_ * cos(angle + arrow_degrees_);//��ü�ͷ��2����
	y2 = end_y + arrow_lenght_ * sin(angle + arrow_degrees_);
}

//����KS�ṹͼ
void KsGraphicDrawer::paintEvent(QPaintEvent*)
{
	static const int w = 90;
	static const int h = 60;
	static const int hSpan = 40;
	static const int vSpan = 40;
	QPainter painter(this);

	painter.drawText(QPoint(hSpan, vSpan/2), "Kripke Structure:");

	int n = 0;
	QSize size(w, h);
	int yPos = 0;
	int col = 0;
	//�������е�Label
	while (n < _labels.size()) {
		++col;
		yPos += h + vSpan;
		for (int n1 = 0; n1 < col; ++n1) {
			if (n >= _labels.size()) {
				break;
			}
			QRect rect(n1 * (w+hSpan), yPos, size.width(), size.height());
			painter.drawEllipse(rect);
			//painter.drawText(rect, Qt::AlignCenter, _labels.at(n).empty() ? "Begin": _labels.at(n));
			painter.drawText(rect, Qt::AlignCenter, (*at(_labels,n)).empty() ? "Begin" : *at(_labels,n));
			_labelsGem[*at(_labels,n)] = rect;
			++n;
		}
	}

	//���ƹ�ϵ��ͷ
	for (const auto& v : _relations) {
		//������ǩ
		if( v.second.empty() )
			continue;

		//�Ե���
		if (v.first == v.second) {	
			QRectF rct = _labelsGem.value(v.first);
			painter.drawLine(rct.center().x(), rct.top(), rct.right()+5, rct.top() );
			painter.drawLine(rct.right()+5, rct.top(), rct.right() + 5, rct.center().y());

			QPointF s(rct.right() + 5, rct.center().y());
			QPointF e(rct.right(), rct.center().y());
			double x1, y1, x2, y2;
			calcVertexes(s.x(), s.y(), e.x(), e.y(), x1, y1, x2, y2);
			painter.drawLine(s, e);
			painter.drawLine(e, { x1, y1 });
			painter.drawLine(e, { x2, y2 });
			continue;
		}
		double x1, y1, x2, y2;
		QPointF start = _labelsGem.value(v.first).center();
		start.setY(start.y());
		QPointF end = _labelsGem.value(v.second).center();
		end.setY(end.y() - 10);
		calcVertexes(start.x(), start.y(), end.x(), end.y(), x1, y1, x2, y2);
		painter.drawLine(start, end);
		painter.drawLine(end, { x1, y1 });
		painter.drawLine(end, { x2, y2 });
	}
}
*/

//����,��ʼ��ҵ����
ImpKs::ImpKs()
{
	/*
    ui.setupUi(this);

	QFont font = ui.inputEdit->font();
	font.setPointSize(12);
	ui.inputEdit->setFont(font);
	ui.outputEdit->setFont(font);

	//���ز�������
	//֧��ͨ���˵�ѡ���������
	connect(ui.actionWhile, &QAction::triggered, this, [this] {
		ui.inputEdit->setText(g_input[0]);
		});
	connect(ui.actionIf, &QAction::triggered, this, [this] {
		ui.inputEdit->setText(g_input[1]);
		});
	connect(ui.actionwhileif, &QAction::triggered, this, [this] {
		ui.inputEdit->setText(g_input[2]);
		});
	connect(ui.actionifWhile, &QAction::triggered, this, [this] {
		ui.inputEdit->setText(g_input[3]);
		});
	connect(ui.actionCobegin, &QAction::triggered, this, [this] {
		ui.inputEdit->setText(g_input[4]);
		});

	//��ʼִ��
	connect(ui.actionBuild, &QAction::triggered, this, &ImpKs::onStart);

    //Ĭ�ϲ�������
    ui.inputEdit->setText(g_input[2]);
	*/
}


//������������Ϊ�﷨��
bool parseStatements(const string& input, Statements& statements) {
	int s = 0;
	int e = 0;
	while (s < input.length())
	{
		//int pos = input.indexOf(QRegularExpression("(if|while)"), s);
		int pos = input.find(QRegularExpression("(if|while)"), s);
		if (-1 == pos) {
			e = input.length();
			//string inputSplit = input.mid(s, e - s);
			string inputSplit = input.substr(s, e - s);
			statements .merge( parseSequence(inputSplit) );
			s = e;
		}
		else {
			if (pos > s) {
				//statements. merge( parseSequence(input.mid(s, pos - s)) );
				statements.merge(parseSequence(input.substr(s, pos - s)));
			}
			s = pos;
			if (input.at(pos) == 'i') {
				//e = input.indexOf("endif");
				e = input.find("endif");
				e += 6;
				statements .push_back( parseIf(input.substr(s, e - s)) );
			}
			else {
				//e = input.indexOf("endwhile");
				e = input.find("endwhile");
				e += 9;
				statements .push_back( parseWhile(input.substr(s, e - s)) );
			}
			s = e;
		}
	}
	return true;
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
		vars .push_back( Variable{ Variable::Int, var, value } );
}


// �����������ϱ�ǩ
// һ���߼���ʽ��Ҫʹ������ǩ
// ͬʱҲ��������չʾ����ǩ�Ĵ����
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

//�����������ϱ�ǩ
void labledStatements(list<Statements> &smss) {
	char prefix = 'A';
	for (auto& sms : smss) {
		int index = 0;
		labledStatements(prefix, index, sms);
		//prefix = char(prefix.unicode() + 1);
		prefix = char(prefix + 1);
	}
}
//void statementToList(const Statements &sms, QStringList &list, QString &space=QString())
void statementToList(const Statements &sms, list<string> &lis, string &space) {//?
	if (sms.empty())
		return;

	string SpaceNew = space + "    ";
	list<string> ls;
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

void ImpKs::showTip(const string& tip)
{
	QMessageBox::warning(Q_NULLPTR, "", tip);
}


void nextVars(const Variables& src, FirstOrderLogical &dst) {
	dst.vars.clear();
	dst.vars = src;
	pair<char,int> pir = dst.assign();

	if (   !(   pir.first==NULL  )   )
		changeValue(dst.vars, pir.first, pir.second);					
}

//������һ����λ�ã�������Ҫ��������
FirstOrderLogical nextStep(list <FirstOrderLogical> &lgs, FirstOrderLogical &cur) {

	FirstOrderLogical lg;
	if (cur.isNull()) {
		lg.postLable = (*(lgs.begin())).preLable;
		nextVars(cur.vars, lg);
		return lg;
	}

	for (auto& v : lgs) {
		if (v.preLable == cur.postLable) {
			nextVars(cur.vars, v);
			if ( v.isConditionOk() )
				return v;
		}
	}
	return lg;
}



void createKsLables(list<list<FirstOrderLogical>>& lgss,
	//����PC��ָ��ǰִ�еĳ����
	const list<string>& pcs,
	list<pair<string, string>>& relations,
	list<string>& labels,
	list<FirstOrderLogical>& lastLgs,
	//��ǰ����ֵ
	const Variables& vars,
	list<string> &states,
	list<KsR> &Rs, int deep = 0) {

	++deep;
	list<string> tmp = pcs;
	list<FirstOrderLogical> lastLgsTmp = lastLgs;
	for (int i = 0; i < pcs.size(); ++i) {
		//�û�ִ����֮��ָ�
		KsR oneRs;
		//string oldLabel = tmp.join(' ');
		string oldLabel = jointList(tmp," ");
		oneRs.preLabel = oldLabel;

		//ֻ�����ո�����Ϊ�ǿ�
		string oldTmp = oldLabel;
		oldTmp=remove(oldTmp," ");
		//oldTmp.remove(' ');
		if (oldTmp.empty())
			oldLabel.clear();

		//ִ����Ҫ�ָ�����һ����״̬
		(*at(lastLgsTmp,i)).vars = vars;
		FirstOrderLogical lastLg = *at(lastLgsTmp,i);
		string lastArgsStr = (*at(lastLgsTmp,i)).valueToString();
		oneRs.preVars = (*at(lastLgsTmp,i)).vars;
		if (!oldLabel.empty() && !lastArgsStr.empty() )
			oldLabel += ',' + lastArgsStr;

		if (oldLabel.find("A1") != -1 && !(oldLabel.find("B")!=-1)) {
			int a = 10;
		}

		*at(lastLgsTmp,i) = nextStep(*at(lgss,i),*at(lastLgsTmp,i));
		
		*at(tmp,i)= (*at(lastLgsTmp,i)).postLable;
		Variables newVars = (*at(lastLgsTmp,i)).vars;
		//string newLabel = tmp.join(' ');
		string newLabel = jointList(tmp, " ");

		//�ռ�R�任
		oneRs.postLabel = newLabel;
		oneRs.postVars = (*at(lastLgsTmp,i)).vars;
		oneRs.opr = (*at(lastLgsTmp,i)).opr;
		Rs.push_back( oneRs);

		//�ռ�״̬S
		string oneState = (*at(lastLgsTmp,i)).valueToString();
		
		if (!oneState.empty() && !contains_list(states,oneState)) {
			states.push_back(oneState);
		}

		if (!newLabel.empty() && !(*at(lastLgsTmp,i)).valueToString().empty())
			newLabel += ',' + (*at(lastLgsTmp,i)).valueToString();

		pair<string, string> r{ oldLabel, newLabel };
		if (contains_list(relations,r)) {
			(*at(tmp,i)) = *at(pcs,i);//����
			(*at(lastLgsTmp,i)) = lastLg;
			continue;
		}

		if (!oldLabel.empty() && !newLabel.empty()) {
			//relations << pair<string, string>{ oldLabel, newLabel };
			relations.push_back( pair<string, string>{ oldLabel, newLabel } );
		}	
		if (! contains_list(labels,oldLabel) && !oldLabel.empty()) {
			labels.push_back(oldLabel);
		}

		if (!contains_list(labels, newLabel)&& !newLabel.empty()) {
			labels.push_back(newLabel);
		}
		createKsLables(lgss, tmp, relations, labels, lastLgsTmp, newVars, states, Rs, deep);
		(*at(tmp,i)) = *at(pcs,i);
		(*at(lastLgsTmp,i)) = lastLg;
	}
}

bool checkInputOk(const string& input) {
	return true;
}


//��ʼִ�г���
void ImpKs::onStart()
{
	/*
	ui.outputEdit->clear();
	ui.scrollArea->setWidget(Q_NULLPTR);
	*/


	//string input = ui.inputEdit->toPlainText();
	string input = g_input[0];//��������

	//ÿ����Ҫ���֮ǰ�����
	if (!checkInputOk(input)) {
		showTip("Code input error, please check your code!");
		return;
	}

	//���������г����
	list<string> processes = parseCoProcesses(input);

	list<Statements> statements;
	for (const auto& v : processes) {
		Statements tmp;
		parseStatements(v, tmp);
		statements.push_back(tmp);
	}

	//�����������ϱ�ǩ
	labledStatements(statements);

	//�������ǩ�Ĵ���
	string label_code;//-------------------------------�����ǩ-----------------
	label_code.append("Labeled function:\n");
	//ui.outputEdit->append("Labeled function:\n");
	for (const auto& v : statements) {
		list<string> list;
		string space;
		statementToList(v, list, space);
		//���һ��������ǩ
		string prefix = (*at(list, 0)).substr(0, 1);
		list.push_back(prefix + "E:");
		//ui.outputEdit->append(list.join('\n'));
		label_code.append(jointList(list, "\n"));

		label_code.append("\n\n");
	}

	//����߼���ʽ
	list<list<FirstOrderLogical>> lgss;
	string logic_code;//-------------------------------����߼���ʽ-----------------
	logic_code.append("First order logical formula:");
	bool hasPc = statements.size() > 1;
	for (int i = 0; i < statements.size(); ++i) {
		list<FirstOrderLogical> formulas = toFormula(*at(statements, i));
		lgss.push_back(formulas);
		for (const auto& v : formulas) {
			if (hasPc) {
				string pc = formatString("pc%d", i);
				string formulaNew = v.toString();
				formulaNew.replace(formulaNew.find("pc"), 2, pc);
				logic_code.append(formatString("pc=%s and %s", pc, formulaNew));
			}
			else
				logic_code.append(v.toString());
		}
	}

	list<string> pcs;
	list<pair<string, string>> relations;
	list<string> lables;
	list<FirstOrderLogical> lastLgs;
	list<string> states;
	list<KsR> Rs;
	Variables vars;
	for (const auto& v : lgss) {
		if (lgss.size() > 1) {
			pcs.push_back("U");
		}
		else {
			pcs.push_back("");
		}
		lastLgs.push_back(FirstOrderLogical());
	}
	createKsLables(lgss, pcs, relations, lables, lastLgs, vars, states, Rs);

	//�������S״̬
	string state_code;//------------------------���״̬s
	state_code.append("\n\nAll States:\n");
	int index = 0;
	for (const auto& v : lables) {
		state_code.append(formatString("S%d:(%s)", index++, v));
	}

	state_code.append("\n");
	index = 0;
	for (const auto& v : Rs) {
		if (!v.toString().empty())
			state_code.append(formatString("R%d:= %s", index++, v.toString()));
	}
	/*
	//����KSͼ
	QWidget* widget = new KsGraphicDrawer(lables, relations);
	ui.scrollArea->setWidget(widget);
	widget->setGeometry(0, 0, ui.scrollArea->width(), ui.scrollArea->height());
	*/
}
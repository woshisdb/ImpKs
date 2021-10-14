/**
* impqt.cpp ��Ҫҵ���ļ���IMP���Է���.
* \a ¬责��˵������
* \f �����������룬�������������
* \u 2021/05/10
*/

#include "imp_ks.h"

#include <QRegularExpression>
#include <QMap>
#include <QLabel>
#include <QPainter>
#include <QList>
#include <QMessageBox>


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
const QString g_input[] = {
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
	QString toString(bool isPost = false) const {
		if ( !isPost )
			return QString("%1=%2").arg(name).arg(value);
		else
			return QString("%1'=%2").arg(name).arg(value);
	}
	Type  type;
	QChar name;
	int   value;
};

struct Statement;
using Statements = QList<Statement>;
using Variables = QList<Variable>;

/**
                                                                      
*/
struct Statement
{
	bool isNull() const {
		return condition.isEmpty() && seqBody.isEmpty();
	}
	void reversedCondition() {
		if (condition.contains("not")) {
			condition.remove("not");
			condition = condition.trimmed();
		}
		else {
			condition = QString("not %1").arg(condition);
		}
	}
	StatementType type;  //�������
	QString label;       //����ǩ
	QString condition;
	QString seqBody;
	Statements ifBody;
	Statements elseBody;
	Statements whileBody;
	QList<Variable>  vars;
};


/**
* \b һ���߼����ݽṹ�����Դ�һ���߼�����KS�ṹ
  \a preLable ǰ�ñ�ǩ
  \a postLable ���ñ�ǩ
  \a condition ����
  \a QString opr ����
  \a QList<Variable> vars;
*/
struct FirstOrderLogical
{
public:
	bool isNull() const {
		return preLable.isEmpty() && postLable.isEmpty();
	}

	static QString findAssignVariable(const QString& statement) {
		QRegularExpression re("(\\w+)\\s*=\\s*");
		QRegularExpressionMatch match = re.match(statement);
		if (match.hasMatch()) {
			QString s = match.captured(1);
			return match.captured(1);
		}
		else {
			return QString();
		}
	}

	QString toString() const {
		if (!condition.isEmpty()) {
			return QString("pc=%1 and pc'=%2 and (%3) and SAME(V)").arg(preLable).arg(postLable).arg(condition);
		}
		else {
			QString tmp = QString("pc=%1 and pc'=%2 and (%3)").arg(preLable).arg(postLable).arg(opr);
			QString var = findAssignVariable(opr);
			if (var.isEmpty()) {
				tmp += " and SAME(V)";
			}
			else {
				tmp += QString(" and SAME(V\\{%1})").arg(var);
			}
			return tmp;
		}
	}

	QString valueToString() const {
		QString tmp;
		for (const auto &v : vars) {
			if (!tmp.isEmpty()) {
				tmp += ",";
			}
			tmp += QString("%1=%2").arg(v.name).arg(v.value);
		}
		return tmp;
	}

	QPair<QChar, int> assign()  const {
		if (opr.isEmpty() || !opr.contains('=')) {
			return QPair<QChar, int>();
		}

		QRegularExpression re("(\\w)\\s*=\\s*(\\w)\\s*([\\*+-])\\s*(\\w)");
		QRegularExpressionMatch match = re.match(opr);
		if (match.hasMatch()) {
			QChar ret = match.captured(1).at(0);
			QChar left = match.captured(2).at(0);
			QChar midOpr = match.captured(3).at(0);
			QChar right = match.captured(4).at(0);

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
			
			return QPair<QChar, int>(ret, nRet);
		}

		re.setPattern("(\\w+)\\s*=\\s*(\\d+)");
		match = re.match(opr);
		if (match.hasMatch()) {
			return QPair<QChar, int>{ match.captured(1).at(0), match.captured(2).toInt()};
		}
		return QPair<QChar, int>();
	}

	bool hasAssign() const {
		if (!condition.isEmpty() || opr.isEmpty()) {
			return false;
		}
		QString var = findAssignVariable(opr);
		if (var.isEmpty())
			return false;

		return true;
	}

	bool isConditionOk() const {
		if (condition.isEmpty())
			return true;
		if (0 == condition.compare("true", Qt::CaseInsensitive))
			return true;
		if (0 == condition.compare("false", Qt::CaseInsensitive))
			return false;

		bool hasNot = false;
		QString conditionNew = condition;
		if (conditionNew.contains("not")) {
			hasNot = true;
			conditionNew.remove("not");
			conditionNew = conditionNew.trimmed();
		}
		QRegularExpression re("(\\w)\\s*([><=andotr]+)\\s*(\\w)");
		QRegularExpressionMatch match = re.match(conditionNew);
		assert(match.hasMatch());

		QChar varLeft = match.captured(1).at(0);
		QString midOpr = match.captured(2);
		QChar varRight = match.captured(3).at(0);

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

	int getVarValue(const QChar var) const {
		if (var.isNumber()) {
			return QString("%1").arg(var).toInt();
		}

		for (const auto& v : vars) {
			if (v.name == var)
				return v.value;
		}
		return 0;
	}

	QString preLable;
	QString postLable;
	QString condition;
	QString opr;
	QList<Variable> vars;
};


//KS �е�R�ṹ
struct KsR
{
	QString preLabel;
	QString postLabel;
	Variables preVars;
	Variables postVars;
	QString opr;

	QString toString() const {
		if (opr.isEmpty() && preLabel.isEmpty() && postLabel.isEmpty() )
			return QString();

		QStringList unknowndVars;
		for (const auto &v : postVars) {
			bool conatins = false;
			for (const auto& vPre : preVars) {
				if (vPre.name == v.name) {
					conatins = true;
					break;
				}
			}
			if (!conatins)
				unknowndVars.push_back(v.name);
		}

		QString tmp;
		tmp += "(pc=";
		tmp += preLabel.isEmpty() ? "U" : preLabel ;

		for (const auto& v : preVars) {
			tmp += ",";
			tmp += v.toString();
		}

		//�ӽ���δ����ı���
		for (const auto& v : unknowndVars) {
			tmp += ",";
			tmp += QString("%1=u").arg(v);
		}
		
		tmp += ") -> (pc'=";
		tmp += postLabel.isEmpty() ? "U" : postLabel;

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
QStringList parseCoProcesses( const QString &text ) {
	QStringList processes;
	QStringList processTags;  //����α�ǩ
	QRegularExpression re("cobegin(.+)coend");
	QRegularExpressionMatch match = re.match(text);
	if (match.hasMatch()) {
		QString processTmp = match.captured(1);
		processTmp.remove(' ');
		processTags = processTmp.split("||");
	}

	//���û�в��г����������������һ�����߳�ִ�еĳ���
	if (processTags.isEmpty()) {
		return processes << text;
	}

	//����в��г�����������������г����
	for (auto& v : processTags) {
		re.setPattern(v + "::([^:]+)");
		match = re.match(text);
		if (match.hasMatch()) {
			QString split = match.captured(1);
			split = split.left(split.lastIndexOf(';') + 1);
			split = split.trimmed();
			processes << split;
		}
	}

	return processes;
}

//���� wait ���
Statement parseWait(const QString& input) {
	Statement sm;
	QString condition;
	QRegularExpression re("wait\\((.+)\\)");
	QRegularExpressionMatch match = re.match(input);
	condition = match.captured(1).trimmed();

	sm.type = StatementType::Wait;
	sm.condition = condition;

	return sm;
}

//����˳�����
Statements parseSequence( const QString &input ) {
	Statements sms;
	QString inputTrimmed = input.trimmed();
	QStringList list = inputTrimmed.split(';', Qt::SkipEmptyParts);
	for (auto& v : list) {
		Statement sm;
		v = v.trimmed();
		//˳�����������ܻ����wait���
		if (v.contains("wait")) {
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

bool parseStatements(const QString& input, Statements& statements);

//���� if ���
Statement parseIf(const QString& input) {
	Statement sm;
	QString inputNew = input;
	inputNew.remove('\n');
	QString condition, ifBody, elseBody;
	
	if (input.contains("else")) {
		QRegularExpression re("if(.+)then(.+)else(.+)endif;", QRegularExpression::MultilineOption);
		QRegularExpressionMatch match = re.match(inputNew);
		Q_ASSERT(match.hasMatch());
		condition = match.captured(1).trimmed();
		ifBody = match.captured(2).trimmed();
		elseBody = match.captured(3).trimmed();
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
Statement parseWhile(const QString& input) {
	QString inputNew = input;
	inputNew.remove('\n');
	Statement sm;
	QString condition, body;
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
QList<FirstOrderLogical> toFormula(const Statements& statements, Statement &out = Statement()) {
	if (statements.isEmpty())
		return QList<FirstOrderLogical>();
	if (out.label.isEmpty()) {
		QString prefix = statements.at(0).label.left(1);
		out.label = prefix + "E";
	}

	QList<FirstOrderLogical> list;
	for (int i = 0; i < statements.size(); ++i) {
		Statement postSm = out;
		Statement sm = statements[i];
		if (i + 1 < statements.size()) {
			postSm = statements.at(i + 1);
		}

		if (sm.type == StatementType::If) {
			if (!sm.ifBody.isEmpty()) {
				list << toFormula(sm, sm.ifBody.first());
				list << toFormula(sm.ifBody, postSm);
			}
			if (!sm.elseBody.isEmpty()) {
				sm.reversedCondition();
				list << toFormula(sm, sm.elseBody.first());
				list << toFormula(sm.elseBody, postSm);
			}
		}
		else if (sm.type == StatementType::While) {
			if (!sm.whileBody.isEmpty()) {
				list << toFormula(sm, sm.whileBody.first());
				list << toFormula(sm.whileBody, sm);
			}
			sm.reversedCondition();
			list << toFormula(sm, postSm);
		}
		else if (sm.type == StatementType::Wait) {
			sm.reversedCondition();
			list << toFormula(sm, sm);
			sm.reversedCondition();
			list << toFormula(sm, postSm);
		}
		else {
			list << toFormula(sm, postSm);
		}
	}
	return list;
}


//���ڻ�ͼ�Ĵ���
class KsGraphicDrawer: public QLabel {
public:
	KsGraphicDrawer( const QStringList &lables, 
		const QList<QPair<QString, QString>> & relations ) 
		: _labels(lables)
		, _relations(relations)
	{
	}

protected:
	void paintEvent(QPaintEvent*);

private:
	QList<QPair<QString, QString>> _relations;
	QStringList _labels;
	QMap<QString, QRectF> _labelsGem;
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
			painter.drawText(rect, Qt::AlignCenter, _labels.at(n).isEmpty() ? "Begin": _labels.at(n));
			_labelsGem[_labels.at(n)] = rect;
			++n;
		}
	}

	//���ƹ�ϵ��ͷ
	for (const auto& v : _relations) {
		//������ǩ
		if( v.second.isEmpty() )
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


//����,��ʼ��ҵ����
ImpQt::ImpQt(QWidget *parent)
    : QMainWindow(parent)
{
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
	connect(ui.actionBuild, &QAction::triggered, this, &ImpQt::onStart);

    //Ĭ�ϲ�������
    ui.inputEdit->setText(g_input[2]);
}


//������������Ϊ�﷨��
bool parseStatements(const QString& input, Statements& statements) {
	int s = 0;
	int e = 0;
	while (s < input.length())
	{
		int pos = input.indexOf(QRegularExpression("(if|while)"), s);
		if (-1 == pos) {
			e = input.length();
			QString inputSplit = input.mid(s, e - s);
			statements << parseSequence(inputSplit);
			s = e;
		}
		else {
			if (pos > s) {
				statements << parseSequence(input.mid(s, pos - s));
			}
			s = pos;
			if (input.at(pos) == 'i') {
				e = input.indexOf("endif");
				e += 6;
				statements << parseIf(input.mid(s, e - s));
			}
			else {
				e = input.indexOf("endwhile");
				e += 9;
				statements << parseWhile(input.mid(s, e - s));
			}
			s = e;
		}
	}
	return true;
}

void changeValue(Variables &vars, QChar var, int value) {
	bool find = false;
	for (auto& v : vars) {
		if (v.name == var) {
			v.value = value;
			find = true;
			break;
		}
	}
	if (!find)
		vars << Variable{ Variable::Int, var, value };
}


// �����������ϱ�ǩ
// һ���߼���ʽ��Ҫʹ������ǩ
// ͬʱҲ��������չʾ����ǩ�Ĵ����
void labledStatements(const QChar& prefix, int& index, Statements& sms) {
	if (sms.isEmpty()) {
		return;
	}
	for (auto& v : sms) {
		v.label = QString("%1%2").arg(prefix).arg(index++);
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
void labledStatements(QList<Statements> &smss) {
	QChar prefix = 'A';
	for (auto& sms : smss) {
		int index = 0;
		labledStatements(prefix, index, sms);
		prefix = QChar(prefix.unicode() + 1);
	}
}

void statementToList(const Statements &sms, QStringList &list, QString &space=QString()) {
	if (sms.isEmpty())
		return;

	QString SpaceNew = space + "    ";
	QStringList ls;
	for (const auto& v : sms) {
		if (v.type == StatementType::Squence) {
			list << v.label + ": " + space + v.seqBody + ';';
		}
		else if (v.type == StatementType::Wait) {
			list << v.label + ": " + space + "wait (" + v.condition + ");";
		}
		else if (v.type == StatementType::If) {
			list << v.label + ": " + space  + "if " + v.condition + " then";
			statementToList(v.ifBody, list, SpaceNew);
			list <<  space + "   else";
			statementToList(v.elseBody, list, SpaceNew);
			list << space + "   endif";
		}
		else if (v.type == StatementType::While) {
			list << v.label +": " + space + "while " + v.condition + " do";
			statementToList(v.whileBody, list, SpaceNew);
			list << space + "   endwhile;";
		}
	}
}

void ImpQt::showTip(const QString& tip)
{
	QMessageBox::warning(Q_NULLPTR, "", tip);
}


void nextVars(const Variables& src, FirstOrderLogical &dst) {
	dst.vars.clear();
	dst.vars = src;
	QPair<QChar, int> pair = dst.assign();
	if (!pair.first.isNull())
		changeValue(dst.vars, pair.first, pair.second);					
}

//������һ����λ�ã�������Ҫ��������
FirstOrderLogical nextStep(QList <FirstOrderLogical> &lgs, FirstOrderLogical &cur) {

	FirstOrderLogical lg;
	if (cur.isNull()) {
		lg.postLable = lgs.first().preLable;
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



void createKsLables(QList<QList<FirstOrderLogical>>& lgss,
	//����PC��ָ��ǰִ�еĳ����
	const QStringList& pcs,
	QList<QPair<QString, QString>>& relations,
	QStringList& labels,
	QList<FirstOrderLogical>& lastLgs,
	//��ǰ����ֵ
	const Variables& vars,
	QStringList &states,
	QList<KsR> &Rs, int deep = 0) {

	++deep;
	QStringList tmp = pcs;
	QList<FirstOrderLogical> lastLgsTmp = lastLgs;
	for (int i = 0; i < pcs.size(); ++i) {
		//�û�ִ����֮��ָ�
		KsR oneRs;
		QString oldLabel = tmp.join(' ');
		oneRs.preLabel = oldLabel;

		//ֻ�����ո�����Ϊ�ǿ�
		QString oldTmp = oldLabel;
		oldTmp.remove(' ');
		if (oldTmp.isEmpty())
			oldLabel.clear();

		//ִ����Ҫ�ָ�����һ����״̬
		lastLgsTmp[i].vars = vars;
		FirstOrderLogical lastLg = lastLgsTmp[i];
		QString lastArgsStr = lastLgsTmp[i].valueToString();
		oneRs.preVars = lastLgsTmp[i].vars;
		if (!oldLabel.isEmpty() && !lastArgsStr.isEmpty() )
			oldLabel += ',' + lastArgsStr;

		if (oldLabel.contains("A1") && !oldLabel.contains("B")) {
			int a = 10;
		}

		lastLgsTmp[i] = nextStep(lgss[i], lastLgsTmp[i]);
		
		tmp[i] = lastLgsTmp[i].postLable;
		Variables newVars = lastLgsTmp[i].vars;
		QString newLabel = tmp.join(' ');

		//�ռ�R�任
		oneRs.postLabel = newLabel;
		oneRs.postVars = lastLgsTmp[i].vars;
		oneRs.opr = lastLgsTmp[i].opr;
		Rs << oneRs;

		//�ռ�״̬S
		QString oneState = lastLgsTmp[i].valueToString();
		if (!oneState.isEmpty() && !states.contains(oneState)) {
			states.push_back(oneState);
		}

		if (!newLabel.isEmpty() && !lastLgsTmp[i].valueToString().isEmpty())
			newLabel += ',' + lastLgsTmp[i].valueToString();

		QPair<QString, QString> r{ oldLabel, newLabel };
		if (relations.contains(r) ) {
			tmp[i] = pcs.at(i);
			lastLgsTmp[i] = lastLg;
			continue;
		}

		if (!oldLabel.isEmpty() && !newLabel.isEmpty()) {
			relations << QPair<QString, QString>{ oldLabel, newLabel };
		}	
		if (!labels.contains(oldLabel) && !oldLabel.isEmpty()) {
			labels.push_back(oldLabel);
		}

		if (!labels.contains(newLabel) && !newLabel.isEmpty()) {
			labels.push_back(newLabel);
		}
		createKsLables(lgss, tmp, relations, labels, lastLgsTmp, newVars, states, Rs, deep);
		tmp[i] = pcs.at(i);
		lastLgsTmp[i] = lastLg;
	}
}

bool checkInputOk(const QString& input) {
	return true;
}


//��ʼִ�г���
void ImpQt::onStart()
{
	ui.outputEdit->clear();
	ui.scrollArea->setWidget(Q_NULLPTR);

	QString input = ui.inputEdit->toPlainText();

	//ÿ����Ҫ���֮ǰ�����
	if (!checkInputOk(input)) {
		showTip("Code input error, please check your code!");
		return;
	}

	//���������г����
	QStringList processes = parseCoProcesses(input);

	QList<Statements> statements;
	for (const auto& v : processes) {
		Statements tmp;
		parseStatements(v, tmp);
		statements << tmp;
	}

	//�����������ϱ�ǩ
	labledStatements(statements);

	//�������ǩ�Ĵ���
	ui.outputEdit->append("Labeled function:\n");
	for (const auto& v : statements) {
		QStringList list;
		statementToList(v, list);
		//���һ��������ǩ
		QString prefix = list.at(0).left(1);
		list << prefix + "E:";
		ui.outputEdit->append(list.join('\n'));
		ui.outputEdit->append("\n\n");
	}

	//����߼���ʽ
	QList<QList<FirstOrderLogical>> lgss;
	ui.outputEdit->append("First order logical formula:");
	bool hasPc = statements.size() > 1;
	for (int i = 0; i < statements.size(); ++i) {
		QList<FirstOrderLogical> formulas = toFormula(statements.at(i));
		lgss << formulas;
		for (const auto& v : formulas) {
			if (hasPc) {
				QString pc = QString("pc%1").arg(i);
				QString formulaNew = v.toString();
				formulaNew.replace("pc", pc);
				ui.outputEdit->append(QString("pc=%1 and %2").arg(pc).arg(formulaNew));
			}
			else
				ui.outputEdit->append(v.toString());
		}
	}

	QStringList pcs;
	QList<QPair<QString, QString>> relations;
	QStringList lables;
	QList<FirstOrderLogical> lastLgs;
	QStringList states;
	QList<KsR> Rs;
	Variables vars;
	for (const auto& v : lgss) {
		if (lgss.size() > 1) {
			pcs << "U";
		} else {
			pcs << "";
		}
		lastLgs << FirstOrderLogical();
	}
	createKsLables(lgss, pcs, relations, lables, lastLgs, vars, states, Rs);

	//�������S״̬
	ui.outputEdit->append("\n\nAll States:\n");
	int index = 0;
	for (const auto& v : lables) {
		ui.outputEdit->append(QString("S%1:(%2)").arg(index++).arg(v));
	}

	ui.outputEdit->append("\n");
	index = 0;
	for (const auto& v : Rs) {
		if( !v.toString().isEmpty() )
			ui.outputEdit->append(QString("R%1:= %2").arg(index++).arg(v.toString()));
	}
	
	//����KSͼ
	QWidget* widget = new KsGraphicDrawer(lables, relations);
	ui.scrollArea->setWidget(widget);
	widget->setGeometry(0, 0, ui.scrollArea->width(), ui.scrollArea->height());
}
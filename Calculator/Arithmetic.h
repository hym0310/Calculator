#ifndef _ARITHMETIC_H_
#define _ARITHMETIC_H_

#include <stack>
#include <list>
#include <string>
#include <ostream>
#include <sstream>
#include <cmath>

using namespace std;

#ifdef UNICODE
typedef wostream _ostream_type;
#else
typedef ostream _ostream_type;
#endif

#ifndef TEXT(quote)
	#ifdef UNICODE
		#define TEXT(quote) L##quote
	#else
		#define TEXT(quote) quote
	#endif
#endif

class CArithmetic
{
public:
	CArithmetic(void);
	CArithmetic(TCHAR oper, const TCHAR *pFuncExp, CArithmetic *left, CArithmetic *right);
	CArithmetic(double result, const TCHAR *pFuncExp);

	~CArithmetic(void);

	double GetResult();
	void Output(_ostream_type &os);

private:
	double Function(basic_string<TCHAR> &funcExp);

private:
	double	result;
	double	left_result;
	double	right_result;
	TCHAR	oper;
	CArithmetic	*left_child;
	CArithmetic *right_child;
	CArithmetic *parent;
	basic_string<TCHAR> funcExp;
};

class COpMixNode
{
public:
	COpMixNode(TCHAR chOperator, basic_string<TCHAR> funcExp, bool bOperator = true)
		: m_bOperator(false)
	{
		bOperator ? (m_chOperator = chOperator) : (m_iOperand = chOperator);
		m_bOperator = bOperator;
		m_funcExp = funcExp;
	}
	COpMixNode(double iOperand, basic_string<TCHAR> funcExp, bool bOperator = false)
		: m_bOperator(false)
	{
		bOperator ? (m_chOperator = (TCHAR)iOperand) : (m_iOperand = iOperand);
		m_bOperator = bOperator;
		m_funcExp = funcExp;
	}
	COpMixNode(const COpMixNode &node)
		: m_bOperator(false)
	{
		node.m_bOperator ? (m_chOperator = node.m_chOperator) : (m_iOperand = node.m_iOperand);
		m_bOperator = node.m_bOperator;
		m_funcExp = node.m_funcExp;
	}
	COpMixNode &operator =(const COpMixNode &node)
	{
		if (&node == this)
		{
			return *this;
		}
		node.m_bOperator ? (m_chOperator = node.m_chOperator) : (m_iOperand = node.m_iOperand);
		m_bOperator = node.m_bOperator;
		m_funcExp = node.m_funcExp;
		return *this;
	}
	~COpMixNode(){}
public:
	union
	{
		TCHAR m_chOperator;
		double m_iOperand;
	};
	bool m_bOperator;
	basic_string<TCHAR> m_funcExp; // 函数表达式
};

class CCalculator
{
public:
	/*
		把表达式字符串中的变量替换成具体的值
		比如 abc + 16; 该方法可以把
	*/
	static void ReplaceArithmExpVariable(basic_string<TCHAR> &arithmExp, basic_string<TCHAR> variable, double varValue);
	/*
		把对应序号的变量替换成具体的值
		比如 {0} + {1} / {3}
	*/
	static void ReplaceArithmExpVariable(basic_string<TCHAR> &arithmExp, int index, double varValue);

public:
	CCalculator();
	~CCalculator();

	double Calculate(const TCHAR *pszExp);
	void Output(_ostream_type &os);

private:
	void TransExpression(const TCHAR *pszExp);
	int GetNumber(const TCHAR *pszStr, double &iRetNum);
	int ComparePriority(TCHAR chOperator, list<TCHAR> &OperatorStack);
	int GetPriority(TCHAR chOperator, bool bIsp);
	void CreateArithmetic();
	int GetFunction(const TCHAR *pszStr, double &iRetNum);
	int GetFunction(const TCHAR *pszStr, basic_string<TCHAR> &funcStr);

private:
	list<COpMixNode> m_OpMixStack;
	CArithmetic *	m_Arithm;
};

#endif //_ARITHMETIC_H_

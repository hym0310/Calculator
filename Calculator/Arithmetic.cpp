#include "stdafx.h"
#include "Arithmetic.h"
#include "deelx.hpp"

CArithmetic::CArithmetic(void)
: result(0), left_result(0)
, right_result(0), oper(TEXT('\0'))
, left_child(NULL), right_child(NULL)
, parent(NULL)
{
}

CArithmetic::CArithmetic(TCHAR choperator, const TCHAR *pFuncExp, CArithmetic *left, CArithmetic *right)
: result(0), left_result(0)
, right_result(0), oper(TEXT('\0'))
, left_child(NULL), right_child(NULL)
, parent(NULL)
{
	this->oper = choperator;
	funcExp = pFuncExp;
	left_child = left;
	right_child = right;
}

CArithmetic::CArithmetic(double uresult, const TCHAR *pFuncExp)
: result(0), left_result(0)
, right_result(0), oper(TEXT('\0'))
, left_child(NULL), right_child(NULL)
, parent(NULL)
{
	this->result = uresult;
	funcExp = pFuncExp;
}

CArithmetic::~CArithmetic(void)
{
	try
	{
		if (left_child != NULL)
		{
			delete left_child;
		}
		if (right_child != NULL)
		{
			delete right_child;
		}
	}
	catch (...)
	{ }

	parent = NULL;
}

double CArithmetic::GetResult()
{
	left_result = 0;
	right_result = 0;

	if (left_child != NULL && right_child != NULL)
	{
		left_result = left_child->GetResult();
		right_result = right_child->GetResult();

		switch (oper)
		{
		case TEXT('+'):
			result = left_result + right_result;
			break;
		case TEXT('-'):
			result = left_result - right_result;
			break;
		case TEXT('*'):
			result = left_result * right_result;
			break;
		case TEXT('/'):
			if (right_result != 0)
				result = left_result / right_result;
			else
				result = 0xFFFFFFFF;
			break;
		case TEXT('^'):
			result = pow(left_result, right_result);
			break;
		default:
			result = 0;
			break;
		}
	}

	basic_string<TCHAR> funcExpTmp = funcExp;
	Function(funcExpTmp);

	return result;
}

void CArithmetic::Output(_ostream_type &os)
{
	if (left_child != NULL && right_child != NULL)
	{
		os << TEXT('(');
		left_child->Output(os);
		os << oper;
		right_child->Output(os);
		os << TEXT(')');
	}
	else
	{
		os << result;
	}
}

double CArithmetic::Function( basic_string<TCHAR> &funcExp )
{
	if (funcExp.empty())
	{
		return result;
	}

	if (funcExp.find(TEXT("sin")) == 0)
	{
		result = sin(result);
		funcExp = funcExp.substr(3);
	}
	else if (funcExp.find(TEXT("cos")) == 0)
	{
		result = cos(result);
		funcExp = funcExp.substr(3);
	}
	else if (funcExp.find(TEXT("tan")) == 0)
	{
		result = tan(result);
		funcExp = funcExp.substr(3);
	}
	else if (funcExp.find(TEXT("asin")) == 0)
	{
		result = asin(result);
		funcExp = funcExp.substr(4);
	}
	else if (funcExp.find(TEXT("acos")) == 0)
	{
		result = acos(result);
		funcExp = funcExp.substr(4);
	}
	else if (funcExp.find(TEXT("atan")) == 0)
	{
		result = atan(result);
		funcExp = funcExp.substr(4);
	}
	else if (funcExp.find(TEXT("log")) == 0)
	{
		result = log(result);
		funcExp = funcExp.substr(3);
	}
	else if (funcExp.find(TEXT("sqrt")) == 0)
	{
		result = sqrt(result);
		funcExp = funcExp.substr(4);
	}

	return Function(funcExp);
}

CCalculator::CCalculator()
: m_Arithm(NULL)
{

}

CCalculator::~CCalculator()
{
	try
	{
		if (m_Arithm != NULL)
		{
			delete m_Arithm;
		}
	}
	catch(...)
	{}
}

void CCalculator::ReplaceArithmExpVariable(basic_string<TCHAR> &arithmExp, basic_string<TCHAR> variable, double varValue)
{
	CRegexpT<TCHAR> hexRegexp(TEXT("^((0x[0-9A-Fa-f]+)|([0-9]+))"));
	CRegexpT<TCHAR> sepRegexp(TEXT("^[\\s+\\-*/()]"));
	basic_string<TCHAR> varExpStr = TEXT("^") + variable + TEXT("(($)|([+\\-*/)\\s]))");
	CRegexpT<TCHAR> varRegexp(varExpStr.c_str());
	MatchResult result;

	basic_ostringstream<TCHAR> ssVarValue;
	ssVarValue << varValue;
	basic_string<TCHAR> strValue = ssVarValue.str();

	for (size_t i = 0; i < arithmExp.size(); )
	{
		basic_string<TCHAR> temp = arithmExp.substr(i);
		result = hexRegexp.Match(temp.c_str());
		if (result.IsMatched())
		{
			i = result.GetEnd() + i;
			continue;
		}
		result = sepRegexp.Match(temp.c_str());
		if (result.IsMatched())
		{
			i = result.GetEnd() + i;
			continue;
		}
		result = varRegexp.Match(temp.c_str());
		if (result.IsMatched())
		{
			arithmExp.replace(i, variable.size(), strValue);
			i += strValue.size();
		}
		else
		{
			do 
			{
				i++;
				temp = arithmExp.substr(i);
				result = sepRegexp.Match(temp.c_str());
			} while (!(result.IsMatched() || temp.empty()));
		}
	}
}

void CCalculator::ReplaceArithmExpVariable(basic_string<TCHAR> &arithmExp, int index, double varValue)
{
	basic_ostringstream<TCHAR> ssVarValue;
	basic_string<TCHAR> variable, varExpStrTmp;

	ssVarValue << TEXT("{") << index << TEXT("}");
	variable = ssVarValue.str();

	ssVarValue.str(TEXT(""));
	ssVarValue << TEXT("\\{") << index << TEXT("\\}");
	varExpStrTmp = ssVarValue.str();

	CRegexpT<TCHAR> hexRegexp(TEXT("^((0x[0-9A-Fa-f]+)|([0-9]+))"));
	CRegexpT<TCHAR> sepRegexp(TEXT("^[\\s+\\-*/()]"));
	basic_string<TCHAR> varExpStr = TEXT("^") + varExpStrTmp + TEXT("(($)|([+\\-*/)\\s]))");
	CRegexpT<TCHAR> varRegexp(varExpStr.c_str());
	MatchResult result;

	ssVarValue.str(TEXT(""));
	if (varValue < 0)
	{
		ssVarValue << TEXT("(0") << varValue << TEXT(")");
	}
	else
	{
		ssVarValue << varValue;
	}
	basic_string<TCHAR> strValue = ssVarValue.str();

	for (size_t i = 0; i < arithmExp.size(); )
	{
		basic_string<TCHAR> temp = arithmExp.substr(i);
		result = hexRegexp.Match(temp.c_str());
		if (result.IsMatched())
		{
			i = result.GetEnd() + i;
			continue;
		}
		result = sepRegexp.Match(temp.c_str());
		if (result.IsMatched())
		{
			i = result.GetEnd() + i;
			continue;
		}
		result = varRegexp.Match(temp.c_str());
		if (result.IsMatched())
		{
			arithmExp.replace(i, variable.size(), strValue);
			i += strValue.size();
		}
		else
		{
			do 
			{
				i++;
				temp = arithmExp.substr(i);
				result = sepRegexp.Match(temp.c_str());
			} while (!(result.IsMatched() || temp.empty()));
		}
	}
}

double CCalculator::Calculate(const TCHAR *pszExp)
{
	TransExpression(pszExp);
	CreateArithmetic();
	if (m_Arithm != NULL)
	{
		return m_Arithm->GetResult();
	}
	return 0;
}

void CCalculator::TransExpression(const TCHAR *pszExp)
{
	double iTmp = 0;
	TCHAR chTmp = TEXT('\0');
	list<TCHAR> OperatorStack;
	basic_string<TCHAR> funcExpTmp;
	list<basic_string<TCHAR> > FuncExpStack;
	bool flag = true;

	for (; *pszExp != TEXT('\0'); pszExp++)
	{
		if (isspace(*pszExp))
		{
			continue;
		}
		if (isdigit(*pszExp) || (flag && *pszExp == TEXT('-')))
		{
			pszExp += (GetNumber(pszExp, iTmp) - 1);
			m_OpMixStack.push_back(COpMixNode(iTmp, TEXT("")));
			flag = false;
		}
		else if ( islower(*pszExp) )
		{
			pszExp += (GetFunction(pszExp, funcExpTmp) - 1);
			flag = false;
		}
		else if (*pszExp == TEXT(')'))
		{
			for (chTmp = OperatorStack.back(), OperatorStack.pop_back(), funcExpTmp = FuncExpStack.back(), FuncExpStack.pop_back(); 
				chTmp != TEXT('(') || ( chTmp == TEXT('(') && !funcExpTmp.empty()); 
				chTmp = OperatorStack.back(), OperatorStack.pop_back(), funcExpTmp = FuncExpStack.back(), FuncExpStack.pop_back() )
			{
				if (chTmp != TEXT('('))
				{
					m_OpMixStack.push_back(COpMixNode((TCHAR)chTmp, funcExpTmp));
				}
				else if ( chTmp == TEXT('(') && !funcExpTmp.empty() && !m_OpMixStack.empty())
				{
					list<COpMixNode>::iterator it = m_OpMixStack.end();
					it--;
					it->m_funcExp += funcExpTmp;
					funcExpTmp = TEXT("");
					break;
				}

				if (OperatorStack.empty() || FuncExpStack.empty())
				{
					break;
				}
			}
			flag = false;
		}
		else
		{
			if (*pszExp == TEXT('('))
			{
				flag = true;
			}
			if (ComparePriority(*pszExp, OperatorStack) > 0)
			{
				OperatorStack.push_back(*pszExp);
				FuncExpStack.push_back(funcExpTmp);
				funcExpTmp = TEXT("");
			}
			else
			{
				do 
				{
					m_OpMixStack.push_back(COpMixNode(OperatorStack.back(), TEXT("")));
					OperatorStack.pop_back();
					FuncExpStack.pop_back();
				} while (ComparePriority(*pszExp, OperatorStack) <= 0);

				OperatorStack.push_back(*pszExp);
				FuncExpStack.push_back(funcExpTmp);
				funcExpTmp = TEXT("");
			}
		}
	}

	while (!OperatorStack.empty())
	{
		m_OpMixStack.push_back(COpMixNode(OperatorStack.back(), TEXT("")));
		OperatorStack.pop_back();
		FuncExpStack.pop_back();
	}

	m_OpMixStack.reverse();

	return ;
}

int CCalculator::ComparePriority(TCHAR chOperator, list<TCHAR> &OperatorStack)
{
	int icp, isp;

	icp = GetPriority(chOperator, false);

	if (OperatorStack.empty())
	{
		isp = 0;
	}
	else
	{
		isp = GetPriority(OperatorStack.back(), true);
	}

	if (icp > isp)
	{
		return 1;
	}

	if (icp < isp)
	{
		return -1;
	}

	return 0;
}

int CCalculator::GetPriority(TCHAR chOperator, bool bIsp)
{
	switch (chOperator)
	{
	case TEXT('+'):
	case TEXT('-'):
		return (bIsp == true ? 3 : 2);
	case TEXT('*'):
	case TEXT('/'):
	case TEXT('%'):
		return (bIsp == true ? 5 : 4);
	case TEXT('^'):
		return (bIsp == true ? 7 : 6);
	case TEXT('('):
		return (bIsp == true ? 1 : 8);
	case TEXT(')'):
		return (bIsp == true ? 8 : 1);
	default:
		return 0;
	}
}

int CCalculator::GetNumber(const TCHAR *pszStr, double &iRetNum)
{
	int iRet = 0;
	basic_string<TCHAR> strNum;

	CRegexpT<TCHAR> regexp(TEXT("^([+\\-])?((0x[0-9A-Fa-f]+)|(([0-9]+)(\\.([0-9]+)?)?([Ee][+\\-][0-9]+)?))"));
	MatchResult result;

	result = regexp.Match(pszStr);
	if (result.IsMatched())
	{
		for (int i = 0; i < result.GetEnd(); i++)
		{
			strNum += (*pszStr);
			pszStr++;
			iRet++;
		}
	}

#ifdef UNICODE
	iRetNum = wcstod(strNum.c_str(), NULL);
#else
	iRetNum = strtod(strNum.c_str(), NULL);
#endif

	return iRet;
}

int CCalculator::GetFunction( const TCHAR *pszStr, double &iRetNum )
{
	int iRet = 0;

	CRegexpT<TCHAR> regexp(TEXT("^(((sin)|(cos)|(tan)|(acos)|(asin)|(atan)|(log)|(sqrt))(\\())(([+\\-])?((0x[0-9A-Fa-f]+)|(([0-9]+)(\\.[0-9]+)?([Ee][+\\-][0-9]+)?)))(\\))"));
	MatchResult result;

	result = regexp.Match(pszStr);
	if (result.IsMatched())
	{
		iRet += result.GetEnd();
	}

	basic_string<TCHAR> arithmExp = pszStr;
	CRegexpT<TCHAR> funcRegexp(TEXT("^((sin)|(cos)|(tan)|(acos)|(asin)|(atan)|(log)|(sqrt))(\\()"));

	result = funcRegexp.Match(pszStr);
	if (result.IsMatched())
	{
		pszStr += result.GetEnd();
	}

	GetNumber(pszStr, iRetNum);

	if (arithmExp.find(TEXT("sin")) == 0)
	{
		iRetNum = sin(iRetNum);
	}
	else if (arithmExp.find(TEXT("cos")) == 0)
	{
		iRetNum = cos(iRetNum);
	}
	else if (arithmExp.find(TEXT("tan")) == 0)
	{
		iRetNum = tan(iRetNum);
	}
	else if (arithmExp.find(TEXT("asin")) == 0)
	{
		iRetNum = asin(iRetNum);
	}
	else if (arithmExp.find(TEXT("acos")) == 0)
	{
		iRetNum = acos(iRetNum);
	}
	else if (arithmExp.find(TEXT("atan")) == 0)
	{
		iRetNum = atan(iRetNum);
	}
	else if (arithmExp.find(TEXT("log")) == 0)
	{
		iRetNum = log(iRetNum);
	}
	else if (arithmExp.find(TEXT("sqrt")) == 0)
	{
		iRetNum = sqrt(iRetNum);
	}

	return iRet;
}

int CCalculator::GetFunction( const TCHAR *pszStr, basic_string<TCHAR> &funcStr )
{
	int iRet = 0;

	CRegexpT<TCHAR> regexp(TEXT("^(sin)|(cos)|(tan)|(acos)|(asin)|(atan)|(log)|(sqrt)"));
	MatchResult result;

	result = regexp.Match(pszStr);
	if (result.IsMatched())
	{
		iRet += result.GetEnd();
	}

	basic_string<TCHAR> arithmExp = pszStr;

	if (arithmExp.find(TEXT("sin")) == 0)
	{
		funcStr = TEXT("sin");
	}
	else if (arithmExp.find(TEXT("cos")) == 0)
	{
		funcStr = TEXT("cos");
	}
	else if (arithmExp.find(TEXT("tan")) == 0)
	{
		funcStr = TEXT("tan");
	}
	else if (arithmExp.find(TEXT("asin")) == 0)
	{
		funcStr = TEXT("asin");
	}
	else if (arithmExp.find(TEXT("acos")) == 0)
	{
		funcStr = TEXT("acos");
	}
	else if (arithmExp.find(TEXT("atan")) == 0)
	{
		funcStr = TEXT("atan");
	}
	else if (arithmExp.find(TEXT("log")) == 0)
	{
		funcStr = TEXT("log");
	}
	else if (arithmExp.find(TEXT("sqrt")) == 0)
	{
		funcStr = TEXT("sqrt");
	}

	return iRet;
}

void CCalculator::CreateArithmetic()
{
	if (m_OpMixStack.empty())
	{
		return ;
	}

	list<CArithmetic *> tmpStack;
	COpMixNode MixNode(TEXT('\0'), TEXT(""));

	while(!m_OpMixStack.empty())
	{
		MixNode = m_OpMixStack.back();
		m_OpMixStack.pop_back();
		if (MixNode.m_bOperator)
		{
			CArithmetic *pRight = tmpStack.back();
			tmpStack.pop_back();
			CArithmetic *pLeft = tmpStack.back();
			tmpStack.pop_back();
			tmpStack.push_back(new CArithmetic(MixNode.m_chOperator, MixNode.m_funcExp.c_str(), pLeft, pRight));
		}
		else
		{
			tmpStack.push_back(new CArithmetic(MixNode.m_iOperand, MixNode.m_funcExp.c_str()));
		}
	}

	if (m_Arithm != NULL)
	{
		delete m_Arithm;
	}

	m_Arithm = tmpStack.back();
	tmpStack.pop_back();
}

void CCalculator::Output(_ostream_type &os)
{
	if (m_Arithm != NULL)
	{
		m_Arithm->Output(os);
		os << TEXT('=');
		os << m_Arithm->GetResult();
	}

	os << endl;
}

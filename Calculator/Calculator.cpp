// Calculator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Arithmetic.h"
#include "deelx.hpp"
#include <iostream>
#include <fstream>
#include <Windows.h>

bool W2C(const wchar_t* wstr, char* str)  
{  
   int len=WideCharToMultiByte(CP_OEMCP,0,wstr,-1/*null terminated*/,str,0,0,0);  
   return len==WideCharToMultiByte(CP_OEMCP,0,wstr,-1/*null terminated*/,str,len,0,0);  
}

int _tmain( int argc, _TCHAR* argv[] )
{
	basic_string<TCHAR> arithmExp;
	CCalculator calculator;

	do 
	{
		std::wcin >> arithmExp;

		if (arithmExp == TEXT("end"))
		{
			break;
		}

		std::wcout << TEXT("= ") << calculator.Calculate(arithmExp.c_str()) << endl;
	} while (true);

	return 0;
}


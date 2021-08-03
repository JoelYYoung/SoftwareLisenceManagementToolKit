#pragma once
#include "AESEncryption.h"
#include "lisencemanager.h"
#include "PCInfo.h"
#include <iostream>
#include <cstdio>
#include <string>
using namespace std;

/**********************************************************************/
/*                                                                    */
/*                              ≤‚ ‘                                  */
/*                                                                    */
/**********************************************************************/
int main()
{
	string strCpuInfo = cpuInfo();
	vector<string> vecHarddiskInfo = harddiskInfo();
	string strLisenceEncrypted = generateLisence(strCpuInfo, vecHarddiskInfo, 12);
	validity result = validateLisence(strLisenceEncrypted);
	cout << "Result is" << result << endl;
}
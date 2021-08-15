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
	string strMacAddr = macAddr();
	cout << "strMacAddr is " << strMacAddr << endl;
	cout << "length of strMacAddr is " << strMacAddr.length() << endl;
	cout << "strCpuInfo is " << strCpuInfo << endl;
	cout << "length of strCpuInfo is " << strCpuInfo.length() << endl;
	string strLisence = generateLisence(strCpuInfo, strMacAddr, 12);
	validity result = validateLisence(strLisence);
	cout << "validity: " << result << endl;
	getchar();
	getchar();
}
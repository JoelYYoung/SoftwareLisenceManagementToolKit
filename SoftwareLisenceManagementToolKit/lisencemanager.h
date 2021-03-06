#pragma once
#include <string>
#include <vector>
using namespace std;

enum validity { VALID = 0, CPUNOTMATCH = 10, MACNOTMATCH = 20, OVERDUE = 30 };

string generateLisence(const string& strCpuInfo, const vector<string>& strMacAddr, unsigned int permissionTime);
validity validateLisence(const string& strLisenceEncrypted);
#pragma once
#include <string>
#include <vector>
using namespace std;

enum validity { VALID = 0, CPUNOTMATCH = 10, HARDDISKNOTMATCH = 20, OVERDUE = 20 };

string generateLisence(const string& strCpuInfo, vector<string>& vecHarddiskInfo, unsigned int permissionTime);
validity validateLisence(const string& strLisenceEncrypted);
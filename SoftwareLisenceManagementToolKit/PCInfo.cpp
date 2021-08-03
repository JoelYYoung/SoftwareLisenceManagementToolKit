/*
* get information of the PC, including:
* CPU serial number, harddisk serial number, present time
*/

#pragma warning(disable:4996)
#include <sstream>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/timeb.h>
using namespace std;

string getCmdResult(const string& strCmd);
vector<string> split(const string& str, const string& delim);


/*
* CPUInfo
* 
* get CPU serial number and return string
*/
string cpuInfo() {
    string strCmd = "wmic cpu get processorid";
    string strRe = getCmdResult(strCmd);
    const string strSpl = "\n";
    vector<string> vecRe = split(strRe, strSpl);
    string strReturn = vecRe[1];

    //删除字符串中的空格和换行键
    auto strIt = strReturn.begin();
    while (strIt != strReturn.end())
    {
        if (*strIt == '\r' || *strIt == ' ')
        {
            strReturn.erase(strIt);
            continue;
        }
        strIt++;
    }
    return strReturn;
}


/*
* harddiskInfo
* 
* get harddiskInfo and return a vector reference
*/
vector<string> harddiskInfo() {
    string strCmd = "wmic diskdrive get Serialnumber";
    string strRe = getCmdResult(strCmd);
    const string strSpl = "\n";
    vector<string> vecRe = split(strRe, strSpl);
    vecRe.erase(vecRe.begin());

    //遍历整个vector
    auto vIt =  vecRe.begin();
    auto vEnd = vecRe.end();
    while (vIt != vEnd) {
        //删除字符串中的空格和换行键
        auto strIt = (*vIt).begin();
        while (strIt != (*vIt).end())
        {
            if (*strIt == '\r' || *strIt == ' ')
            {
                (*vIt).erase(strIt);
                continue;
            }
            strIt++;
        }
        vIt++;
    }
    if ((*(vecRe.end() - 1)).length() == 0) {
        vecRe.erase(vecRe.end() - 1);
    }

    return vecRe;
}


/*
* getTime
* 
* get present time and return the timestamp
*/
string getTime() {
    struct timeb rawtime;
    ftime(&rawtime);
    unsigned long long time = (unsigned long long)(rawtime.time) * 1000 + rawtime.millitm;
    
    ostringstream stream;
    stream << time;
    string strTime = stream.str();
    return strTime;
}

/*
* getCmdResult
* 
* input commond line and return result as string
*/
string getCmdResult(const string& strCmd)
{
    char buf[10240] = { 0 };
    FILE* pf = NULL;
    if ((pf = _popen(strCmd.c_str(), "r")) == NULL)
    {
        return "";
    }
    string strResult;
    while (fgets(buf, sizeof buf, pf))
    {
        strResult += buf;
    }
    _pclose(pf);

    unsigned int iSize = strResult.size();
    if (iSize > 0 && strResult[iSize - 1] == '\n')  // linux
    {
        strResult = strResult.substr(0, iSize - 1);
    }
    
    return strResult;
}


/*
* split
* 
* split string and return a vector
*/
vector<string> split(const string& str, const string& delim) {
    vector<string> res;
    if ("" == str) return res;
    //先将要切割的字符串从string类型转换为char*类型
    char* strs = new char[str.length() + 1]; //不要忘了
    strcpy(strs, str.c_str());

    char* d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char* p = strtok(strs, d);
    while (p) {
        string s = p; //分割得到的字符串转换为string类型
        res.push_back(s); //存入结果数组
        p = strtok(NULL, d);
    }

    return res;
}
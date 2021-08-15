/**********************************************************************/
/* Module description: get information of the PC, including:          */
/* CPU serial number, harddisk serial number, present time.           */
/*                                                                    */
/* Note: the code has been modified to be adapted to both Linux32/64  */
/* and Windows32/64 platform.                                         */
/**********************************************************************/

#pragma warning(disable:4996)
#include "PCInfo.h"
#include <sstream>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#ifdef _MSC_VER //Windows platform
	#include <Windows.h>
	#include <Iphlpapi.h>
	#include <Assert.h>
	#pragma comment(lib, "iphlpapi.lib")
    #include <vector>
    #include <bitset>
    #include <array>
    #include <intrin.h>
#else //Linux platform
    #include <sys/ioctl.h>
    #include <net/if.h> 
    #include <unistd.h>
    #include <netinet/in.h>
    #include <string.h>
#endif
#include <sys/timeb.h>
using namespace std;


/*
* Function name: native_cpuid
* Description: get cpuid and return string
*/
#ifdef __linux__
// reference: https://stackoverflow.com/questions/6491566/getting-the-machine-serial-number-and-cpu-id-using-c-c-in-linux
void native_cpuid(unsigned int* eax, unsigned int* ebx, unsigned int* ecx, unsigned int* edx)
{
    // ecx is often an input as well as an output
    asm volatile("cpuid"
        : "=a" (*eax),
        "=b" (*ebx),
        "=c" (*ecx),
        "=d" (*edx)
        : "0" (*eax), "2" (*ecx));
}
#endif

/*
* Function name: CPUInfo
* Description: get CPU serial number and return string
*/
string cpuInfo() {
    string strReturn;

    #ifdef _MSC_VER
        // reference: https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=vs-2019
        char vendor[0x20];
        memset(vendor, 0, sizeof(vendor));
        
        std::array<int, 4> cpui;
        // Calling __cpuid with 0x0 as the function_id argument gets the number of the highest valid function ID
        __cpuid(cpui.data(), 0);
        int nIds_ = cpui[0];
    
        std::vector<std::array<int, 4>> data_;
        for (int i = 0; i <= nIds_; ++i) {
            __cpuidex(cpui.data(), i, 0);
            data_.push_back(cpui);
    
            //fprintf(stdout, "%08X-%08X-%08X-%08X\n", cpui[0], cpui[1], cpui[2], cpui[3]);
        }
    
        *reinterpret_cast<int*>(vendor) = data_[0][1];
        *reinterpret_cast<int*>(vendor + 4) = data_[0][3];
        *reinterpret_cast<int*>(vendor + 8) = data_[0][2];
        //fprintf(stdout, "vendor: %s\n", vendor); // GenuineIntel or AuthenticAMD or other
        //fprintf(stdout, "vendor serialnumber: %08X%08X\n", data_[1][3], data_[1][0]);
        char cpuInfo[50];
        sprintf(cpuInfo, "%08X%08X", data_[1][3], data_[1][0]);
        string strCpuInfo = cpuInfo;
        return strCpuInfo;
    #else
        unsigned eax, ebx, ecx, edx;
    
        char vendor[0x20];
        memset(vendor, 0, sizeof(vendor));

        eax = 0; // processor info and feature bits
        native_cpuid(&eax, &ebx, &ecx, &edx);
        //fprintf(stdout, "%d, %d, %d, %d\n", eax, ebx, ecx, edx);
    
        *reinterpret_cast<int*>(vendor) = ebx;
        *reinterpret_cast<int*>(vendor + 4) = edx;
        *reinterpret_cast<int*>(vendor + 8) = ecx;
        //fprintf(stdout, "vendor: %s\n", vendor); // GenuineIntel or AuthenticAMD or other
    
        eax = 1; // processor serial number
        native_cpuid(&eax, &ebx, &ecx, &edx);
    
        // see the CPUID Wikipedia article on which models return the serial number in which registers
        //printf("vendor serialnumber: %08X%08X\n", edx, eax);
        char cpuInfo[50];
        sprintf(cpuInfo, "%08X%08X", edx, eax);
        string strCpuInfo = cpuInfo;
        return strCpuInfo;
    #endif

    return strReturn;
}


/*
* Function Name: macAddr
* Descrition: get MAC address and return string
*/
string macAddr() {
#ifdef _MSC_VER
	// reference: https://stackoverflow.com/questions/13646621/how-to-get-mac-address-in-windows-with-c
    PIP_ADAPTER_INFO AdapterInfo;
    DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
    char* mac_addr = (char*)malloc(18);

    AdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    if (AdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        free(mac_addr);
        return NULL; // it is safe to call free(NULL)
    }

    // Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
    if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(AdapterInfo);
        AdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);
        if (AdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            free(mac_addr);
            return NULL;
        }
    }

    if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
        // Contains pointer to current adapter info
        PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
        do {
            // technically should look at pAdapterInfo->AddressLength
            //   and not assume it is 6.
            sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
                pAdapterInfo->Address[0], pAdapterInfo->Address[1],
                pAdapterInfo->Address[2], pAdapterInfo->Address[3],
                pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
            
            // printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
            // return the last one.
            // return mac_addr;

            //printf("\n");
            pAdapterInfo = pAdapterInfo->Next;
        } while (pAdapterInfo);
    }
    free(AdapterInfo);
    string strMacAddr = mac_addr;
    return strMacAddr;
#else
	// reference: https://stackoverflow.com/questions/1779715/how-to-get-mac-address-of-your-machine-using-a-c-program/35242525
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		fprintf(stderr, "fail to socket: %d\n", sock);
		return NULL;
	};

	struct ifconf ifc;
	char buf[1024];
	int success = 0;

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
		fprintf(stderr, "fail to ioctl: SIOCGIFCONF\n");
		return NULL;
	}

	struct ifreq* it = ifc.ifc_req;
	const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));
	struct ifreq ifr;

	for (; it != end; ++it) {
		strcpy(ifr.ifr_name, it->ifr_name);
		if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
			if (!(ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
				if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
					success = 1;
					break;
				}
		}
	}
		else {
			fprintf(stderr, "fail to ioctl: SIOCGIFFLAGS\n");
			return NULL;
		}
}

	unsigned char mac_address[6];
	if (success) memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
    char* mac_addr = (char*)malloc(18);
	sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X", mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);
    string strMacAddr = mac_addr;
    return strMacAddr;
#endif
}


/*
* Function name: harddiskInfo(abandoned)
* Description: get harddiskInfo and return a vector reference
*/
//vector<string> harddiskInfo() {
//    string strCmd = "wmic diskdrive get Serialnumber";
//    string strRe = getCmdResult(strCmd);
//    const string strSpl = "\n";
//    vector<string> vecRe = split(strRe, strSpl);
//    vecRe.erase(vecRe.begin());
//
//    //遍历整个vector
//    auto vIt =  vecRe.begin();
//    auto vEnd = vecRe.end();
//    while (vIt != vEnd) {
//        //删除字符串中的空格和换行键
//        auto strIt = (*vIt).begin();
//        while (strIt != (*vIt).end())
//        {
//            if (*strIt == '\r' || *strIt == ' ')
//            {
//                (*vIt).erase(strIt);
//                continue;
//            }
//            strIt++;
//        }
//        vIt++;
//    }
//    if ((*(vecRe.end() - 1)).length() == 0) {
//        vecRe.erase(vecRe.end() - 1);
//    }
//
//    return vecRe;
//}


/*
* Function Name: getTime
* Description: get present time and return the timestamp
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
* Function Name: getCmdResult(abandoned)
* Description: input commond line and return result as string
*/
//string getCmdResult(const string& strCmd)
//{
//    char buf[10240] = { 0 };
//    FILE* pf = NULL;
//    if ((pf = _popen(strCmd.c_str(), "r")) == NULL)
//    {
//        return "";
//    }
//    string strResult;
//    while (fgets(buf, sizeof buf, pf))
//    {
//        strResult += buf;
//    }
//    _pclose(pf);
//
//    unsigned int iSize = strResult.size();
//    if (iSize > 0 && strResult[iSize - 1] == '\n')  // linux
//    {
//        strResult = strResult.substr(0, iSize - 1);
//    }
//    
//    return strResult;
//}


/*
* Function Name: split
* Description: split string and return a vector
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
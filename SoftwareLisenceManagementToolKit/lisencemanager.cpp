/**********************************************************************/
/* Module description: Generate lisence and check validity of		  */
/* lisence.															  */
/**********************************************************************/

#pragma warning(disable:4996)
#include "lisencemanager.h"
#include <string>
#include <vector>
#include <sstream>
#include "AESEncryption.h"
#include "PCInfo.h"
#include "cstdio"
using namespace std;

#define TIMECOEFFICIENT 2592000000

bool checkMacAddr(vector<string> vecReal, vector<string> vecLisence);

/*
* Function name: generateLisence
* Description: 
*	input permission time and machine information, generate
*	lisence string. Use RSA-AES algorithm to encrypt it and 
*   return the string result
*/
string generateLisence(const string& strCpuInfo, const vector<string>& vecMacAddr, unsigned int permissionTime) {
	string timestamp = getTime();
	ostringstream stream; //��permissionTimeת��Ϊstring
	stream << permissionTime;
	string strPermissionTime = stream.str();

	//ճ��������Ҫ����Lisence�����ԭʼ��Ϣ
	string strLisenceRaw;
	strLisenceRaw += strCpuInfo; //���Cpu��Ϣ
	//abandoned
	//auto vecIt = vecHarddiskInfo.begin();
	//auto vecEnd = vecHarddiskInfo.end();
	//while (vecIt != vecEnd) {
	//	strLisenceRaw += '\n';
	//	strLisenceRaw += *vecIt; //���Harddisk��Ϣ
	//	vecIt++;
	//}
	auto macAddrP = vecMacAddr.begin();
	while (macAddrP != vecMacAddr.end()) {
		strLisenceRaw += '\n';
		strLisenceRaw += *macAddrP;
		macAddrP++;
	}
	strLisenceRaw += '\n';
	strLisenceRaw += timestamp; //�������Lisenceʱ��ȡ��timestamp
	strLisenceRaw += '\n';
	strLisenceRaw += strPermissionTime; //������õ�ʹ��ʱ������λ�£�ֵС��int���ޣ�
	
	//ʹ��AES�㷨��ԭʼ��Ϣ���м���
	string strLisenceEncrypted;
	byte key[16] = { 0x2b, 0x7e, 0x15, 0x16,
					0x28, 0xae, 0xd2, 0xa6,
					0xab, 0xf7, 0x15, 0x88,
					0x09, 0xcf, 0x4f, 0x3c };
	word w[4 * (Nr + 1)];
	KeyExpansion(key, w); //��չ�Ĺ�Կ
	byte plain[16] = { 0 };
	int rawLength = strLisenceRaw.length();
	int round = rawLength / 16;
	int remain = rawLength % 16; //��str����ת��Ϊ16�ı���
	if (remain != 0) {
		round += 1;
	}
	for (int i = 0; i < round; i++) {
		for (int j = 0; j < 16; j++) {
			//cout << "i = " << i << endl;
			if (i * 16 + j >= rawLength) {
				plain[j] = 0;
			}
			else {
				plain[j] = (byte)(strLisenceRaw[i * 16 + j]);
			}
		}
		encrypt(plain, w);
		for (int j = 0; j < 16; j++) {
			strLisenceEncrypted += (char)plain[j].to_ulong();
		}
	}

	return strLisenceEncrypted;
}

/*
* Function Name: validateLisence
* Description: 
*	input lisence file CONTENT(Encrypted), validate if 
*	lisence is valid on user's machine within permitted 
*	deadline
*/
validity validateLisence(const string& strLisenceEncrypted) {
	//�Լ����ַ������н���
	string strLisenceRaw;
	int round = strLisenceEncrypted.length() / 16;
	byte key[16] = { 0x2b, 0x7e, 0x15, 0x16,
					0x28, 0xae, 0xd2, 0xa6,
					0xab, 0xf7, 0x15, 0x88,
					0x09, 0xcf, 0x4f, 0x3c };
	word w[4 * (Nr + 1)];
	KeyExpansion(key, w); //��չ�Ĺ�Կ
	byte encrypted[16] = { 0 };
	for (int i = 0; i < round; i++) {
		for (int j = 0; j < 16; j++) {
			encrypted[j] = strLisenceEncrypted[i * 16 + j];
		}
		decrypt(encrypted, w);
		for (int i = 0; i < 16; i++) {
			strLisenceRaw += (char)encrypted[i].to_ulong();
		}
	}
	//���ַ����еõ�CPUSn��harddiskSn���Լ�ʱ����Ϣ
	//printf("Validate: strLisenceRaw is %s\n", strLisenceRaw.c_str());
	vector<string> info = split(strLisenceRaw, "\n");
	
	string strCpuInfo = info[0];
	vector<string> vecMacAddr(info.begin()+1, info.end()-2);
	string timestamp = *(info.end()-2);
	string strTimePermission = *(info.end()-1);
	unsigned long long llTimestamp = stoll(timestamp);
	int timePermission = stoi(strTimePermission);

	string strCpuInfo_real = cpuInfo();
	vector<string> vecMacAddr_real = macAddr();

	//�жϻ���ָ���Ƿ��Ǻ�
	if (strCpuInfo.compare(strCpuInfo_real) != 0)return CPUNOTMATCH;
	if (!checkMacAddr(vecMacAddr, vecMacAddr_real))return MACNOTMATCH;
	//�ж��Ƿ��ڹ涨��ʱ����
	unsigned long long presentTime = stoll(getTime());
	if (presentTime > llTimestamp + (long long)timePermission * TIMECOEFFICIENT) return OVERDUE;

	return VALID;
}

/*
* Function name:
* Description: 
*/

bool checkMacAddr(vector<string> vecReal, vector<string> vecLisence) {
	auto realP = vecReal.begin();
	while (realP != vecReal.end()) {
		auto lisenceP = vecLisence.begin();
		while (lisenceP != vecLisence.end()) {
			if ((*realP).compare((*lisenceP)) == 0) {
				return true;
			}
			lisenceP++;
		}
		realP++;
	}
	return false;
}
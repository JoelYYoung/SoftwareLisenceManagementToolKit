/**********************************************************************/
/*                                                                    */
/*       Generate lisence and check validity of lisence               */
/*                                                                    */
/**********************************************************************/
#pragma warning(disable:4996)
#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "AESEncryption.h"
#include "PCInfo.h"
using namespace std;

#define TIMECOEFFICIENT 2592000000
enum validity {VALID = 0, CPUNOTMATCH = 10, HARDDISKNOTMATCH = 20, OVERDUE = 20};


/*
* generateLisence
*
* input permission time and machine information, generate
* lisence string. Use RSA-AES algorithm to encrypt it and 
* return the string result.
*/
string generateLisence(const string strCpuInfo, vector<string>& vecHarddiskInfo, unsigned int permissionTime) {
	string timestamp = getTime();
	ostringstream stream; //��permissionTimeת��Ϊstring
	stream << permissionTime;
	string strPermissionTime = stream.str();

	//ճ��������Ҫ����Lisence�����ԭʼ��Ϣ
	string strLisenceRaw;
	strLisenceRaw += strCpuInfo; //���Cpu��Ϣ
	auto vecIt = vecHarddiskInfo.begin();
	auto vecEnd = vecHarddiskInfo.end();
	while (vecIt != vecEnd) {
		strLisenceRaw += '\n';
		strLisenceRaw += *vecIt; //���Harddisk��Ϣ
		vecIt++;
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
	int remain = rawLength % 16; //��str����ת��Ϊ16�ı���
	if (remain != 0) {
		string append = (16 - remain, "\0");
		strLisenceRaw += append;
	}
	int round = rawLength / 16;
	for (int i = 0; i < round; i++) {
		for (int j = 0; j < 16; j++) {
			plain[j] = (byte)(strLisenceRaw[i * 16 + j]);
		}
		encrypt(plain, w);
		for (int j = 0; j < 16; j++) {
			strLisenceEncrypted += (char)plain[j].to_ulong();
		}
	}

	return strLisenceEncrypted;
}

/*
* validateLisence
*
* input lisence file CONTENT(Encrypted), validate if lisence is valid on user's
* machine within permitted deadline.
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
	vector<string> info = split(strLisenceRaw, "\n");
	
	string strCpuInfo = info[0];
	string timestamp = *(info.end() - 2);
	string strTimePermission = *(info.end() - 1);
	vector<string> vecHarddiskInfo(info.begin() + 1, info.end() - 2);
	unsigned long long llTimestamp = stoll(timestamp);
	int timePermission = stoi(strTimePermission);

	string strCpuInfo_real = cpuInfo();
	vector<string> vecHarddiskInfo_real = harddiskInfo();

	//�жϻ���ָ���Ƿ��Ǻ�
	if (strCpuInfo.compare(strCpuInfo_real) != 0)return CPUNOTMATCH;
	int vecLength = vecHarddiskInfo.size();
	int vecLength_real = vecHarddiskInfo_real.size();
	if (vecLength != vecLength_real) return HARDDISKNOTMATCH;
	for (int i = 0; i < vecLength; i++) {
		if (vecHarddiskInfo[i].compare(vecHarddiskInfo_real[i]) != 0) return HARDDISKNOTMATCH;
	}
	//�ж��Ƿ��ڹ涨��ʱ����
	unsigned long long presentTime = stoll(getTime());
	if (presentTime > llTimestamp + (long long)timePermission * TIMECOEFFICIENT) return OVERDUE;

	return VALID;
}
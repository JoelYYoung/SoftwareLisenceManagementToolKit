#pragma once
#include <iostream>
#include <bitset>
#include <string>
using namespace std;

typedef bitset<8> byte;
typedef bitset<32> word;

#define Nr 10  // AES-128需要 10 轮加密
#define Nk 4   // Nk 表示输入密钥的 word 个数

void KeyExpansion(byte key[4 * Nk], word w[4 * (Nr + 1)]);
void encrypt(byte in[4 * 4], word w[4 * (Nr + 1)]);
void decrypt(byte in[4 * 4], word w[4 * (Nr + 1)]);
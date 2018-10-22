#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>
#include "DES.h"
using namespace std;

void Copy(char *dst, const char *src, int n)
{
    for (int i = 0; i < n; i++)
        dst[i] = src[i];
}

int BitsSplit(char bytes[], int n, char bits[], int base = BASE)
{
    for (int i = 0; i < n; i++)
        for (int j = base - 1, x = bytes[i]; j >= 0; j--, x >>= 1)
            bits[i * base + j] = x & 1;
    return n * base;
}

int BitsMerge(char bits[], int n, char bytes[])
{
    int cnt = 0;
    for (int i = 0; i < n; i += BASE)
    {
        int x = 0;
        for (int j = 0; j < BASE; j++)
            x = (x << 1) | bits[i + j];
        bytes[cnt++] = x;
    }
    return cnt;
}

void cshl(char bits[], int n, int k)
{
    char Temp[n];
    for (int i = 0; i < n; i++)
        Temp[i] = bits[(i + k) % n];
    Copy(bits, Temp, n);
}

void Permutate(const char src[], const int n, char dst[], int m, int P[])
{
    for (int i = 0; i < m; i++)
        dst[i] = src[P[i]];
}

void KeysGeneration(char key[], bool isDecode)
{
    char bits[KEY_SIZE], C[MKEY_SIZE];
    BitsSplit(key, KEY_BYTE, bits);
    Permutate(bits, KEY_SIZE, C, MKEY_SIZE, IPC1);
    for (int i = 0; i < ITER_NUM; i++)
    {
        cshl(C, MKEY_SIZE >> 1, LS[i]);
        cshl(C + (MKEY_SIZE >> 1), MKEY_SIZE >> 1, LS[i]);
        int p = (isDecode) ? ITER_NUM - i - 1 : i;
        Permutate(C, MKEY_SIZE, Keys[p], SKEY_SIZE, IPC2);
    }
}

void XOR(char dst[], const char a[], const char b[], int n)
{
    for (int i = 0; i < n; i++)
        dst[i] = a[i] ^ b[i];
}

char *F(char bits[], char Key[])
{
    char B[SKEY_SIZE], C[SKEY_SIZE];
    Permutate(bits, HALF_BLK, C, SKEY_SIZE, PE1);
    XOR(B, C, Key, SKEY_SIZE);
    for (int i = 0, k = 0; i < SKEY_SIZE; i += S_SIZE, k++)
    {
        int r = (B[i] << 1) | B[i + 5], c = 0;
        for (int j = 1; j <= 4; j++)
            c = (c << 1) | B[i + j];
        char temp[1] = {S[k][r][c]};
        BitsSplit(temp, 1, C + 4 * k, 4);
    }
    Permutate(C, HALF_BLK, bits, HALF_BLK, PE2);
    return bits;
}

void Switch(char bits[], int n)
{
    int m = n >> 1;
    for (int i = 0; i < m; i++)
        swap(bits[i], bits[i + m]);
}

void Run(char bytes[])
{
    char B[BLK_SIZE], C[BLK_SIZE];
    int n = BitsSplit(bytes, BLK_BYTE, B);
    print(B, BLK_SIZE);
    Permutate(B, BLK_SIZE, C, BLK_SIZE, IP1);
    for (int i = 0; i < ITER_NUM; i++)
    {
        char *L = C, *R = C + HALF_BLK;
        Copy(B, R, HALF_BLK);
        XOR(B + HALF_BLK, L, F(R, Keys[i]), HALF_BLK);
        Copy(C, B, BLK_SIZE);
    }
    Switch(C, BLK_SIZE);
    Permutate(C, BLK_SIZE, B, BLK_SIZE, IP2);
    int m = BitsMerge(B, BLK_SIZE, bytes);
}

int main(int argc, char **argv)
{
    char key[KEY_BYTE + 5] = "abcdefgh";
    bool isDecode = true;
    KeysGeneration(key, isDecode);

    FILE *fin = fopen("TEST/test.txt", "r");
    FILE *fout = fopen("TEST/test.des", "w");

    char block[BLK_BYTE];
    for (int cnt = 0; (cnt = fread(block, 1, BLK_BYTE, fin)) != 0;)
    {
        for (int i = cnt; i < BLK_BYTE; i++)
            block[i] = 0;
        printf("%s\n", block);
        Run(block);
        for (int i = 0; i < BLK_BYTE; i++)
            printf("%x ", block[i]);
        printf("\n");
        fwrite(block, 1, BLK_BYTE, fout); //decode????
    }
    fclose(fin);
    fclose(fout);
    return 0;
}

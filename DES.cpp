#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>
#include <time.h>
#include <unistd.h>
#include "DES.h"
#include "Util.h"
using namespace std;

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

void DES(char bytes[])
{
    char B[BLK_SIZE], C[BLK_SIZE];
    int n = BitsSplit(bytes, BLK_BYTE, B);
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

void RandomKey(char key[])
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < KEY_BYTE; i++)
    {
        int x = rand() % 3;
        if (x == 0)
            key[i] = '0' + rand() % 10;
        else if (x == 1)
            key[i] = 'A' + rand() % 26;
        else
            key[i] = 'a' + rand() % 26;
    }
    key[KEY_BYTE] = '\0';
}

int main(int argc, char **argv)
{
    int flag = 0;
    bool isDecode = false;
    string input, output;
    char key[KEY_BYTE + 5];
    memset(key, 0, sizeof(key));

    for (char ch; (ch = getopt(argc, argv, "dk:i:o:")) != -1;)
    {
        switch (ch)
        {
        case 'd':
            isDecode = true;
            break;
        case 'k':
            strncpy(key, optarg, KEY_BYTE);
            flag |= 1;
            break;
        case 'i':
            input = optarg;
            flag |= 2;
            break;
        case 'o':
            output = optarg;
            break;
        }
    }

    if ((flag & 2) == 0 || (isDecode && (flag & 1) == 0))
        WriteUsage();

    if (output.size() == 0)
        output = input + ".des";

    if ((flag & 1) == 0)
        RandomKey(key);
    printf("Key: %s\n", key);
    KeysGeneration(key, isDecode);

    FILE *fin = fopen(input.c_str(), "r");
    FILE *fout = fopen(output.c_str(), "w");

    char block[BLK_BYTE];
    for (int cnt = 0; (cnt = fread(block, 1, BLK_BYTE, fin)) != 0;)
    {
        for (int i = cnt; i < BLK_BYTE; i++)
            block[i] = 0;
        DES(block);
        fwrite(block, 1, BLK_BYTE, fout); //decode????
    }

    fclose(fin);
    fclose(fout);
    return 0;
}

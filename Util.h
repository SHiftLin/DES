#include <iostream>
#include <string>
#include <sys/stat.h>

typedef long long LL;

struct FileInfo
{
    std::string name;
    LL size;
    FileInfo(){};
    FileInfo(std::string _name, LL _size)
    {
        name = _name;
        size = _size;
    }
};

void print(const char v[], int n)
{
    for (int i = 0; i < n; i++)
        printf("%d ", v[i]);
    printf("\n");
}

void Copy(char *dst, const char *src, int n)
{
    for (int i = 0; i < n; i++)
        dst[i] = src[i];
}

int BitsSplit(char bytes[], int n, char bits[], int base = 8)
{
    for (int i = 0; i < n; i++)
        for (int j = base - 1, x = bytes[i]; j >= 0; j--, x >>= 1)
            bits[i * base + j] = x & 1;
    return n * base;
}

int BitsMerge(char bits[], int n, char bytes[], int base = 8)
{
    int cnt = 0;
    for (int i = 0; i < n; i += base)
    {
        int x = 0;
        for (int j = 0; j < base; j++)
            x = (x << 1) | bits[i + j];
        bytes[cnt++] = x;
    }
    return cnt;
}

void Permutate(const char src[], const int n, char dst[], int m, int P[])
{
    for (int i = 0; i < m; i++)
        dst[i] = src[P[i]];
}

void XOR(char dst[], const char a[], const char b[], int n)
{
    for (int i = 0; i < n; i++)
        dst[i] = a[i] ^ b[i];
}

void cshl(char bits[], int n, int k)
{
    char Temp[n];
    for (int i = 0; i < n; i++)
        Temp[i] = bits[(i + k) % n];
    Copy(bits, Temp, n);
}

void Switch(char bits[], int n)
{
    int m = n >> 1;
    for (int i = 0; i < m; i++)
        std::swap(bits[i], bits[i + m]);
}

LL getFileSize(const char *path)
{
    struct stat status;
    if (stat(path, &status) == -1)
        return -1;
    else
        return status.st_size;
}

unsigned char *IntegerToBytes(LL x, unsigned char *bytes, int n)
{
    for (int i = n - 1; i >= 0; i--, x >>= 8)
        bytes[i] = x & 255;
    return bytes;
}

LL BytesToInteger(const char *bytes, int n)
{
    LL x = 0;
    for (int i = 0; i < n; i++)
        x = (x << 8) | (unsigned char)bytes[i];
    return x;
}

void WriteUsage()
{
    printf("Usage:\n");
    printf("./DES -i input [-odk]\n\n");
    printf("      -i input    Input path.\n");
    printf("      -o output   Output path.\n");
    printf("      -d          Decode mode.\n");
    printf("      -k key      Key.\n");
    printf("\n");
    exit(0);
}

void WriteError(const char mesg[])
{
    printf("Error: %s\n", mesg);
    exit(0);
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NUM_ROUNDS 16
#define BLOCK_SIZE 64
#define byte unsigned char

const int ip[64] = {58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
                    62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
                    57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
                    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};

const int fp[64] = {40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
                    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
                    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
                    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25};

const int pc1_l[28] = {57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18, 10, 2,
                       59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36};
const int pc1_r[28] = {63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22, 14, 6,
                       61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4};
const int pc2[48] = {14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10, 23, 19, 12, 4, 26, 8,
                     16, 7, 27, 20, 13, 2, 41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
                     44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};
const int shifts[] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1}; // 每轮左移的位数
const int S_box[8][64] = {
    {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
     4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0, 15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},
    {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10, 3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
     0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15, 13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9},
    {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8, 13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
     13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7, 1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12},
    {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15, 13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
     10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4, 3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14},
    {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9, 14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
     4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14, 11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3},
    {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11, 10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
     9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6, 4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13},
    {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1, 13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
     1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2, 6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12},
    {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7, 1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
     7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8, 2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}};
const int e[48] = {32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9, 10, 11, 12, 13, 12, 13,
                   14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 24, 25,
                   26, 27, 28, 29, 28, 29, 30, 31, 32, 1};
const int p[32] = {16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10, 2, 8, 24, 14,
                   32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25};
// 二进制转换函数
void int2bin(uint64_t a, int n, byte *res)
{
    for (int i = 0; i < n; i++)
    {
        res[n - i - 1] = a % 2;
        a = a / 2;
    }
}

uint64_t bin2int(byte *a, int n)
{
    uint64_t result = 0;
    for (int i = 0; i < n; i++)
    {
        result = result * 2 + (int)a[i];
    }
    return result;
}

// 循环左移
void leftRotate(byte *a, int n, int off)
{
    byte temp[n];
    for (int i = 0; i < n; i++)
    {
        temp[i] = a[(i + off) % n];
    }
    for (int i = 0; i < n; i++)
    {
        a[i] = temp[i];
    }
}

// 二进制数组异或
void binXor(byte *a, byte *b, byte *res, int n)
{
    for (int i = 0; i < n; i++)
    {
        res[i] = a[i] ^ b[i];
    }
}

// 初始置换
void IP(byte *a, byte *res)
{

    for (int i = 0; i < 64; i++)
    {
        res[i] = a[ip[i] - 1];
    }
}

// 最终置换
void FP(byte *a, byte *res)
{

    for (int i = 0; i < 64; i++)
    {
        res[i] = a[fp[i] - 1];
    }
}

// 选择置换1
void PC1(uint64_t key, byte *left, byte *right)
{

    byte temp_key[64];
    int2bin(key, 64, temp_key);
    for (int i = 0; i < 28; i++)
    {
        left[i] = temp_key[pc1_l[i] - 1];
        right[i] = temp_key[pc1_r[i] - 1];
    }
}

// 选择置换2
void PC2(byte *key, byte *res)
{

    for (int i = 0; i < 48; i++)
    {
        res[i] = key[pc2[i] - 1];
    }
}

// 子密钥生成
void keyGen(uint64_t key, byte subkeys[NUM_ROUNDS][48])
{
    byte left[28], right[28];
    PC1(key, left, right);

    byte combined[56];

    for (int round = 0; round < NUM_ROUNDS; round++)
    {
        leftRotate(left, 28, shifts[round]);
        leftRotate(right, 28, shifts[round]);

        // 合并左右部分
        for (int i = 0; i < 28; i++)
        {
            combined[i] = left[i];
            combined[i + 28] = right[i];
        }

        // 选择置换2生成子密钥
        PC2(combined, subkeys[round]);
    }
}

// S盒变换
void S(byte *a, byte *res)
{
    // S-boxes for DES (6-bit input -> 4-bit output)

    // 将6位输入数据进行查找并变换
    for (int i = 0; i < 8; i++)
    {
        int row = (a[i * 6] << 1) + a[i * 6 + 5];
        int col = (a[i * 6 + 1] << 3) + (a[i * 6 + 2] << 2) + (a[i * 6 + 3] << 1) + a[i * 6 + 4];
        int val = S_box[i][row * 16 + col];
        int2bin(val, 4, &res[i * 4]);
    }
}

// 扩展置换
void Expand(byte *a, byte *res)
{

    for (int i = 0; i < 48; i++)
    {
        res[i] = a[e[i] - 1];
    }
}

// P置换
void P(byte *a, byte *res)
{

    for (int i = 0; i < 32; i++)
    {
        res[i] = a[p[i] - 1];
    }
}

// Feistel函数
void Feistel(byte *a, byte *subkey, byte *res)
{
    byte expanded[48], s_out[48], p_out[32];
    Expand(a, expanded);
    binXor(expanded, subkey, s_out, 48);
    S(s_out, p_out);
    P(p_out, res);
}

// DES轮函数
void goRound(byte *l, byte *r, byte *subkey)
{
    byte temp[32];
    byte ftemp[32];
    Feistel(r, subkey, ftemp);

    binXor(l, ftemp, temp, 32);
    for (int i = 0; i < 32; i++)
    {
        l[i] = r[i];
        r[i] = temp[i];
    }
}

// DES加解密
/**
 * @param[in] method 1加密，2解密
 */
void DES(uint64_t plain, uint64_t key, int method, uint64_t *result)
{
    byte subkeys[NUM_ROUNDS][48];
    keyGen(key, subkeys);
    if (method == 2)
    {
        byte tempkey[NUM_ROUNDS][48];
        for (int i = 0; i < NUM_ROUNDS; i++)
        {
            memcpy(tempkey[i], subkeys[NUM_ROUNDS - i - 1], 48);
        }
        memcpy(subkeys, tempkey, 48 * NUM_ROUNDS);
    }
    byte plain_bin[64], res_bin[64];
    int2bin(plain, 64, plain_bin);

    // 初始置换
    byte ip_out[64];
    IP(plain_bin, ip_out);

    byte left[32], right[32];
    for (int i = 0; i < 32; i++)
    {
        left[i] = ip_out[i];
        right[i] = ip_out[i + 32];
    }

    // 加解密过程

    for (int i = 0; i < NUM_ROUNDS; i++)
    {
        goRound(left, right, subkeys[i]);
    }

    byte lr[64];
    for (int i = 0; i < 32; i++)
    {
        lr[i] = right[i];
        lr[i + 32] = left[i];
    }

    FP(lr, res_bin);
    *result = bin2int(res_bin, 64);
}

int main()
{
    while (1)
    {
        uint64_t result, key, input;
        int mode;
        while (1)
        {
            printf(
                "0: exit\n"
                "1: encrypt\n"
                "2: decrypt\n"
                "Choice: ");
            scanf("%d", &mode);
            if (mode == 0)
            {
                return 0;
            }
            else if (mode == 1 || mode == 2)
            {
                break;
            }
            else
            {
                printf("No such command!\n");
            }
        }

        printf("Input key(hex): ");
        scanf("%llx", &key);
        printf("Input text(hex): ");
        scanf("%llx", &input);
        DES(input, key, mode, &result);
        printf("Result: 0x%016llx\n", result);
    }
    return 0;
}

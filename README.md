# 分组加密的各种模式实现
采用PKCS#7策略，实现了AES/DES加密算法中ECB、CBC、OFB、CFB、CTR五种工作模式。
### AES.h/DES.h
相应加密算法的实现。
### main.c
主程序，用于测试AES算法的正确性。
### judge.py
调用相关库检验`main.c`中的结果。
### Makefile
用于编译和清理`main`程序
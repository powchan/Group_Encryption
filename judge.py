from Crypto.Cipher import AES
from Crypto.Util import Counter
import binascii

def aes_cbc_decrypt(key, iv, ciphertext):
    """
    使用AES-CBC模式解密密文。

    :param key: 16字节的密钥
    :param iv: 16字节的初始化向量
    :param ciphertext: 要解密的密文（字节）
    :return: 解密后的明文（字节）
    """
    cipher = AES.new(key, AES.MODE_CBC, iv)
    plaintext = cipher.decrypt(ciphertext)
    return plaintext

def aes_cfb_decrypt(key, iv, ciphertext, segment_size=8):
    """
    使用AES-CFB模式解密密文。

    :param key: 16字节的密钥
    :param iv: 16字节的初始化向量
    :param ciphertext: 要解密的密文（字节）
    :param segment_size: CFB模式的分段大小（位），默认为8位
    :return: 解密后的明文（字节）
    """
    cipher = AES.new(key, AES.MODE_CFB, iv, segment_size=segment_size)
    plaintext = cipher.decrypt(ciphertext)
    return plaintext

def aes_ctr_decrypt(key, iv, ciphertext):
    """
    使用AES-CTR模式解密密文。

    :param key: 16字节的密钥
    :param iv: 16字节的初始化向量（IV）
    :param ciphertext: 要解密的密文（字节）
    :return: 解密后的明文（字节）
    """
    # 分离Nonce和计数器
    nonce = iv[:14]            # 前14字节作为Nonce
    counter_byte = iv[14:15]   # 第15字节作为计数器
    reserved = iv[15:16]       # 第16字节保留或固定

    # 将计数器字节转换为整数，初始值为0
    initial_counter = int.from_bytes(counter_byte, byteorder='big')

    # 创建CTR计数器
    ctr = Counter.new(
        8,                    # 计数器长度为8位（1字节）
        prefix=nonce,         # 前缀为Nonce
        initial_value=initial_counter,  # 初始计数器值
        suffix=reserved,      # 后缀为保留字节
        little_endian=False   # 大端字节序
    )

    # 创建AES-CTR解密器
    cipher = AES.new(key, AES.MODE_CTR, counter=ctr)

    # 执行解密
    plaintext = cipher.decrypt(ciphertext)
    return plaintext

def main():
    # 定义密钥和IV（初始化向量）
    key = bytes([
        0x31, 0x32, 0x33, 0x34,  # '1','2','3','4'
        0x35, 0x36, 0x37, 0x38,  # '5','6','7','8'
        0x31, 0x32, 0x33, 0x34,  # '1','2','3','4'
        0x35, 0x36, 0x37, 0x38   # '5','6','7','8'
    ])  # 对应ASCII "1234567812345678"

    iv = bytes([
        0x31, 0x32, 0x33, 0x34,  # '1','2','3','4'
        0x35, 0x36, 0x37, 0x38,  # '5','6','7','8'
        0x31, 0x32, 0x33, 0x34,  # '1','2','3','4'
        0x35, 0x36, 0x37, 0x38   # '5','6','7','8'
    ])  # 对应ASCII "1234567812345678"

    # CBC模式密文（16字节 * 2）
    cbc_ciphertext_hex = "3BD21D7D8B1656C77A77B1E7ECB33C723850E95F90841748756F029CD18EF198"
    cbc_ciphertext = bytes.fromhex(cbc_ciphertext_hex)

    # CFB模式密文（16字节 * 2）
    cfb_ciphertext_hex = "A35FB116F359F02BC82A696B24BC75487B3F6141E7BB3CA78EB30ED2875CF576"
    cfb_ciphertext = bytes.fromhex(cfb_ciphertext_hex)

    # CTR模式密文（16字节 * 2）
    ctr_ciphertext_hex = "655D1C55F2F8827FF2EC779EF0006C41C9DDC9330E26E5A3FA5B0EBFD6C240E9"
    ctr_ciphertext = bytes.fromhex(ctr_ciphertext_hex)

    # 解密CBC模式密文
    plaintext_cbc = aes_cbc_decrypt(key, iv, cbc_ciphertext)
    print("解密CBC模式密文结果:")
    print(plaintext_cbc.hex().upper())
    print()

    # 解密CFB模式密文（s=8bit）
    plaintext_cfb = aes_cfb_decrypt(key, iv, cfb_ciphertext, segment_size=8)
    print("解密CFB模式密文结果:")
    print(plaintext_cfb.hex().upper())
    print()

    # 解密CTR模式密文
    plaintext_ctr = aes_ctr_decrypt(key, iv, ctr_ciphertext)
    print("解密CTR模式密文结果:")
    print(plaintext_ctr.hex().upper())
    print()

if __name__ == "__main__":
    main()

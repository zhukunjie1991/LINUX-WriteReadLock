#ifndef _BASE64_ENCODE_H
#define _BASE64_ENCODE_H 1

/* Base64 operations */
 /*编码长度    
 @[in]
 len:输入的数据长度
 @[out]
 return:编码后的长度,比实际的编码长度多1位
 */
int Base64EncodeLen(int len);
/*解码长度    
 @[in]
 bufcoded:经过编码后的字符串
 @[out]
 return:解码后的长度
 */
int Base64DecodeLen(const char *bufcoded);
/*编码
 @[in]
 string:需要编码的数据
 len:需要编码的数据的长度
 @[out]
 encoded:编码后的字符串
 return:编码后的长度，末尾以'\0'结尾，等于Base64EncodeLen返回的大小
 */
int Base64Encode(char *encoded, const char *string, int len);
/*解码
 @[in]
 bufcoded:需要解码的数据
 @[out]
 bufplain:解码后的数据
 return:解码后的长度,等于Base64DecodeLen返回的大小
 */
int Base64Decode(char *bufplain, const char *bufcoded);

/* 这两个方法是private，外面不能调用，下个版本将会在头文件去掉 */
int Base64EncodeBinary(char *encoded, const unsigned char *string, int len);
int Base64DecodeBinary(unsigned char *bufplain, const char *bufcoded);

#endif /* _BASE64_ENCODE_H */

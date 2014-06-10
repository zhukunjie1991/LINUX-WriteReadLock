#ifndef _BASE64_ENCODE_H
#define _BASE64_ENCODE_H 1

/* Base64 operations */
 /*���볤��    
 @[in]
 len:��������ݳ���
 @[out]
 return:�����ĳ���,��ʵ�ʵı��볤�ȶ�1λ
 */
int Base64EncodeLen(int len);
/*���볤��    
 @[in]
 bufcoded:�����������ַ���
 @[out]
 return:�����ĳ���
 */
int Base64DecodeLen(const char *bufcoded);
/*����
 @[in]
 string:��Ҫ���������
 len:��Ҫ��������ݵĳ���
 @[out]
 encoded:�������ַ���
 return:�����ĳ��ȣ�ĩβ��'\0'��β������Base64EncodeLen���صĴ�С
 */
int Base64Encode(char *encoded, const char *string, int len);
/*����
 @[in]
 bufcoded:��Ҫ���������
 @[out]
 bufplain:����������
 return:�����ĳ���,����Base64DecodeLen���صĴ�С
 */
int Base64Decode(char *bufplain, const char *bufcoded);

/* ������������private�����治�ܵ��ã��¸��汾������ͷ�ļ�ȥ�� */
int Base64EncodeBinary(char *encoded, const unsigned char *string, int len);
int Base64DecodeBinary(unsigned char *bufplain, const char *bufcoded);

#endif /* _BASE64_ENCODE_H */

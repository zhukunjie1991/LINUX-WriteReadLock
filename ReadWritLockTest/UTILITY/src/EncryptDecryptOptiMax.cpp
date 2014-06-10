#include "EncryptDecryptFunc.h"

static void rc4_crypt( struct rc4_state *s, unsigned char *data, int length );
static void rc4_setup( struct rc4_state *s, unsigned char *rkey,  int length );
static void GenerateEncryptionKey(const char *caInputKey, char *caOutputKey);
static int Encrypt(char * BufferIn,int    BufferInLength,char * BufferOut,int  * BufferOutLength,char * randKey);
static int Decrypt(char  * BufferIn,char* BufferOut,int ufferInLength,int * BufferOutLength,char* randKey );

bool EncryptDecryptFile(const char* caInputFilePath,
                        const char* caOutputFilePath,
                        const char* caInputKey,
                        int         iEncryptDecryptIndc)
{
  int   iReturnValue = 0                    ;
  char  *caCompDecomBuffer=NULL                 ;
  char  *caCryptedBuffer=NULL             ;
  char  *caCompBuffer=NULL                  ;
  char  *caReadBuffer=NULL                  ;
  char  *caDecompBuffer=NULL                ;
  char  *caEncryptedBuffer=NULL             ;
  char  *caDecryptedBuffer=NULL             ;
  int   iEncryptedBufferLngth = 0           ;
  int   iDecryptedBufferLngth = 0           ;
  int   iNmbrOfBytesNmbr = 0                ;
  FILE  *FilePointerR                       ;
  FILE  *FilePointerW                       ;
  char  randKey[KEYSIZE]  = ENCRYPTION_KEY  ;
  bool  bRet = true;

  if ((iEncryptDecryptIndc != INDC_COMPRESS_AND_ENCRYPT) &&
      (iEncryptDecryptIndc != INDC_ONLY_ENCRYPT) &&
      (iEncryptDecryptIndc != INDC_DECRYPT_AND_DECOMPRESS) &&
      (iEncryptDecryptIndc != INDC_ONLY_DECRYPT) &&
      (iEncryptDecryptIndc != INDC_ONLY_COMPRESS) &&
      (iEncryptDecryptIndc != INDC_ONLY_DECOMPRESS))
  {
    printf("\n Enter Valid Encrypt/Decrypt Option :");
    printf("\n 1: Compress & Encrypt");
    printf("\n 2: Only Encrypt");
    printf("\n 3: Decrypt & Decompress");
    printf("\n 4: Only Decrypt");
    printf("\n Usage is as follows : ");
    printf("\n { Executable Name <Input File> <Output File> <Encrypt/Decrypt Option> <Optional - Encryption Key >}");
    return(false);
  }

/*  if (strcmp(caInputKey,NULL) != 0)*/
  if(caInputKey != NULL)
  {
    memset(randKey,NULL,KEYSIZE);
    GenerateEncryptionKey(caInputKey, randKey);
  }

  /** Start Rahul 020609 **/
  caReadBuffer = (char*)malloc(MAX_MSG_LEN +1);
  if (caReadBuffer == NULL)
  {
    printf("\n Error in Allocating Memory in Encrypt Function ");
    return(false);
  }

  caCryptedBuffer = (char*)malloc(MAX_MSG_LEN +1);
  if (caCryptedBuffer == NULL)
  {
    printf("\n Error in Allocating Memory in Encrypt Function ");
	bRet = false;
	goto EncryptDecypt_End;
  }

  caCompDecomBuffer = (char*)malloc(MAX_MSG_LEN +1);
  if (caCompDecomBuffer == NULL)
  {
    printf("\n Error in Allocating Memory in Encrypt Function ");
	bRet = false;
	goto EncryptDecypt_End;
  }

  caEncryptedBuffer = caCryptedBuffer;
  caDecryptedBuffer = caCryptedBuffer;
  caDecompBuffer = caCompDecomBuffer ;
  caCompBuffer = caCompDecomBuffer ;

/** End Rahul 020609 **/
  FilePointerR = fopen(caInputFilePath,"r");
  if(FilePointerR == NULL)
  {
    printf("\nCannot Open File for Read\n");
	bRet = false;
	goto EncryptDecypt_End;
  }

  FilePointerW = fopen(caOutputFilePath,"w+");
  if(FilePointerW == NULL)
  {
    printf("\nCannot Open File for Write\n");
	bRet = false;
	goto EncryptDecypt_End;
  }

  for (; ;)
  {
    memset(caReadBuffer,NULL,MAX_MSG_LEN+1);
    memset(caCryptedBuffer,NULL,MAX_MSG_LEN+1);
    iEncryptedBufferLngth = 0;
    iNmbrOfBytesNmbr = 0;
    iNmbrOfBytesNmbr = fread(caReadBuffer,1,MAX_MSG_LEN,FilePointerR);
    if(iNmbrOfBytesNmbr <= 0)
    {
      if (iNmbrOfBytesNmbr < 0)
      {
        printf("\nError In Reading File\n");
      }
	  bRet = false;
	  goto EncryptDecypt_End;
    }

    if (iEncryptDecryptIndc == INDC_COMPRESS_AND_ENCRYPT)
    {
		printf("Error indication value1 = %d\n", iEncryptDecryptIndc);
		bRet = false;
		goto EncryptDecypt_End;
    }

    if (iEncryptDecryptIndc == INDC_ONLY_ENCRYPT)
    {
      iReturnValue = Encrypt(caReadBuffer,
                             iNmbrOfBytesNmbr,
                             caEncryptedBuffer,
                             &iEncryptedBufferLngth,
                             randKey);
      if(iReturnValue != true )
      {
        printf("\nError While Encrypting a File\n");
		bRet = false;
		goto EncryptDecypt_End;
      }
      fwrite(caEncryptedBuffer, iEncryptedBufferLngth,1,FilePointerW);
    }

    if (iEncryptDecryptIndc == INDC_DECRYPT_AND_DECOMPRESS)
    {
		printf("Error indication value = %d\n", iEncryptDecryptIndc);
		bRet = false;
		goto EncryptDecypt_End;
    }

    if (iEncryptDecryptIndc == INDC_ONLY_DECRYPT)
    {
     
      iReturnValue = Decrypt(caReadBuffer,
                             caDecryptedBuffer,
                             iNmbrOfBytesNmbr,
                             &iDecryptedBufferLngth,
                             randKey);
      if(iReturnValue != true )
      {
        printf("\nError While Decrypting a File\n");
		bRet = false;
		goto EncryptDecypt_End;
      }

      fwrite(caDecryptedBuffer, iDecryptedBufferLngth,1,FilePointerW);
    }

    if (iEncryptDecryptIndc == INDC_ONLY_COMPRESS)
    {
		printf("Error indication value2 = %d\n", iEncryptDecryptIndc);
		bRet = false;
		goto EncryptDecypt_End;
    }
    if (iEncryptDecryptIndc == INDC_ONLY_DECOMPRESS)
    {
		printf("Error indication value3 = %d\n", iEncryptDecryptIndc);
		bRet = false;
		goto EncryptDecypt_End;
    }
    if((feof(FilePointerR)) || (iNmbrOfBytesNmbr < MAX_MSG_LEN))
    {
      fclose(FilePointerR);
	  FilePointerR = NULL;
      break;
    }
  } /** End Of Infinite For Loop **/
  if (FilePointerW != NULL)
  {
    fclose(FilePointerW);
	FilePointerW = NULL;
  }

EncryptDecypt_End: 

  if(caCryptedBuffer) free(caCryptedBuffer);
  if(caCompDecomBuffer) free(caCompDecomBuffer);
  if(caReadBuffer) free(caReadBuffer);
  if(FilePointerR) fclose(FilePointerR);
  if(FilePointerW) fclose(FilePointerW);

  return bRet;
}

static void rc4_setup( struct rc4_state *s, unsigned char *rkey,  int length )
{
  int i, j, k, *m, a;

  s->x = 0;
  s->y = 0;
  m = s->m;

  for( i = 0; i < 256; i++ )
  {
      m[i] = i;
  }

  j = k = 0;

  for( i = 0; i < 256; i++ )
  {
      a = m[i];
      j = (unsigned char) ( j + a + rkey[k] );

      m[i] = m[j]; m[j] = a;
      if( ++k >= length ) k = 0;
  }
}

static void rc4_crypt( struct rc4_state *s, unsigned char *data, int length )
{
  int i, x, y, *m, a, b;

  x = s->x;
  y = s->y;
  m = s->m;

  for( i = 0; i < length; i++ )
  {
    x = (unsigned char) ( x + 1 ); a = m[x];
    y = (unsigned char) ( y + a );
    m[x] = b = m[y];
    m[y] = a;
    data[i] ^= m[(unsigned char) ( a + b )];
  }

  s->x = x;
  s->y = y;
}

static void GenerateEncryptionKey(const char *caInputKey, char *caOutputKey)
{
  int   iCount;
  int   iSizeOfKey;
  char  caEncdKey[KEYSIZE];

  memset(caEncdKey,0,KEYSIZE);
  strcpy(caEncdKey,ENCODING_KEY);
  iSizeOfKey = strlen(caInputKey);

  for (iCount = 0; iCount < iSizeOfKey; iCount++)
  {
    caOutputKey[iCount]= (char)(((int)caInputKey[iCount] *
                                (int)caEncdKey[strlen(caEncdKey)-(iCount+1)%strlen(caEncdKey)]
                                )% MAX_ASCII_CHAR_VALUE);
    if ((int)caOutputKey[iCount] == 0)
    {
      caOutputKey[iCount] = 'S';
    }
  }
  printf("\nNew Key %s\n",caOutputKey);
}

static int Encrypt(char * BufferIn,
            int    BufferInLength,
            char * BufferOut,
            int  * BufferOutLength,
            char * randKey)
{
  struct rc4_state  key;
	char		          caEncyBuffer[MAX_CRYPT_LEN];
	int		            lTempBufferLngth  = 0;
	char		          *caTempBuffer = NULL;

/** Start Rahul 020609 **/
  caTempBuffer = (char*)malloc(MAX_MSG_LEN +1);
	if (caTempBuffer == NULL)
	{
		printf("\n Error in Allocating Memory in Encrypt Function ");
		return(false);
	}
/** End Rahul 020609 **/
	memset(caTempBuffer,NULL,MAX_MSG_LEN + 1);
	memset(caEncyBuffer,NULL,MAX_CRYPT_LEN);
	rc4_setup(&key, (unsigned char *)randKey, 5);

  for (; ;)
	{
		if (BufferInLength <= MAX_CRYPT_LEN)
		{
			memcpy(caEncyBuffer,BufferIn + lTempBufferLngth,BufferInLength);
			rc4_crypt(&key,(unsigned char *)caEncyBuffer,BufferInLength);
			memcpy(caTempBuffer + lTempBufferLngth,caEncyBuffer, BufferInLength);
			lTempBufferLngth += BufferInLength;
			break;
		}
		else
		{
			memcpy(caEncyBuffer,BufferIn + lTempBufferLngth,MAX_CRYPT_LEN);
			rc4_crypt(&key,(unsigned char *)caEncyBuffer,MAX_CRYPT_LEN);
		}
		memcpy(caTempBuffer + lTempBufferLngth,caEncyBuffer,MAX_CRYPT_LEN);
		lTempBufferLngth += MAX_CRYPT_LEN;
		BufferInLength -= MAX_CRYPT_LEN;
  }
	memcpy(BufferOut,caTempBuffer,lTempBufferLngth);
	*BufferOutLength = lTempBufferLngth;

  return true;
}

static int Decrypt(char  * BufferIn,
            char  * BufferOut,
            int     BufferInLength,
            int   * BufferOutLength,
            char  * randKey )
{
  struct rc4_state  key;
	int			          lTempBufferLngth  = 0;
	char			        buf[MAX_CRYPT_LEN];

	memset(buf,NULL,MAX_CRYPT_LEN);
	rc4_setup(&key, (unsigned char *)randKey, 5);

	for (; ;)
	{
		if (BufferInLength <= MAX_CRYPT_LEN)
		{
			memcpy(buf,BufferIn + lTempBufferLngth,BufferInLength);
			rc4_crypt(&key,(unsigned char *)buf,BufferInLength);
			memcpy(BufferOut + lTempBufferLngth,buf,BufferInLength);
			lTempBufferLngth += BufferInLength;
			break;
		}
		else
		{
			memcpy(buf,BufferIn + lTempBufferLngth,MAX_CRYPT_LEN);
			rc4_crypt(&key,(unsigned char *)buf,MAX_CRYPT_LEN);
		}
		memcpy(BufferOut + lTempBufferLngth,buf,MAX_CRYPT_LEN);
		lTempBufferLngth += MAX_CRYPT_LEN;
		BufferInLength -= MAX_CRYPT_LEN;
	}
	*BufferOutLength = lTempBufferLngth;

  return true;
}

/****************************************************************************/
/*                H E A D E R  I N F O R M A T I O N  B O X                 */
/****************************************************************************/
/*                                                                          */
/*                      TCS CONFIDENTIAL INFORMATION                        */
/*                                                                          */
/*                COPYRIGHT (c) 2002 TCS All Rights Reserved                */
/*                                                                          */
/*    This module contains confidential and proprietary information of      */
/*    TCS, and any reproduction, disclosure or use in whole or in part      */
/*    is expressly forbidden except as may be specifically authorized       */
/*    by prior written agreement or permission of TCS.                      */
/*                                                                          */
/****************************************************************************/
/****************************************************************************/
/*   Author       :  Joe                                           */
/*   Date         :  2011-7-12                                            */
/*   Synopsis     :                                                         */
/*                                                                          */
/*   Modifications:                                                         */
/****************************************************************************/
/*                                                                          */
/* Date     Author Mod. CR No. Description                                  */
/* -------- ------ ---- ------- --------------------------------------------*/
/*																																					*/
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _ENCRYPTDECRYPT_FUNC_H_
#define _ENCRYPTDECRYPT_FUNC_H_

#define   KEYSIZE                 				30       
#define   ENCODING_KEY            		    "ABXZYN" 
#define   MIN_INPUT_ARGUMENT_NUMBER       4        
#define   MAX_INPUT_ARGUMENT_NUMBER       5        
#define   MAX_ASCII_CHAR_VALUE    		    256      
#define 	BINARY_FLAG									    1        
#define   ENCRYPTION_KEY          		    "AAAAA"  
#define   INDC_COMPRESS_AND_ENCRYPT       1        
#define   INDC_ONLY_ENCRYPT               2        
#define   INDC_DECRYPT_AND_DECOMPRESS     3        
#define   INDC_ONLY_DECRYPT               4        
#define		INDC_ONLY_COMPRESS					    5        
#define		INDC_ONLY_DECOMPRESS				    6        
#define		MAX_MSG_LEN											30*1024
#define MAX_CRYPT_LEN                      4096

#ifndef _POSIX_PATH_MAX
#define _POSIX_PATH_MAX 255
#endif

struct rc4_state
{
  int x, y, m[256];
};



/****************************************************************************/
/*  Encrypt or Decrypt file                                              		*/
/****************************************************************************/	
bool EncryptDecryptFile(const char* caInputFilePath,
	                    const char* caOutputFilePath,
	                    const char* caInputKey,
	                    int         iEncryptDecryptIndc);
                       

													
													

#endif	

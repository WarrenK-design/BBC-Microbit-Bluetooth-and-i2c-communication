/* small_aes.h
 *
 * Copyright (c) 2012  Rafael Azuero Hurtado - German Londoño Paez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */

/** 
* @file small_aes.h
* @brief Small AES with T-box
*/

#ifndef _SMALL_AES_H_
#define _SMALL_AES_H_

#include "string.h"

#ifdef __cplusplus
    extern "C" {
#endif

enum {
    SMALL_AES_ENCRYPTION = 0,
    SMALL_AES_DECRYPTION = 1,
    SMALL_AES_BLOCK_SIZE = 16
};

typedef struct AES {    
    unsigned int key[60];
    unsigned int  rounds;
    
    unsigned int reg[SMALL_AES_BLOCK_SIZE / sizeof(unsigned int)];    
} AES;

/**
* @fn int aesSetKey(AES* aes, const unsigned char *key, unsigned int len, int dir)
* @brief Sets AES key
* @param aes The AES struct
* @param key The key bytes
* @param len The Key length
* @param dir If encrypt or decrypt
* @return -1 if the key size is not valid; 0 otherwise.
*/
int aesSetKey(AES* aes, const unsigned char *key, unsigned int len, int dir);

/**
* @fn void aesEncrypt(AES* aes, const unsigned char *inBlock, unsigned char *outBlock)
* @brief Encrypts a 16-byte block
* @param aes The AES struct
* @param inBlock The 16-byte input block
* @param outBlock The 16-byte output block
*/
void aesEncrypt(AES* aes, const unsigned char *inBlock, unsigned char *outBlock);

/**
* @fn void aesDecrypt(AES* aes, const unsigned char *inBlock, unsigned char *outBlock)
* @brief Decrypts a 16-byte block
* @param aes The AES struct
* @param inBlock The 16-byte input block
* @param outBlock The 16-byte output block
*/
void aesDecrypt(AES* aes, const unsigned char *inBlock, unsigned char *outBlock);

/**
* @fn void aesEcbEncrypt(AES* aes, unsigned char *output, const unsigned char *input, unsigned int inputSize)
* @brief Encrypts a n-byte block with AES-ECB
* @param aes The AES struct
* @param output The input data
* @param input The output data
* @param inputSize The input data size in bytes
*/
void aesEcbEncrypt(AES* aes, unsigned char *output, const unsigned char *input, unsigned int inputSize);

/**
* @fn void aesEcbDecrypt(AES* aes, unsigned char *output, const unsigned char *input, unsigned int inputSize)
* @brief Decrypts a n-bytes block with AES-ECB
* @param aes The AES struct
* @param output The input data
* @param input The output data
* @param inputSize The input data size in bytes
*/
void aesEcbDecrypt(AES* aes, unsigned char *output, const unsigned char *input, unsigned int inputSize);

/**
* @fn unsigned int byteReverseWord32(unsigned int value)
* @brief Converts a  big-endian 32-bit word to little-endian format
* @param value The big-endian 32-bit word
* @return The little-endian 32-bit word
*/
unsigned int byteReverseWord32(unsigned int value);

/**
* @fn void byteReverseWords(unsigned int *out, const unsigned int *in, unsigned int byteCount)
* @brief Converts a big-endian block to little-endian format
* @param out The output block
* @param in The input block
* @param byteCount The Number of bytes
*/
void byteReverseWords(unsigned int *out, const unsigned int *in, unsigned int byteCount);

/**
* @fn unsigned int rotlFixed(unsigned int x, unsigned int y)
* @brief Rotates a 32-bit word n-bytes to the left
* @param x The32-bit word
* @param y The number of bytes to rotate
* @return The rotated 32-bit word
*/
unsigned int rotlFixed(unsigned int x, unsigned int y);

/**
* @fn unsigned int rotrFixed(unsigned int x, unsigned int y)
* @brief Rotates a 32-bit word n-bytes to the right
* @param x The 32-bit word
* @param y The number of bytes to rotate
* @return The rotated 32-bit word
*/
unsigned int rotrFixed(unsigned int x, unsigned int y);


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif
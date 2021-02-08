/*
 * Copyright (c) 2011-2014 Apple Inc. All Rights Reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */


#include <Security/Security.h>
#include <Security/SecBase.h>

#include "../sslViewer/sslAppUtils.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */
#include <fcntl.h>
#include <time.h>

#ifdef NO_SERVER
#include "keychain/securityd/spi.h"
#endif

static
unsigned char ServerRSA_Key_der[] = {
    0x30, 0x82, 0x02, 0x5b, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 0x00, 0xab,
    0x05, 0xba, 0xdc, 0x2d, 0xb3, 0x03, 0xf1, 0x6c, 0x60, 0x7f, 0x5e, 0x80,
    0x85, 0x54, 0x24, 0xc7, 0x87, 0x6e, 0xe5, 0xf1, 0xae, 0x76, 0x59, 0xe3,
    0x4b, 0x9c, 0xff, 0xa2, 0x41, 0xfc, 0x40, 0xf8, 0xa8, 0x33, 0x12, 0xa9,
    0x1b, 0x1a, 0xc5, 0xe8, 0xef, 0xb1, 0xe3, 0x75, 0x0b, 0xd2, 0x28, 0x49,
    0x48, 0x64, 0x9c, 0x3d, 0x89, 0xb6, 0xf8, 0xa5, 0x93, 0xd0, 0x29, 0x8c,
    0x9e, 0x7a, 0xf6, 0x00, 0x20, 0x08, 0x52, 0x08, 0xdc, 0x1d, 0x17, 0x8b,
    0x44, 0x4e, 0x32, 0x13, 0xdc, 0xb1, 0x50, 0xa5, 0xf0, 0x94, 0x25, 0x50,
    0xfa, 0x1e, 0xe3, 0xae, 0x66, 0x19, 0x13, 0x3e, 0x20, 0x86, 0x05, 0x9c,
    0xda, 0xd9, 0xff, 0x8f, 0x72, 0x6e, 0xf5, 0xc1, 0xfd, 0x86, 0xae, 0x26,
    0xcc, 0x4b, 0xc8, 0x9f, 0xa9, 0xd6, 0x3a, 0x1f, 0xb6, 0x8a, 0x8f, 0x04,
    0x2d, 0xbb, 0xa4, 0x47, 0xb3, 0xfb, 0xf9, 0x02, 0x03, 0x01, 0x00, 0x01,
    0x02, 0x81, 0x80, 0x38, 0x04, 0xf1, 0x77, 0x4b, 0xb4, 0xd6, 0xb6, 0xce,
    0xf4, 0x30, 0xe4, 0x68, 0x9e, 0xc3, 0xb8, 0x24, 0x6f, 0x75, 0x60, 0xf6,
    0xb0, 0x59, 0xee, 0x09, 0xa8, 0xeb, 0xed, 0x44, 0x5d, 0xee, 0xdd, 0xed,
    0x55, 0x53, 0x1d, 0x6a, 0xad, 0x09, 0x31, 0x08, 0xa2, 0xf3, 0x16, 0xf9,
    0x70, 0xfc, 0xce, 0xdb, 0x6a, 0x4e, 0x22, 0x6b, 0x79, 0xdf, 0xa8, 0x44,
    0xbc, 0x4d, 0x34, 0x3e, 0xee, 0x6e, 0x81, 0xfa, 0xe5, 0xf4, 0x62, 0x95,
    0x30, 0xce, 0x49, 0x11, 0x42, 0x2b, 0x2e, 0x6a, 0x87, 0x0c, 0x6a, 0x1f,
    0xaf, 0x22, 0xec, 0x32, 0x6b, 0x3e, 0x1b, 0xc3, 0xcb, 0xb4, 0x46, 0xd6,
    0x14, 0xd0, 0x52, 0x6b, 0x4c, 0x63, 0x74, 0xcb, 0xbe, 0xeb, 0xf8, 0xbf,
    0x31, 0xd6, 0xe3, 0x42, 0x1f, 0x77, 0x68, 0xf2, 0xf2, 0xf0, 0xf4, 0x24,
    0x10, 0x5f, 0x9c, 0x3c, 0x5c, 0xbb, 0x5b, 0x19, 0xed, 0x30, 0x01, 0x02,
    0x41, 0x00, 0xd2, 0x72, 0x8b, 0xd9, 0x1a, 0x8d, 0xcb, 0xa2, 0x56, 0x6b,
    0x3b, 0x78, 0xf3, 0x7a, 0xd4, 0x97, 0x90, 0xcd, 0xcd, 0x5a, 0x32, 0x06,
    0x3d, 0xb0, 0xc2, 0xfb, 0x9f, 0x95, 0x51, 0x63, 0xcf, 0xcd, 0x5c, 0xcb,
    0x4b, 0xa7, 0xe5, 0x5f, 0xd0, 0xd3, 0x5a, 0xc8, 0x92, 0xe1, 0xde, 0xe0,
    0x83, 0x86, 0xfe, 0xdd, 0xe1, 0xb4, 0x00, 0x72, 0x25, 0xb4, 0x20, 0x19,
    0xf6, 0x94, 0xf8, 0xfd, 0x4e, 0x01, 0x02, 0x41, 0x00, 0xd0, 0x0a, 0x89,
    0x2a, 0x99, 0x49, 0x35, 0x60, 0x14, 0x8d, 0x2c, 0xe7, 0x72, 0xa0, 0x19,
    0xd6, 0x86, 0x60, 0x0d, 0xa6, 0x44, 0x89, 0x30, 0x98, 0xea, 0xeb, 0xdf,
    0xfb, 0xb5, 0x56, 0x23, 0x3c, 0xe4, 0xc9, 0x76, 0x4f, 0x90, 0x8e, 0x55,
    0x7d, 0x51, 0xcb, 0x41, 0xf1, 0x73, 0xb0, 0xa9, 0x8b, 0x36, 0xf9, 0x1a,
    0xfe, 0x6f, 0xa3, 0x2e, 0x13, 0x30, 0xc4, 0xe3, 0x2c, 0x51, 0x7d, 0x1d,
    0xf9, 0x02, 0x40, 0x6e, 0x72, 0x55, 0x79, 0x04, 0x99, 0xa4, 0x64, 0xb7,
    0x8c, 0x21, 0xb3, 0x51, 0xbd, 0x86, 0x33, 0x61, 0x78, 0xd1, 0x2c, 0x64,
    0x12, 0xa8, 0x6f, 0xcb, 0x75, 0x39, 0x84, 0xa9, 0x29, 0x84, 0x16, 0xd8,
    0x7b, 0x8d, 0x62, 0x39, 0x5c, 0x77, 0x01, 0x65, 0xa4, 0xdc, 0x89, 0x94,
    0x6a, 0x2a, 0x3d, 0x40, 0x27, 0x7c, 0xdb, 0xf6, 0x5b, 0xf1, 0xf3, 0xbd,
    0xe1, 0x42, 0x6b, 0x5e, 0xdd, 0xba, 0x01, 0x02, 0x40, 0x6b, 0x56, 0x14,
    0x41, 0x23, 0x47, 0x2f, 0x3f, 0xbc, 0x3a, 0xbe, 0x81, 0x47, 0x95, 0xac,
    0xdf, 0x0f, 0x03, 0x7d, 0xe7, 0x5c, 0x13, 0x00, 0x3c, 0xd5, 0x70, 0x0f,
    0x67, 0x19, 0xbf, 0x30, 0x7d, 0x19, 0x79, 0x8c, 0x0e, 0x2f, 0x02, 0x10,
    0xbf, 0x90, 0xb4, 0xf2, 0xf7, 0xf5, 0x7d, 0x9f, 0x6c, 0x11, 0x57, 0xe3,
    0x02, 0x85, 0x6c, 0xc6, 0xb7, 0xe6, 0x00, 0x9f, 0x48, 0xdd, 0x5e, 0x55,
    0x59, 0x02, 0x40, 0x51, 0xb5, 0xaf, 0x36, 0x4b, 0x91, 0x6c, 0x89, 0x19,
    0x44, 0x96, 0x6c, 0x4a, 0x94, 0x58, 0x29, 0x5c, 0x38, 0xd4, 0x6b, 0x24,
    0xdc, 0x3b, 0xb1, 0x66, 0xc1, 0x3c, 0xc8, 0x17, 0x97, 0xb7, 0x05, 0xd9,
    0x18, 0xb6, 0x43, 0x16, 0xf8, 0xf9, 0x5e, 0xed, 0x7e, 0x6f, 0xc4, 0xa6,
    0x6a, 0x3a, 0xc7, 0xc0, 0x21, 0x6b, 0x39, 0xac, 0xf3, 0xf6, 0x40, 0xd5,
    0x25, 0xf7, 0x44, 0x1d, 0xd4, 0xb2, 0x37
};

static
unsigned char ServerRSA_Cert_CA_RSA_der[] = {
    0x30, 0x82, 0x02, 0x69, 0x30, 0x82, 0x01, 0x51, 0xa0, 0x03, 0x02, 0x01,
    0x02, 0x02, 0x01, 0x01, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86,
    0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00, 0x30, 0x25, 0x31, 0x23, 0x30,
    0x21, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x1a, 0x53, 0x65, 0x63, 0x75,
    0x72, 0x69, 0x74, 0x79, 0x54, 0x65, 0x73, 0x74, 0x20, 0x43, 0x41, 0x20,
    0x43, 0x65, 0x72, 0x74, 0x20, 0x28, 0x52, 0x53, 0x41, 0x29, 0x30, 0x20,
    0x17, 0x0d, 0x31, 0x35, 0x30, 0x33, 0x32, 0x33, 0x30, 0x37, 0x31, 0x30,
    0x32, 0x36, 0x5a, 0x18, 0x0f, 0x32, 0x30, 0x35, 0x35, 0x30, 0x33, 0x31,
    0x33, 0x30, 0x37, 0x31, 0x30, 0x32, 0x36, 0x5a, 0x30, 0x3e, 0x31, 0x28,
    0x30, 0x26, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x13, 0x1f, 0x53, 0x65, 0x63,
    0x75, 0x72, 0x69, 0x74, 0x79, 0x54, 0x65, 0x73, 0x74, 0x73, 0x20, 0x53,
    0x65, 0x72, 0x76, 0x65, 0x72, 0x20, 0x43, 0x65, 0x72, 0x74, 0x20, 0x28,
    0x52, 0x53, 0x41, 0x29, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04,
    0x03, 0x13, 0x09, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x68, 0x6f, 0x73, 0x74,
    0x30, 0x81, 0x9f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
    0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81, 0x8d, 0x00, 0x30, 0x81,
    0x89, 0x02, 0x81, 0x81, 0x00, 0xab, 0x05, 0xba, 0xdc, 0x2d, 0xb3, 0x03,
    0xf1, 0x6c, 0x60, 0x7f, 0x5e, 0x80, 0x85, 0x54, 0x24, 0xc7, 0x87, 0x6e,
    0xe5, 0xf1, 0xae, 0x76, 0x59, 0xe3, 0x4b, 0x9c, 0xff, 0xa2, 0x41, 0xfc,
    0x40, 0xf8, 0xa8, 0x33, 0x12, 0xa9, 0x1b, 0x1a, 0xc5, 0xe8, 0xef, 0xb1,
    0xe3, 0x75, 0x0b, 0xd2, 0x28, 0x49, 0x48, 0x64, 0x9c, 0x3d, 0x89, 0xb6,
    0xf8, 0xa5, 0x93, 0xd0, 0x29, 0x8c, 0x9e, 0x7a, 0xf6, 0x00, 0x20, 0x08,
    0x52, 0x08, 0xdc, 0x1d, 0x17, 0x8b, 0x44, 0x4e, 0x32, 0x13, 0xdc, 0xb1,
    0x50, 0xa5, 0xf0, 0x94, 0x25, 0x50, 0xfa, 0x1e, 0xe3, 0xae, 0x66, 0x19,
    0x13, 0x3e, 0x20, 0x86, 0x05, 0x9c, 0xda, 0xd9, 0xff, 0x8f, 0x72, 0x6e,
    0xf5, 0xc1, 0xfd, 0x86, 0xae, 0x26, 0xcc, 0x4b, 0xc8, 0x9f, 0xa9, 0xd6,
    0x3a, 0x1f, 0xb6, 0x8a, 0x8f, 0x04, 0x2d, 0xbb, 0xa4, 0x47, 0xb3, 0xfb,
    0xf9, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x0d, 0x30, 0x0b, 0x30, 0x09,
    0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x02, 0x30, 0x00, 0x30, 0x0d, 0x06,
    0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00,
    0x03, 0x82, 0x01, 0x01, 0x00, 0xa7, 0x35, 0x63, 0xc8, 0x0a, 0xf7, 0xae,
    0x0f, 0xe8, 0x10, 0x50, 0xb0, 0x79, 0x04, 0xa5, 0xf3, 0x48, 0x5d, 0x0a,
    0x7a, 0x6b, 0xd0, 0xef, 0x17, 0x43, 0xe5, 0x21, 0xd4, 0xbb, 0xef, 0xac,
    0x04, 0x21, 0x50, 0x7d, 0xd8, 0xe8, 0xf1, 0x3f, 0xd1, 0xb7, 0xa5, 0x93,
    0xb4, 0xa8, 0xec, 0x23, 0xbe, 0xb6, 0xd9, 0xcc, 0xbe, 0x3c, 0x81, 0x34,
    0x25, 0x24, 0x81, 0x1e, 0xd9, 0x8c, 0xd6, 0x20, 0x14, 0x36, 0x83, 0x29,
    0x7f, 0x56, 0xcd, 0xc3, 0x90, 0xd0, 0x2d, 0x54, 0x8b, 0x05, 0xcb, 0xab,
    0xb1, 0xf2, 0x44, 0xfc, 0xba, 0x73, 0xbf, 0x97, 0xc2, 0x2b, 0x5a, 0x6a,
    0x49, 0x27, 0x29, 0x7c, 0xb7, 0xb1, 0x4a, 0x1f, 0x28, 0x41, 0x05, 0x63,
    0x58, 0x8e, 0xd5, 0x7e, 0x46, 0x74, 0x11, 0x01, 0x72, 0x93, 0x1f, 0xea,
    0xf7, 0x37, 0x4a, 0xfa, 0x84, 0x53, 0xb6, 0x3c, 0x0e, 0xde, 0xe5, 0x1c,
    0x12, 0x86, 0x0a, 0xf6, 0x8b, 0xac, 0xc8, 0xb5, 0x9a, 0x9b, 0xd2, 0x28,
    0x15, 0x18, 0x83, 0x0a, 0xfc, 0x47, 0x1a, 0xcf, 0xed, 0xa1, 0x95, 0x4e,
    0xcc, 0x3c, 0x2a, 0x9a, 0xdf, 0x09, 0xec, 0x28, 0x20, 0xfd, 0xc5, 0x42,
    0xf1, 0xd8, 0x2f, 0x21, 0x88, 0xec, 0xe2, 0x24, 0xb2, 0xe2, 0x45, 0x5a,
    0xce, 0xb4, 0x78, 0xb3, 0x30, 0x38, 0x0e, 0x1c, 0x6a, 0xa3, 0x04, 0x0e,
    0xac, 0xa4, 0x97, 0xe3, 0xc1, 0x46, 0x0a, 0x9d, 0x65, 0x9a, 0xe9, 0x02,
    0x12, 0xf0, 0x88, 0x58, 0xc6, 0xde, 0xe5, 0x23, 0x42, 0x3c, 0x58, 0x52,
    0x27, 0x1a, 0xe2, 0xf5, 0x4a, 0x21, 0x47, 0xb2, 0x13, 0x0c, 0xb2, 0xd0,
    0xcc, 0xb3, 0xfd, 0x66, 0x2a, 0xa6, 0x38, 0x5b, 0xe6, 0x2e, 0x90, 0x9e,
    0x62, 0x3f, 0x7e, 0x60, 0xee, 0xd4, 0x02, 0x58, 0x7d, 0x5c, 0xf8, 0x39,
    0x27, 0xa9, 0xdb, 0x3e, 0x24, 0x3c, 0xc0, 0xde, 0xc8
};


#define SERVER "127.0.0.1"
//#define SERVER "17.201.58.114"
#define PORT 23232
#define BUFLEN 128
#define COUNT 10

#if 0
static void dumppacket(const unsigned char *data, unsigned long len)
{
    unsigned long i;
    for(i=0;i<len;i++)
    {
        if((i&0xf)==0) printf("%04lx :",i);
        printf(" %02x", data[i]);
        if((i&0xf)==0xf) printf("\n");
    }
    printf("\n");
}
#endif

/* 2K should be enough for everybody */
#define MTU 2048
static unsigned char readBuffer[MTU];
static unsigned int  readOff=0;
static size_t        readLeft=0;

static
OSStatus SocketRead(
                    SSLConnectionRef 	connection,
                    void 				*data,
                    size_t 				*dataLength)
{
    int fd = (int)connection;
    ssize_t len;
    uint8_t *d=readBuffer;

    if(readLeft==0)
    {
        len = read(fd, readBuffer, MTU);

        if(len>0) {
            readOff=0;
            readLeft=(size_t) len;
            printf("SocketRead: %ld bytes... epoch: %02x seq=%02x%02x\n",
                   len, d[4], d[9], d[10]);

        } else {
            int theErr = errno;
            switch(theErr) {
                case EAGAIN:
                    //printf("SocketRead: EAGAIN\n");
                    *dataLength=0;
                    /* nonblocking, no data */
                    return errSSLWouldBlock;
                default:
                    perror("SocketRead");
                    return errSecIO;
            }
        }
    }

    if(readLeft<*dataLength) {
        *dataLength=readLeft;
    }

    memcpy(data, readBuffer+readOff, *dataLength);
    readLeft-=*dataLength;
    readOff+=*dataLength;

    return errSecSuccess;

}

static
OSStatus SocketWrite(
                     SSLConnectionRef   connection,
                     const void         *data,
                     size_t 			*dataLength)	/* IN/OUT */
{
    int fd = (int)connection;
    ssize_t len;
    OSStatus err = errSecSuccess;
    const uint8_t *d=data;

#if 0
    if((rand()&3)==1) {

        /* drop 1/8th packets */
        printf("SocketWrite: Drop %ld bytes... epoch: %02x seq=%02x%02x\n",
               *dataLength, d[4], d[9], d[10]);
        return errSecSuccess;

    }
#endif

    len = send(fd, data, *dataLength, 0);

    if(len>0) {
        *dataLength=(size_t)len;
        printf("SocketWrite: Sent %ld bytes... epoch: %02x seq=%02x%02x\n",
               len, d[4], d[9], d[10]);
        return err;
    }

    int theErr = errno;
    switch(theErr) {
        case EAGAIN:
            /* nonblocking, no data */
            err = errSSLWouldBlock;
            break;
        default:
            perror("SocketWrite");
            err = errSecIO;
            break;
    }

    return err;

}


int main(int argc, char **argv)
{
    int fd;
    struct sockaddr_in sa;

    if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
        perror("socket");
        exit(-1);
    }

#ifdef NO_SERVER
# if DEBUG
    securityd_init();
# endif
#endif

    memset((char *) &sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(PORT);
    if (inet_aton(SERVER, &sa.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    time_t seed=time(NULL);
//    time_t seed=1298952499;
    srand((unsigned)seed);
    printf("Random drop initialized with seed = %lu\n", seed);

    if(connect(fd, (struct sockaddr *)&sa, sizeof(sa))==-1)
    {
        perror("connect");
        return errno;
    }

    /* Change to non blocking io */
    fcntl(fd, F_SETFL, O_NONBLOCK);

    SSLConnectionRef c=(SSLConnectionRef)(intptr_t)fd;


    OSStatus            ortn;
    SSLContextRef       ctx = NULL;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

    SSLClientCertificateState certState;
    SSLCipherSuite negCipher;
    SSLProtocol negVersion;

	/*
	 * Set up a SecureTransport session.
	 */
	ortn = SSLNewDatagramContext(false, &ctx);
	if(ortn) {
		printSslErrStr("SSLNewDatagramContext", ortn);
		return ortn;
	}
	ortn = SSLSetIOFuncs(ctx, SocketRead, SocketWrite);
	if(ortn) {
		printSslErrStr("SSLSetIOFuncs", ortn);
		return ortn;
	}

    ortn = SSLSetConnection(ctx, c);
	if(ortn) {
		printSslErrStr("SSLSetConnection", ortn);
		return ortn;
	}

    ortn = SSLSetMaxDatagramRecordSize(ctx, 400);
    if(ortn) {
		printSslErrStr("SSLSetMaxDatagramRecordSize", ortn);
        return ortn;
	}

    /* Lets not verify the cert, which is a random test cert */
    ortn = SSLSetEnableCertVerify(ctx, false);
    if(ortn) {
        printSslErrStr("SSLSetEnableCertVerify", ortn);
        return ortn;
    }

    ortn = SSLSetCertificate(ctx, chain_from_der(false, ServerRSA_Key_der, sizeof(ServerRSA_Key_der),
                                                 ServerRSA_Cert_CA_RSA_der, sizeof(ServerRSA_Cert_CA_RSA_der)));
    if(ortn) {
        printSslErrStr("SSLSetCertificate", ortn);
        return ortn;
    }

    do {
		ortn = SSLHandshake(ctx);
	    if(ortn == errSSLWouldBlock) {
		/* keep UI responsive */
		sslOutputDot();
	    }
    } while (ortn == errSSLWouldBlock);


    SSLGetClientCertificateState(ctx, &certState);
	SSLGetNegotiatedCipher(ctx, &negCipher);
	SSLGetNegotiatedProtocolVersion(ctx, &negVersion);

    int count;
    size_t len, readLen, writeLen;
    char buffer[BUFLEN];

    count = 0;
    while(count<COUNT) {
        int timeout = 10000;

        snprintf(buffer, BUFLEN, "Message %d", count);
        len = strlen(buffer);

        ortn=SSLWrite(ctx, buffer, len, &writeLen);
        if(ortn) {
            printSslErrStr("SSLWrite", ortn);
            break;
        }
        printf("Wrote %lu bytes\n", writeLen);

        count++;

        do {
            ortn=SSLRead(ctx, buffer, BUFLEN, &readLen);
        } while((ortn==errSSLWouldBlock) && (timeout--));
        if(ortn==errSSLWouldBlock) {
            printf("Echo timeout...\n");
            continue;
        }
        if(ortn) {
                printSslErrStr("SSLRead", ortn);
                break;
        }
        buffer[readLen]=0;
        printf("Received %lu bytes: %s\n", readLen, buffer);

     }

    SSLClose(ctx);

    SSLDisposeContext(ctx);

#pragma clang diagnostic pop

    return ortn;
}

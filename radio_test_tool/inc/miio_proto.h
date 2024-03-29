
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <endian.h>
#include <stdint.h>
#include <arpa/inet.h>

#define header_size  (32)
#define md5_size (16)


#ifdef __cplusplus
extern "C" {
#endif

void init_msg_head(char *msg_head,uint64_t indid,
        uint32_t stamp, const char* md5_sign, uint16_t length);

void parse_msg_head(const char* result_head, uint16_t* length,
        uint64_t* did, uint32_t* stamp, char* token);

void encrypt(const uint64_t indid, const char* token,const uint32_t stamp,const char* body, const int bodylen,char* msg,int msglength);

int decrypt(char* msg, const int msglen, const char* token,char* json,int jsonlen);

#ifdef __cplusplus
}
#endif

//
// Created by chenqiongyao on 2021/1/25.
//
#include "pdu_util.h"
#include "../log/log_util.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>
#include <arpa/inet.h>


PduUtil::PduUtil() {
}


int PduUtil::SetUserId(char *buf, int userId) {
    int user_id_offset = 4;
    int terminal_token = htonl(userId);
    memcpy(buf + user_id_offset, (char *) (&terminal_token), sizeof(int));
}


/***********************************************************
 * @PDU解析
 *  see the define in file
 *  pdu_base.h
 *
 */
int PduUtil::OnPduParse(char *buffer, int length, PDUBase &base /*return value*/ ) {
    if (length <= 0) {
        return 0;
    }

    char *position = buffer;
    int *startFlag = (int *) position;  //读取前面4字节

    if (ntohl(*startFlag) != PDUBase::start_flag) {  //是否未包头标识
        LOGE("start_flag != PDUBase::start_flag");
        return -1;
    }
    position += sizeof(int);

    char *pdu_type = (char *) position;
    base.offset = *pdu_type;
    position += sizeof(char);

    int *offset = (int *) position;
    base.offset = ntohl(*offset);
    position += sizeof(int);

    int *size = (int *) position;
    base.size = ntohl(*size);
    position += sizeof(int);

    long *presentationTimeUs = (long *) position;
    base.presentationTimeUs = ntohq(*presentationTimeUs);
    position += sizeof(long);

    int *flags = (int *) position;
    base.flags = ntohl(*flags);
    position += sizeof(int);

    int *reserved = (int *) position;
    base.reserved = ntohl(*reserved);
    position += sizeof(int);

    int *len = (int *) position;
    base.length = ntohl(*len);
    position += sizeof(int);

    LOGD("PDUBase receive body length:[%d]", base.length);
    //not a full package.
    if (position - buffer + base.length > length)
        return 0;

    std::shared_ptr<char> pBody(new char[base.length]);
    memcpy(pBody.get(), position, base.length);
    base.body = pBody;

    return position - buffer + base.length;
}


/****************************************************
 *  const static int start_flag;
    int command_id ;
    int seq_id;
    int length ;
    std::shared_ptr<char> body ;
 */
int PduUtil::OnPduPack(PDUBase &base, char *&outBuffer /*this is return value*/) {
    int totalLen = 0;

    int startFlag = htonl(PDUBase::start_flag);
    totalLen += sizeof(int);

    char pdu_type = base.pdu_type;
    totalLen += sizeof(char);

    int protoOffset = htonl(base.offset);
    totalLen += sizeof(int);

    int size = htonl(base.size);
    totalLen += sizeof(int);

    long presentationTimeUs = htonq(base.presentationTimeUs);
    totalLen += sizeof(long);
    LOGD("size of long:[%d]", sizeof(long));

    int flags = htonl(base.flags);
    totalLen += sizeof(int);

    int reserved = htonl(base.reserved);
    totalLen += sizeof(int);

    int protoLen = htonl(base.length);
    totalLen += sizeof(int);

    totalLen += base.length;
    char *buf = (char *) malloc(totalLen); //malloc memory,need free
    if (buf == nullptr)
        return -1;

    int offset = 0;
    memcpy(buf + offset, (char *) (&startFlag), sizeof(int));
    offset += sizeof(int);

    memcpy(buf + offset, (char *) (&pdu_type), sizeof(char));
    offset += sizeof(char);

    memcpy(buf + offset, (char *) (&offset), sizeof(int));
    offset += sizeof(int);

    memcpy(buf + offset, (char *) (&size), sizeof(int));
    offset += sizeof(int);

    memcpy(buf + offset, (char *) (&presentationTimeUs), sizeof(long));
    offset += sizeof(long);

    memcpy(buf + offset, (char *) (&flags), sizeof(int));
    offset += sizeof(int);

    memcpy(buf + offset, (char *) (&reserved), sizeof(int));
    offset += sizeof(int);

    memcpy(buf + offset, (char *) (&protoLen), sizeof(int));
    offset += sizeof(int);

    memcpy(buf + offset, base.body.get(), base.length);
    outBuffer = buf;

    LOGD("PDUBase send body length:[%d]", base.length);
    return totalLen;

}





//
// Created by chenqiongyao on 2021/1/25.
//
#include "pdu_util.h"
#include "log_util.h"

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
    char *position = buffer;
    int *startFlag = (int *) position;
    if (length <= 0) {
        return 0;
    }
    if (ntohl(*startFlag) != PDUBase::start_flag) {
        LOGD("start_flag != PDUBase::start_flag");
        return -1;
    }

    position += sizeof(int);
    int *terminalToken = (int *) position;
    base.terminal_token = ntohl(*terminalToken);

    position += sizeof(int);
    int *commandId = (int *) position;
    base.command_id = ntohl(*commandId);

    position += sizeof(int);
    int *seqId = (int *) position;
    base.seq_id = ntohl(*seqId);

    position += sizeof(int);
    int *len = (int *) position;
    base.length = ntohl(*len);


    position += sizeof(int);

    //not a full package.
    if (position - buffer + base.length > length)
        return 0;

    std::shared_ptr<char> pBody(new char[base.length + 1]);
    memcpy(pBody.get(), position, base.length);
    base.body = pBody;


    return position - buffer + base.length;
}


/****************************************************
 *  const static int startflag = 88888888 ;
    int command_id ;
    int seq_id;
    int length ;
    std::shared_ptr<char> body ;

 */
int PduUtil::OnPduPack(PDUBase &base, char *&outBuf /*this is return value*/) {
    int totalLen = 0;

    int startFlag = htonl(PDUBase::start_flag);
    totalLen += sizeof(int);
    int terminalToken = htonl(base.terminal_token);
    totalLen += sizeof(int);
    int commandId = htonl(base.command_id);
    totalLen += sizeof(int);
    int seq = htonl(base.seq_id);
    totalLen += sizeof(int);
    int protoLen = htonl(base.length);
    totalLen += sizeof(int);

    totalLen += base.length;
    char *buf = (char *) malloc(totalLen);
    if (buf == nullptr)
        return -1;

    int offset = 0;
    memcpy(buf + offset, (char *) (&startFlag), sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, (char *) (&terminalToken), sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, (char *) (&commandId), sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, (char *) (&seq), sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, (char *) (&protoLen), sizeof(int));
    offset += sizeof(int);
    memcpy(buf + offset, base.body.get(), base.length);
    outBuf = buf;

    LOGT(buf);

    return totalLen;

}





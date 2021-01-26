#ifndef PDU_UTIL_H
#define PDU_UTIL_H

#include "pdu_base.h"


class PduUtil {
public:

    PduUtil();

    /***********************************************************
     * @PDU解析
     * if, default pdu did not met your requirement, override this function.
     * if, error format, return -1.
     * if, has package, return readed package.
     * if, does not have a full package, return 0.
     * if, IT'S OVERRIDER, OnPduPack also need to be override.
     */
    virtual int OnPduParse(char *buffer, int length, PDUBase &base /*return value*/ );


    /***********************************************************
     * @pdu封包
     */
    virtual int OnPduPack(PDUBase &base, char *&outBuffer);

    int SetUserId(char *buf, int userId);

};

#endif // PDU_UTIL_H

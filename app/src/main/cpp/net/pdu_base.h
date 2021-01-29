#ifndef PDU_BASE_H
#define PDU_BASE_H

#include <memory>   //shared_ptr

class PDUBase {
public:
    PDUBase();

    /********************************************
     * index 0, [0,4)
     * start flag.
     */
    const static int start_flag = 0X12345678;

    int pdu_type;

    int offset;

    int size;

    long presentationTimeUs;

    int flags;

    int reserved;

    int length;

    std::shared_ptr<char> body;
};

#endif // PDU_BASE_H

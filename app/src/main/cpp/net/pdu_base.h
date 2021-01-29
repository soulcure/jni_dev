#ifndef PDU_BASE_H
#define PDU_BASE_H

#include <memory>   //shared_ptr

class PDUBase {
public:
    PDUBase();

    const static char LOCAL_BYTES = 0x00;
    const static char LOCAL_STRING = 0x01;
    const static char VIDEO_FRAME = 0x02;
    const static char AUDIO_FRAME = 0x03;
    const static char PING_MSG = 0x0E;
    const static char PONG_MSG = 0x0F;

    /********************************************
     * index 0, [0,4)
     * start flag.
     */
    const static int start_flag = 0X12345678;

    char pdu_type;

    int offset;

    int size;

    long presentationTimeUs;

    int flags;

    int reserved;

    int length;

    std::shared_ptr<char> body;
};

#endif // PDU_BASE_H

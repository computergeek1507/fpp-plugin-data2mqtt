#pragma once

#include <stdlib.h>
#include <list>
#include <string>

class MQTTItem {
public:
    MQTTItem(std::string const& _topic, std::string const& _payload, unsigned int startChannel );
    ~MQTTItem();

    bool BulbWhite();

    std::string GetTopic(){return _topic;}
    std::string GetPayload(){return _payload;}
    bool SendData( unsigned char *data);

    unsigned int GetStartChannel(){return _startChannel;}

private:

    unsigned int _startChannel;
    unsigned int _seqCount;
    std::string _topic;
    std::string _payload;
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;

    void outputData( uint8_t r ,uint8_t g ,uint8_t b );

    void replaceValues(std::string & valueStr, uint8_t r ,uint8_t g ,uint8_t b );
    void RGBtoHSIV(float fR, float fG, float fB, float& fH, float& fSI, float& fSV,float& fI, float& fV);
};
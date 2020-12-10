#include "MQTTItem.h"

#include "mqtt.h"
#include "common.h"
#include "log.h"

#include <stdlib.h>
#include <cstdint>
#include <thread> 
#include <cmath>

#include <iostream>
#include <istream>
#include <ostream>

MQTTItem::MQTTItem(std::string const& topic, std::string const& payload,  unsigned int startChannel) :
    _startChannel(startChannel),
    _topic(topic),
    _payload(payload),
    _r(0),
    _g(0),
    _b(0)
{

}

MQTTItem::~MQTTItem()
{ }



bool MQTTItem::BulbWhite()
{
    try
    {
        uint8_t r = 255;
        uint8_t g = 255;
        uint8_t b = 255;

        //std::thread t(&MQTTItem::outputData, this, r, g, b );
        //t.detach();
        outputData(r, g, b );
        return true;
    }
    catch(std::exception ex)
    {
        LogInfo(VB_PLUGIN, ex.what());
    }
    return false;
}

bool MQTTItem::SendData( unsigned char *data)
{
    try
    {
        uint8_t r = data[_startChannel - 1];
        uint8_t g = data[_startChannel];
        uint8_t b = data[_startChannel + 1];

        if(r == _r && g == _g && b == _b) {
            return true;
        }
        _r = r;
        _g = g;
        _b = b;

        //std::thread t(&MQTTItem::outputData, this, r, g, b );
        //t.detach();
        outputData(r, g, b );
        return true;
    }
    catch(std::exception ex)
    {
        LogInfo(VB_PLUGIN, ex.what());
    }
    return false;
}

void MQTTItem::outputData( uint8_t r ,uint8_t g ,uint8_t b )
{
    if(!mqtt->IsConnected())
        return;
    if(_topic.empty())
        return;
    if(_payload.empty())
        return;
    std::string payload = _payload;
    replaceAll(payload, "%R%" , std::to_string(r));
    replaceAll(payload, "%G%" , std::to_string(g));
    replaceAll(payload, "%B%" , std::to_string(b));
    
    float h,s,i;

    RGBtoHSI(r/255.0,r/255.0,r/255.0,h,s,i);
    
    int ih = (h*100)/360;
    int is = (s*100);
    int ii = (i*100);
    
    replaceAll(payload, "%H%" , std::to_string(ih));
    replaceAll(payload, "%S%" , std::to_string(is));
    replaceAll(payload, "%I%" , std::to_string(ii));

    mqtt->PublishRaw(_topic,payload);
}

void MQTTItem::RGBtoHSI(float fR, float fG, float fB, float& fH, float& fS, float& fI) {
    float M  = std::max(std::max(fR, fG), fB);
    float m = std::min(std::min(fR, fG), fB);
    float c = M-m;
  
    fI = (1.0/3.0)*(fR+fG+fB);
  
    if(c==0) {
        fH = 0.0;
        fS = 0.0;
    }
    else {
        if(M==fR) {
            fH = fmod(((fG-fB)/c), 6.0);
        }
        else if(M==fG) {
            fH = (fB-fR)/c + 2.0;
        }
        else if(M==fB) {
            fH = (fR-fG)/c + 4.0;
        }
        fH *=60.0;
        if(fI!=0) {
            fS = 1.0 - (m/fI);
        }
        else {
            fS = 0.0;
        }
    }
}

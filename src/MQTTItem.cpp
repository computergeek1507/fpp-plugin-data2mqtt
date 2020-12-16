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
    _b(0),
    _seqCount(0)
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
            if(_seqCount<1201) {
                ++_seqCount;
                return true;
            }
        }
        _seqCount=0;
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

    replaceValues(payload, r, g, b);

    mqtt->PublishRaw(_topic,payload);
}

void MQTTItem::replaceValues(std::string & valueStr, uint8_t r ,uint8_t g ,uint8_t b )
{
    replaceAll(valueStr, "%R%" , std::to_string(r));
    replaceAll(valueStr, "%G%" , std::to_string(g));
    replaceAll(valueStr, "%B%" , std::to_string(b));

    int rScale = (r * 100)/255;
    int gScale = (g * 100)/255;
    int bScale = (b * 100)/255;

    replaceAll(valueStr, "%RS%" , std::to_string(rScale));
    replaceAll(valueStr, "%GS%" , std::to_string(gScale));
    replaceAll(valueStr, "%BS%" , std::to_string(bScale));

    if(r > 254)
        replaceAll(valueStr, "%SW%" , "ON");
    else
        replaceAll(valueStr, "%SW%" , "OFF");

    float h,si,sv,i,v;

    RGBtoHSIV(r/255,g/255,b/255,h,si,sv,i,v);
    
    int ih = (h);
    int isi = (si*100);
    int isv = (sv*100);
    int ii = (i*100);
    int iv = (v*100);
    
    replaceAll(valueStr, "%H%" , std::to_string(ih));
    replaceAll(valueStr, "%S%" , std::to_string(isi));
    replaceAll(valueStr, "%SI%" , std::to_string(isi));
    replaceAll(valueStr, "%SV%" , std::to_string(isv));
    replaceAll(valueStr, "%I%" , std::to_string(ii));
    replaceAll(valueStr, "%V%" , std::to_string(iv));
}

void MQTTItem::RGBtoHSIV(float fR, float fG, float fB, float& fH, float& fSI, float& fSV,float& fI, float& fV) {
    float M  = std::max(std::max(fR, fG), fB);
    float m = std::min(std::min(fR, fG), fB);
    float c = M-m;
    fV = M;
    //fL = (1.0/2.0)*(M+m);
    fI = (1.0/3.0)*(fR+fG+fB);
  
    if(c==0) {
        fH = 0.0;
        fSI = 0.0;
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
            fSI = 1.0 - (m/fI);
        }
        else {
            fSI = 0.0;
        }
    }

    fSV = M == 0 ? 0 : (M - m) / M;

    if(fH < 0.0)
        fH += 360.0;
}

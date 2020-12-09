#include "MQTTItem.h"

//#include "mqtt.h"

#include <stdlib.h>
#include <cstdint>
#include <thread> 

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

bool MQTTItem::BulbOn()
{
    try
    {
        std::thread t(&MQTTItem::sendBulbOn, this );
        t.detach();
        //sendBulbOn();
        return true;
    }
    catch(std::exception ex)
    {
        std::cout << ex.what();
    }
    return false;
}

bool MQTTItem::BulbOff()
{
    try
    {
        std::thread t(&MQTTItem::sendBulbOff, this );
        t.detach();
        //sendBulbOff();
        return true;
    }
    catch(std::exception ex)
    {
        std::cout << ex.what();
    }
    return false;
}

bool MQTTItem::BulbWhite()
{
    try
    {
        uint8_t r = 255;
        uint8_t g = 255;
        uint8_t b = 255;

        std::thread t(&MQTTItem::outputData, this, r, g, b );
        t.detach();
        //outputData(r, g, b );
        return true;
    }
    catch(std::exception ex)
    {
        std::cout << ex.what();
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

        std::thread t(&MQTTItem::outputData, this, r, g, b );
        t.detach();
        //outputData(r, g, b );
        return true;
    }
    catch(std::exception ex)
    {
        std::cout << ex.what();
    }
    return false;
}

void MQTTItem::sendBulbOn()
{

}

void MQTTItem::sendBulbOff()
{
    

}

void MQTTItem::outputData( uint8_t r ,uint8_t g ,uint8_t b )
{
    std::string repURL = "http://" + _topic + "/cm?cmnd=Color%20" + std::to_string(r)
    + "," + std::to_string(g) + ","  + std::to_string(b);

    
}

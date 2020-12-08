#include "TasmotaBulb.h"

#include <stdlib.h>
#include <cstdint>
#include <thread> 

#include <iostream>
#include <istream>
#include <ostream>

TasmotaBulb::TasmotaBulb(std::string const& ip, unsigned int startChannel) :
    _startChannel(startChannel),
    _ipAddress(ip),
    _r(0),
    _g(0),
    _b(0),
    unreachable(false),
    m_curl(NULL)
{
    m_curl = curl_easy_init();
}

TasmotaBulb::~TasmotaBulb()
{
    if (m_curl)
        curl_easy_cleanup(m_curl);
}

bool TasmotaBulb::BulbOn()
{
    try
    {
        std::thread t(&TasmotaBulb::sendBulbOn, this );
        t.detach();
        //sendBulbOn();
        return true;
    }
    catch(std::exception ex)
    {
        unreachable = true;
        std::cout << ex.what();
    }
    return false;
}

bool TasmotaBulb::BulbOff()
{
    try
    {
        std::thread t(&TasmotaBulb::sendBulbOff, this );
        t.detach();
        //sendBulbOff();
        return true;
    }
    catch(std::exception ex)
    {
        unreachable = true;
        std::cout << ex.what();
    }
    return false;
}

bool TasmotaBulb::BulbWhite()
{
    try
    {
        if(unreachable)
            return false;

        uint8_t r = 255;
        uint8_t g = 255;
        uint8_t b = 255;

        std::thread t(&TasmotaBulb::outputData, this, r, g, b );
        t.detach();
        //outputData(r, g, b );
        return true;
    }
    catch(std::exception ex)
    {
        unreachable = true;
        std::cout << ex.what();
    }
    return false;
}

bool TasmotaBulb::SendData( unsigned char *data)
{
    try
    {
        if(unreachable)
            return false;

        uint8_t r = data[_startChannel - 1];
        uint8_t g = data[_startChannel];
        uint8_t b = data[_startChannel + 1];

        if(r == _r && g == _g && b == _b) {
            return true;
        }
        _r = r;
        _g = g;
        _b = b;

        std::thread t(&TasmotaBulb::outputData, this, r, g, b );
        t.detach();
        //outputData(r, g, b );
        return true;
    }
    catch(std::exception ex)
    {
        unreachable = true;
        std::cout << ex.what();
    }
    return false;
}

void TasmotaBulb::sendBulbOn()
{
    std::string repURL = "http://" + _ipAddress + "/cm?cmnd=Power%20On";
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(m_curl, CURLOPT_URL, repURL.c_str());

    CURLcode status = curl_easy_perform(m_curl);
    if (status != CURLE_OK) {
        unreachable = true;
        std::cout << "failed to send on command\n";
        return;
    }
    unreachable = false;
}

void TasmotaBulb::sendBulbOff()
{
    std::string repURL = "http://" + _ipAddress + "/cm?cmnd=Power%20Off";
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(m_curl, CURLOPT_URL, repURL.c_str());

    CURLcode status = curl_easy_perform(m_curl);
    if (status != CURLE_OK) {
        unreachable = true;
        std::cout << "failed to send on command\n";
        return;
    }
    unreachable = false;
}

void TasmotaBulb::outputData( uint8_t r ,uint8_t g ,uint8_t b )
{
    std::string repURL = "http://" + _ipAddress + "/cm?cmnd=Color%20" + std::to_string(r)
    + "," + std::to_string(g) + ","  + std::to_string(b);
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(m_curl, CURLOPT_URL, repURL.c_str());

    CURLcode status = curl_easy_perform(m_curl);
    if (status != CURLE_OK) {
        unreachable = true;
        std::cout << "failed to send color command\n";
    }
}

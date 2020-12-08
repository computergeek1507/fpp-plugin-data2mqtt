
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cstring>

#include <istream>
#include <ostream>

#include <iostream> 

#include <thread> 

#include <vector>

#include <unistd.h>
#include <termios.h>
#include <chrono>
#include <thread>
#include <cmath>

#include <httpserver.hpp>
#include "common.h"
#include "settings.h"
#include "Plugin.h"
#include "Plugins.h"
#include "log.h"
#include "channeloutput/channeloutput.h"
#include "mqtt.h"
#include "MultiSync.h"

#include "fppversion_defines.h"

#include "commands/Commands.h"

#include "TasmotaBulb.h"

class FPPTasmotaPlugin : public FPPPlugin, public httpserver::http_resource {
private:
    std::vector<std::unique_ptr <TasmotaBulb>> _tasmotaOutputs;
    Json::Value config;

public:

    FPPTasmotaPlugin() : FPPPlugin("fpp-tasmota-plugin") {
        printf ("FPP-Tasmota-Plugin Starting\n");
        readFiles();
    }
    virtual ~FPPTasmotaPlugin() 
    {
        _tasmotaOutputs.clear();
    }

    virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request &req) override {
        std::string v = getIPs();
        return std::shared_ptr<httpserver::http_response>(new httpserver::string_response(v, 200));
    }
    
#if FPP_MAJOR_VERSION < 4 || FPP_MINOR_VERSION < 1
    virtual void modifyChannelData(int ms, uint8_t *seqData) override {
#else
    virtual void modifySequenceData(int ms, uint8_t *seqData) override {
#endif
        try
        {
            sendChannelData(seqData);
        }
        catch(std::exception ex)
        {
            std::cout << ex.what();
        }
    }

    virtual void playlistCallback(const Json::Value &playlist, const std::string &action, const std::string &section, int item) {
        if (settings["Start"] == "PlaylistStart" && action == "start") {
            sendBulbsOn();
        }  
    }
    
    void sendBulbsOn() {
        for(auto & output: _tasmotaOutputs)
        {
            output->BulbOn();
        }
    }

    void sendChannelData(unsigned char *data) {
        for(auto & output: _tasmotaOutputs)
        {
            output->SendData(data);
        }
    }
    
    void saveDataToFile()
    {
        std::ofstream outfile;
        outfile.open ("/home/fpp/media/config/fpp-tasmota-plugin");

        if(_tasmotaOutputs.size() ==0) {
            outfile <<  "nooutputsfound,1";
            outfile <<  "\n";
        }

        for(auto & out: _tasmotaOutputs) {
            outfile << out->GetIPAddress();
            outfile <<  ",";
            outfile << out->GetStartChannel();
            outfile <<  "\n";
        }
        outfile.close();
    }


    void readFiles()
    {
        //read ip and start channel settings from JSON setting file. 
        if (LoadJsonFromFile("/home/fpp/media/config/plugin.fpp-tasmota.json", config)) {
            for (int i = 0; i < config.size(); i++) {
                std::string const ip = config[i]["ip"].asString();
                unsigned int sc =  config[i]["startchannel"].asInt();
                if(!ip.empty()) {
                    std::unique_ptr<TasmotaBulb> bulb = std::make_unique<TasmotaBulb>(ip,sc);
                    printf ("Adding Bulb %s\n" ,bulb->GetIPAddress().c_str());
                    _tasmotaOutputs.push_back(std::move(bulb));
                }
            }
        }
        saveDataToFile();
    }
    
    std::string getIPs()
    {
        std::string ips;
        for(auto & out: _tasmotaOutputs) {
            ips += out->GetIPAddress();
            ips += ",";
        }
        printf ("IP Adresses %s\n" ,ips.c_str());
        return ips;
    } 
};


extern "C" {
    FPPPlugin *createPlugin() {
        return new FPPTasmotaPlugin();
    }
}

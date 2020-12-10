
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

#include "MQTTItem.h"

class Data2MQTTPlugin : public FPPPlugin, public httpserver::http_resource {
private:
    std::vector<std::unique_ptr <MQTTItem>> _MQTTOutputs;
    Json::Value config;

public:

    Data2MQTTPlugin() : FPPPlugin("fpp-plugin-data2mqtt") {
        LogInfo(VB_PLUGIN, "Initializing Data2MQTT Plugin\n");
        readFiles();
    }
    virtual ~Data2MQTTPlugin() 
    {
        _MQTTOutputs.clear();
    }

    virtual const std::shared_ptr<httpserver::http_response> render_GET(const httpserver::http_request &req) override {
        std::string v = getTopics();
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
            //sendBulbsOn();
        }  
    }
    

    void sendChannelData(unsigned char *data) {
        for(auto & output: _MQTTOutputs)
        {
            output->SendData(data);
        }
    }
    
    void saveDataToFile()
    {
        std::ofstream outfile;
        outfile.open ("/home/fpp/media/config/fpp-plugin-data2mqtt");

        if(_MQTTOutputs.size() ==0) {
            outfile <<  "nooutputsfound,1";
            outfile <<  "\n";
        }

        for(auto & out: _MQTTOutputs) {
            outfile << out->GetTopic();
            outfile <<  ",";
            outfile << out->GetPayload();
            outfile <<  ",";
            outfile << out->GetStartChannel();
            outfile <<  "\n";
        }
        outfile.close();
    }


    void readFiles()
    {
        //read topic, payload and start channel settings from JSON setting file. 
        if (LoadJsonFromFile("/home/fpp/media/config/plugin.data2mqtt.json", config)) {
            for (int i = 0; i < config.size(); i++) {
                std::string const topic = config[i]["topic"].asString();
                std::string const payload = config[i]["payload"].asString();
                unsigned int sc =  config[i]["startchannel"].asInt();
                if(!topic.empty()) {
                    std::unique_ptr<MQTTItem> mqttItem = std::make_unique<MQTTItem>(topic,payload,sc);
                    _MQTTOutputs.push_back(std::move(mqttItem));
                }
            }
        }
        saveDataToFile();
    }
    
    std::string getTopics()
    {
        std::string topics;
        for(auto & out: _MQTTOutputs) {
            topics += out->GetTopic();
            topics += ",";
        }
        return topics;
    } 
};


extern "C" {
    FPPPlugin *createPlugin() {
        return new Data2MQTTPlugin();
    }
}

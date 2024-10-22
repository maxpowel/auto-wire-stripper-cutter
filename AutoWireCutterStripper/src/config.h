#ifndef _CONFIG_
#define _CONFIG_

struct config {
    int steps_mm_extruder;
    int steps_mm_linear;
};

struct config _CONFIG;

#include <LittleFS.h>

struct config * get_config() {
    return &_CONFIG;
}

void writeConfig(){
    File file = LittleFS.open("config", FILE_WRITE);
    file.write((uint8_t*)&_CONFIG, sizeof(_CONFIG));
    file.close();
}

void readConfig(){
    /*if(LittleFS.exists("config")) {
        File file = LittleFS.open("config");
        file.read((uint8_t*)&_CONFIG, sizeof(_CONFIG));
        file.close();
    } else {*/
        _CONFIG.steps_mm_extruder = 135;
        _CONFIG.steps_mm_linear = 404;
        /*writeConfig();
    }*/
    
}


int init_config() {
    /*int ret = LittleFS.begin(true);
    if (ret != 0) {
        return ret;
    }
    readConfig();*/
    return 0;
}

#endif
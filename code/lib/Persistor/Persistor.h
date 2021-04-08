#include <EEPROM.h>
#include <myTypes.h>

class Persistor
{
public:
    Persistor();
    void saveConfig();
    void loadConfig();
    void eraseConfig();
    void loadCredentials();
    void saveCredentials();
    void setCredentials(String ssid, String password);
    
    Credentials credentials;
    Config config;

private:
    const int eepromSize = sizeof credentials + sizeof config + 20;

    // do not change! credentials for in-code change!
    // char ssid[32] = "";
    // char password[32] = "";

    int configStart = sizeof credentials + 10;
    bool USESERIAL = true;

    bool isStored(char isStored[3]);

};
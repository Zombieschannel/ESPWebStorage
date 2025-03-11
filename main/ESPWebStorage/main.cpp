#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD_MMC.h>
#include <iostream>
#include <vector>
#include <SPIFFS.h>
#include <WiFi.h>
#define HTTP_UPLOAD_BUFLEN 8192
#define HTTP_RAW_BUFLEN 8192
#include <WebServer.h>
#include "JSON.hpp"
#include "nvs_flash.h"
#include "Site.hpp"
#include <ctime>
using namespace std;

#include "../../wifi/ssid_pass.txt" //const char *ssid = ""; //const char *password = "";

unique_ptr<WebServer> server;
void handleClients(void* data)
{
    while (1)
    {
        server->handleClient();
    }
}

uint64_t totalBytesStreamed = 0;
File* processingFile = nullptr;
bool done = true;
JSON json;

static const unsigned char upload[] =
{
0b00000001, 0b10000000, 
0b00000011, 0b11000000, 
0b00000111, 0b11100000, 
0b00001111, 0b11110000, 
0b00011111, 0b11111000, 
0b00111111, 0b11111100, 
0b01111111, 0b11111110, 
0b11111011, 0b11011111, 
0b11110011, 0b11001111, 
0b11100011, 0b11000111, 
0b00000011, 0b11000000, 
0b00000011, 0b11000000, 
0b00000011, 0b11000000, 
0b00000011, 0b11000000, 
0b00000011, 0b11000000, 
0b00000011, 0b11000000,
};
static const unsigned char downsample[7][8] =
{
    {
        0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111,
    },
    {
        0b00000000, 0b11111110, 0b11111110, 0b11111110, 0b11111110, 0b11111110, 0b11111110, 0b11111110,
    },
    {
        0b00000000, 0b00000000, 0b11111100, 0b11111100, 0b11111100, 0b11111100, 0b11111100, 0b11111100,
    },
    {
        0b00000000, 0b00000000, 0b00000000, 0b11111000, 0b11111000, 0b11111000, 0b11111000, 0b11111000,
    },
    {
        0b00000000, 0b00000000, 0b00000000, 0b01111000, 0b01111000, 0b01111000, 0b01111000, 0b00000000,
    },
    {
        0b00000000, 0b00000000, 0b00000000, 0b00111000, 0b00111000, 0b00111000, 0b00000000, 0b00000000,
    },
    {
        0b00000000, 0b00000000, 0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b00000000, 0b00000000,
    },
};
class StorageServer
{
    TwoWire wire;
    unique_ptr<Adafruit_SSD1306> display;
public:
StorageServer()
        : wire(Wire)
    {
        usb_serial_jtag_driver_config_t usb_serial_jtag_config;
        usb_serial_jtag_config.rx_buffer_size = 256;
        usb_serial_jtag_config.tx_buffer_size = 256;

        esp_err_t ret = ESP_OK;
        ret = usb_serial_jtag_driver_install(&usb_serial_jtag_config);
        if (ret != ESP_OK)
        {
            printf("usb_serial_jtag_driver_install fail\n");
            return;
        }
        wire.begin(5, 6);
        display = make_unique<Adafruit_SSD1306>(128, 32, &wire, -1);
    }
    static vector<FileInfo> FilesInDir(const string &path)
    {
        vector<FileInfo> files;
        FileInfo fi;
        bool isDir;
        File f = SD_MMC.open(path.c_str(), "r");
        while (1)
        {
            isDir = false;
            string name = f.getNextFileName(&isDir).c_str();
            if (name == "")
                break;
            {
                File fl = SD_MMC.open(name.c_str(), "r");
                fi.fileSize = fl.size();

                time_t time = fl.getLastWrite();
                tm* timeInfo = localtime(&time);
                fi.day = timeInfo->tm_mday;
                fi.month = timeInfo->tm_mon;
                fi.year = timeInfo->tm_year;
                fl.close();
            }
            fi.name = name.substr(path.size() + 1);
            if (isDir)
                fi.type = 0;
            else
            {
                if (name.ends_with(".PNG") || name.ends_with(".JPG") || name.ends_with(".BMP"))
                    fi.type = 1;
                else if (name.ends_with(".MP4") || name.ends_with(".MOV") || name.ends_with(".MKV"))
                    fi.type = 2;
                else
                    fi.type = 3;
            }
            files.push_back(fi);
        }
        f.close();
        return files;
    }
    static void sendFile(const string& fileName, bool download = false)
    {
        uint64_t Clock = millis();
        setCpuFrequencyMhz(240);
        File f = SD_MMC.open(fileName.c_str(), "r");

        if (download)
            server->sendHeader("Content-Disposition", String("attachment;filename=") + fileName.substr(fileName.rfind("/") + 1).c_str(), true);

        processingFile = &f;
        done = false;
        if (fileName.ends_with(".JPG"))
            server->streamFile(f, "image/jpeg");
        else if (fileName.ends_with(".PNG"))
            server->streamFile(f, "image/png");
        else if (fileName.ends_with(".MP4"))
            server->streamFile(f, "image/mp4");
        else
            server->streamFile(f, "image/application/octet-stream"); 

        setCpuFrequencyMhz(80);
        ESP_LOGE("WebServer", "%s Time: %.2f", fileName.c_str(), (millis() - Clock) / 1000.f);
        done = true;
        delay(10);
        processingFile = nullptr;
    }
    static void page_notFound()
    {
        string uri = server->uri().c_str();
        if (uri[0] == '/')
            uri.erase(0, 1);
        if (json.exists(uri.substr(0, uri.find('/'))) && uri.find("..") == string::npos)
        {
            string path = json.getValue(uri.substr(0, uri.find('/')));
            string filename = "";
            if (uri.find('/') != string::npos)
                filename = uri.substr(uri.find('/'));
            if (SD_MMC.exists((path + filename).c_str()))
            {
                if (uri.contains('.'))
                {
                    ESP_LOGE("WebServer", "200");
                    sendFile(path + filename, server->hasArg("download"));
                }
                else
                {
                    ESP_LOGE("WebServer", "200");
                    ESP_LOGE("WebServer", "%s", server->uri().c_str());

                    string returnSite;
                    vector<FileInfo> files = FilesInDir(path + filename);
                    if (server->hasArg("tile"))
                    {
                        returnSite = websiteTile;
                        addEntriesTile(returnSite, files, uri);
                    }
                    else
                    {
                        returnSite = websiteList;
                        addEntriesList(returnSite, files, uri);
                    }
                server->send(200, "text/html", returnSite.c_str());
                }
            }
            else
            {
                ESP_LOGE("WebServer", "404");
                ESP_LOGE("WebServer", "%s", server->uri().c_str());
                server->send(404, "text/plain", "Not found");
            }
        }
        else if (server->uri() == "/BG.PNG" || server->uri() == "/FOLDER.PNG" || server->uri() == "/IMAGE.PNG" || 
            server->uri() == "/VIDEO.PNG" || server->uri() == "/OTHER.PNG" || server->uri() == "/MONTSERR.TTF" || 
            (server->uri().startsWith("/thumb/") && !string(server->uri().c_str()).contains("..")))
        {
            ESP_LOGE("WebServer", "200");
            sendFile(server->uri().c_str(), server->hasArg("download"));
        }
        else
        {
            ESP_LOGE("WebServer", "404");
            ESP_LOGE("WebServer", "%s", server->uri().c_str());
            server->send(404, "text/plain", "Not found");
        }
    }
    string command(const string& input)
    {
        static string currentDirectory = "/server";
        string first = input.substr(0, input.find(' '));
        if (first == "ls")
        {
            string returnData = "";
            FileInfo fi;
            bool isDir;
            File f = SD_MMC.open(currentDirectory.c_str(), "r");
            while (1)
            {
                isDir = false;
                string name = f.getNextFileName(&isDir).c_str();
                if (name == "")
                    break;
                {
                    File fl = SD_MMC.open(name.c_str(), "r");
                    fi.fileSize = fl.size();

                    time_t time = fl.getLastWrite();
                    tm* timeInfo = localtime(&time);
                    fi.day = timeInfo->tm_mday;
                    fi.month = timeInfo->tm_mon;
                    fi.year = timeInfo->tm_year;
                    fl.close();
                }
                fi.name = name.substr(currentDirectory.size() + 1);

                returnData += to_string(fi.day) + "/" + to_string(fi.month + 1) + "/" + to_string(fi.year + 1900) + " " + to_string(fi.fileSize) + " " + to_string(isDir) + " " + fi.name + "\n";
            }
            f.close();
            return returnData + currentDirectory;
        }
        else if (first == "cd")
        {
            string second = input.substr(input.find(' ') + 1);
            for (int i = 0; i < second.size(); i++)
            {
                if (second[i] == ' ')
                {
                    second.erase(i);
                    i--;
                }
            }
            if (second == "..")
            {
                if (currentDirectory.rfind('/') == 0)
                    currentDirectory = "/";
                else
                    currentDirectory.substr(0, currentDirectory.rfind('/'));
                return currentDirectory;
            }
            else if (SD_MMC.exists((currentDirectory + "/" + second).c_str()))
            {
                currentDirectory += "/" + second;
                return currentDirectory;
            }
            else
            {
                return first + ": " + second + ": No such directory\n" + currentDirectory;
            }
        }
        else if (first == "pwd")
        {
            return currentDirectory + "\n" + currentDirectory;
        }
        else if (first == "rm")
        {
            string second = input.substr(input.find(' ') + 1);
            if (SD_MMC.exists((currentDirectory + "/" + second).c_str()))
            {
                if (SD_MMC.remove((currentDirectory + "/" + second).c_str()))
                    return currentDirectory;
                else
                    return first + ": cannot remove " + second + ": SD error\n" + currentDirectory;
            }
            else
                return first + ": cannot remove " + second + ": No such file\n" + currentDirectory;
        }
        else if (first == "rmdir")
        {
            string second = input.substr(input.find(' ') + 1);
            if (SD_MMC.exists((currentDirectory + "/" + second).c_str()))
            {
                if (SD_MMC.rmdir((currentDirectory + "/" + second).c_str()))
                    return currentDirectory;
                else
                    return first + ": cannot remove " + second + ": SD error\n" + currentDirectory;
            }
            else
                return first + ": cannot remove " + second + ": No such directory\n" + currentDirectory;
        }
        else if (first == "lstlnk")
        {
            string returnData = "";
            for (auto& n : json.map)
                returnData += n.first + " " + n.second + "\n";
            return returnData + currentDirectory;
        }
        else if (first == "mklnk")
        {
            string second = "";
            if (input.find(' ') != string::npos)
                second = input.substr(input.find(' ') + 1);
            
            if (SD_MMC.exists((currentDirectory + "/" + second).c_str()))
            {
                const string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
                string randomLink = "";
                srand(millis());
                while (true)
                {
                    for (int i = 0; i < 8; i++)
                        randomLink += characters[rand() % characters.size()];
                    if (!json.exists(randomLink))
                        break;
                }
                if (second != "")
                    json.setValueStr(randomLink, currentDirectory + "/" + second);
                else
                    json.setValueStr(randomLink, currentDirectory);
                saveLinks();
                return currentDirectory;
            }
            else
                return first + ": cannot link " + currentDirectory + "/" + second + ": No such directory\n" + currentDirectory;
        }
        else if (first == "rmlnk")
        {
            string second = input.substr(input.find(' ') + 1);
            if (json.exists(second))
            {
                json.map.erase(second);
                saveLinks();
                return currentDirectory;
            }
            else
                return first + ": cannot remove " + second + ": No such link\n" + currentDirectory;
        }
        else if (first == "restart")
        {
            ESP.restart();
            return "";
        }
        else
        {
            return first + ": command not found\n" + currentDirectory;
        }
    }
    void loadLinks()
    {
        json.loadFromFile("/spiffs/links.json");
    }
    void saveLinks()
    {
        json.saveToFile("/spiffs/links.json");
    }
    void Start()
    {
        if (!SPIFFS.begin(false))
            ESP_LOGE("WebServer", "Failed to init SPIFFS!");

        if (!SPIFFS.exists("/restart"))
        {
            delay(50);
            SPIFFS.open("/restart", "w+", true).close();
            ESP.restart();
        }
        SPIFFS.remove("/restart");

        loadLinks();
        SD_MMC.setPins(9, 10, 8, 7, 18, 17);
        if (!SD_MMC.begin("/sd", false, false, 40000))
            ESP_LOGE("WebServer", "Failed to init SD card!");


        /*string data;
        data.resize(512);
        File f = SD_MMC.open("/server/testing/DSC02221.JPG");
        uint32_t t = millis();
        for (int i = 0; i < f.size(); i += data.size())
            f.read((uint8_t*)&data[0], data.size());
        float s = f.size() / 1000000.f, tim = (millis() - t) / 1000.f;
        ESP_LOGE("WebServer", "Read file of %.1f MB in %.1f sec => %.1f KB/s", s, tim, s * 1000 / tim);
        f.close();*/

        display->begin(SSD1306_SWITCHCAPVCC, 0x3C);

        display->clearDisplay();
        display->display();
        display->setTextSize(1);
        display->setTextColor(1);
        display->cp437(false);

        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(10);
            display->print(".");
            display->display();
        }

        server = make_unique<WebServer>(80);
        server->onNotFound(StorageServer::page_notFound);
        server->begin();

        xTaskCreatePinnedToCore(handleClients, "WebServer", 8192, NULL, 1, NULL, 0);

        Update();
    }
    void Update()
    {
        uint64_t screenRefresh = millis();
        uint64_t speedRefresh = millis();
        uint32_t offset = 0;
        uint32_t size = 0;
        uint32_t lastOffset = 0;
        uint32_t uploadSpeed = 0;
        while (1)
        {
            if (!done)
            {
                offset = processingFile->position();
                size = processingFile->size();
            }
            else
            {
                totalBytesStreamed += size;
                size = 0;
                offset = 0;
                lastOffset = 0;
            }
            char arr[256] = {0};
            
            if (usb_serial_jtag_read_bytes(arr, 256, 0))
            {
                string data(arr);
                string output = command(data);
                usb_serial_jtag_write_bytes(&output[0], output.size(), 0);
            }
            if (millis() - speedRefresh > 500)
            {
                uploadSpeed = (offset - lastOffset) * 2;
                lastOffset = offset;
                speedRefresh = millis();
            }
            if (millis() - screenRefresh > 50)
            {
                screenRefresh = millis();
                display->clearDisplay();
                display->setTextSize(1);
                display->setCursor(0, 0);
                if (done)
                    display->println(WiFi.localIP());
                else
                    display->printf("%ld KB/s", uploadSpeed / 1000);
                display->setCursor(103, 0);
                if (millis() >= 24 * 3600 * 1000)
                    display->printf("%ld d", millis() / 1000 / 3600 / 24);
                else if (millis() >= 3600 * 1000)
                    display->printf("%ld h", millis() / 1000 / 3600);
                else if (millis() >= 60 * 1000)
                    display->printf("%ld m", millis() / 1000 / 60);
                else
                    display->printf("%ld s", millis() / 1000);

                display->setTextSize(2);
                display->drawBitmap(0, 16, upload, 16, 16, 1);
                display->setCursor(20, 16);
                if (totalBytesStreamed + offset > 800000000)
                    display->printf("%.2f GB", (totalBytesStreamed + offset) / 1000000000.f);
                else if (totalBytesStreamed + offset > 800000)
                    display->printf("%.2f MB", (totalBytesStreamed + offset) / 1000000.f);
                else if (totalBytesStreamed + offset > 800)
                    display->printf("%.2f KB", (totalBytesStreamed + offset) / 1000.f);
                else
                    display->printf("%lld B", totalBytesStreamed + offset);
                display->display();
            }
            else
                delay(1);
        }
    }
};
void app()
{
    StorageServer s;
    s.Start();
}
extern "C"
{
    void app_main()
    {
        delay(50);
        nvs_flash_init();
        app();
    }
}
#ifndef FSHANDLE_H
#define FSHANDLE_H

#include "Arduino.h"

#define MAX_FILE_SIZE 4096

#include "FS.h"
#include "LittleFS.h"
#include "ArduinoJson.h"

#define FORMAT_LITTLEFS_IF_FAILED true

#include "configHandle.h"

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.print("- failed to open directory\r\n");
        return;
    }
    if(!root.isDirectory()){
        Serial.print(" - not a directory\r\n");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.printf("  DIR : ");
            Serial.printf("%c\r\n", file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.printf("  FILE: ");
            Serial.printf(file.name());
            Serial.printf("\tSIZE: ");
            Serial.printf("%s\r\n", file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.printf("Dir created\r\n");
    } else {
        Serial.printf("mkdir failed\r\n");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.printf("Dir removed\r\n");
    } else {
        Serial.printf("rmdir failed\r\n");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.print("- failed to open file for reading\r\n");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

String readFile(fs::FS &fs, String fileName)
{
    File file = fs.open(fileName);
    if(!file || file.isDirectory())
    {
        Serial.println("readFile -> failed to open file");
        return "";
    }
    String fileText = "";
    while (file.available())
    {
        fileText = file.readString();
    }
    file.close();
    return fileText;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.printf("- failed to open file for writing\r\n");
        return;
    }
    if(file.print(message)){
        Serial.printf("- file written\r\n");
    } else {
        Serial.printf("- write failed\r\n");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.printf("- failed to open file for appending\r\n");
        return;
    }
    if(file.print(message)){
        Serial.printf("- message appended\r\n");
    } else {
        Serial.printf("- append failed\r\n");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.printf("- file renamed\r\n");
    } else {
        Serial.printf("- rename failed\r\n");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.printf("- file deleted\r\n");
    } else {
        Serial.printf("- delete failed\r\n");
    }
}

// SPIFFS-like write and delete file, better use #define CONFIG_LITTLEFS_SPIFFS_COMPAT 1

void writeFile2(fs::FS &fs, const char * path, const char * message){
    if(!fs.exists(path)){
		if (strchr(path, '/')) {
            Serial.printf("Create missing folders of: %s\r\n", path);
			char *pathStr = strdup(path);
			if (pathStr) {
				char *ptr = strchr(pathStr, '/');
				while (ptr) {
					*ptr = 0;
					fs.mkdir(pathStr);
					*ptr = '/';
					ptr = strchr(ptr+1, '/');
				}
			}
			free(pathStr);
		}
    }

    Serial.printf("Writing file to: %s\r\n", path);
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.printf("- failed to open file for writing\r\n");
        return;
    }
    if(file.print(message)){
        Serial.printf("- file written\r\n");
    } else {
        Serial.printf("- write failed\r\n");
    }
    file.close();
}

void deleteFile2(fs::FS &fs, const char * path){
    Serial.printf("Deleting file and empty folders on path: %s\r\n", path);

    if(fs.remove(path)){
        Serial.printf("- file deleted\r\n");
    } else {
        Serial.printf("- delete failed\r\n");
    }

    char *pathStr = strdup(path);
    if (pathStr) {
        char *ptr = strrchr(pathStr, '/');
        if (ptr) {
            Serial.printf("Removing all empty folders on path: %s\r\n", path);
        }
        while (ptr) {
            *ptr = 0;
            fs.rmdir(pathStr);
            ptr = strrchr(pathStr, '/');
        }
        free(pathStr);
    }
}

void testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.printf("- failed to open file for writing\r\n");
        return;
    }

    size_t i;
    Serial.printf("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.printf("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              Serial.print(".");
            }
            len -= toRead;
        }
        Serial.printf("\r\n");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.printf("- failed to open file for reading\r\n");
    }
}

#endif
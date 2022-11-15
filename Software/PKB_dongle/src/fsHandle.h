#include "Arduino.h"

#include "FS.h"
#include "LittleFS.h"

#define FORMAT_LITTLEFS_IF_FAILED true


void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    USBSerial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        USBSerial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        USBSerial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            USBSerial.print("  DIR : ");

#ifdef CONFIG_LITTLEFS_FOR_IDF_3_2
            USBSerial.println(file.name());
#else
            USBSerial.print(file.name());
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            USBSerial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
#endif

            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            USBSerial.print("  FILE: ");
            USBSerial.print(file.name());
            USBSerial.print("  SIZE: ");

#ifdef CONFIG_LITTLEFS_FOR_IDF_3_2
            USBSerial.println(file.size());
#else
            USBSerial.print(file.size());
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            USBSerial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
#endif
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    USBSerial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        USBSerial.println("Dir created");
    } else {
        USBSerial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    USBSerial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        USBSerial.println("Dir removed");
    } else {
        USBSerial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    USBSerial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        USBSerial.println("- failed to open file for reading");
        return;
    }

    USBSerial.println("- read from file:");
    while(file.available()){
        USBSerial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    USBSerial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        USBSerial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        USBSerial.println("- file written");
    } else {
        USBSerial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    USBSerial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        USBSerial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        USBSerial.println("- message appended");
    } else {
        USBSerial.println("- append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    USBSerial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        USBSerial.println("- file renamed");
    } else {
        USBSerial.println("- rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    USBSerial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        USBSerial.println("- file deleted");
    } else {
        USBSerial.println("- delete failed");
    }
}

// SPIFFS-like write and delete file, better use #define CONFIG_LITTLEFS_SPIFFS_COMPAT 1

void writeFile2(fs::FS &fs, const char * path, const char * message){
    if(!fs.exists(path)){
		if (strchr(path, '/')) {
            USBSerial.printf("Create missing folders of: %s\r\n", path);
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

    USBSerial.printf("Writing file to: %s\r\n", path);
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        USBSerial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        USBSerial.println("- file written");
    } else {
        USBSerial.println("- write failed");
    }
    file.close();
}

void deleteFile2(fs::FS &fs, const char * path){
    USBSerial.printf("Deleting file and empty folders on path: %s\r\n", path);

    if(fs.remove(path)){
        USBSerial.println("- file deleted");
    } else {
        USBSerial.println("- delete failed");
    }

    char *pathStr = strdup(path);
    if (pathStr) {
        char *ptr = strrchr(pathStr, '/');
        if (ptr) {
            USBSerial.printf("Removing all empty folders on path: %s\r\n", path);
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
    USBSerial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        USBSerial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    USBSerial.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          USBSerial.print(".");
        }
        file.write(buf, 512);
    }
    USBSerial.println("");
    uint32_t end = millis() - start;
    USBSerial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        USBSerial.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              USBSerial.print(".");
            }
            len -= toRead;
        }
        USBSerial.println("");
        end = millis() - start;
        USBSerial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        USBSerial.println("- failed to open file for reading");
    }
}

#ifndef FSHANDLE_H
#define FSHANDLE_H

#include "Arduino.h"

#define MAX_FILE_SIZE 4096

#define DISK_LABEL "PMK Dongle"

#include "FS.h"
#include "SdFat.h"
#include "ArduinoJson.h"

#include "diskio.h"
#include "ff.h"

#include "configHandle.h"



// converts character array
// to string and returns it
String convertToString(char* a, int size)
{
    int i;
    String s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}



void format_fat12(void)
{
  // Working buffer for f_mkfs.
  uint8_t workbuf[4096];

  // Elm Cham's fatfs objects
  FATFS elmchamFatfs;

  // Make filesystem.
  FRESULT fresult = f_mkfs("", FM_FAT, 0, workbuf, sizeof(workbuf));
  if(fresult != FR_OK)
  {
    Serial.print(F("Error, f_mkfs failed with error code: ")); 
    Serial.println(fresult, DEC);
    while(1) yield();
  }

  // mount to set disk label
  fresult = f_mount(&elmchamFatfs, "0:", 1);
  if(fresult != FR_OK)
  {
    Serial.print(F("Error, f_mount failed with error code: ")); 
    Serial.println(fresult, DEC);
    while(1) yield();
  }

  // Setting label
  Serial.printf("Setting disk label to: %c\r\n", DISK_LABEL);
  fresult = f_setlabel(DISK_LABEL);  // TODO to fix...
  if(fresult != FR_OK)
  {
    Serial.print(F("Error, f_setlabel failed with error code: ")); 
    Serial.println(fresult, DEC);
    while(1) yield();
  }

  // unmount
  f_unmount("0:");

  // sync to make sure all data is written to flash
  flash.syncBlocks();

  Serial.println(F("Formatted flash!"));
}



void check_fat12(void)
{
  // Check new filesystem
  if (!fatfs.begin(&flash)) {
    Serial.println(F("Error, failed to mount newly formatted filesystem!"));
    while(1) delay(1);
  }
}



void listDir(FatVolume &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File32 root = fs.open(dirname, O_READ);
    if(!root){
        Serial.print("- failed to open directory\r\n");
        return;
    }
    if(!root.isDirectory()){
        Serial.print(" - not a directory\r\n");
        return;
    }

    File32 file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.printf("  DIR : ");
            Serial.printf("%c\r\n", file.name());
            if(levels){
                //listDir(fs, file.path(), levels -1);
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



void createDir(FatVolume &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.printf("Dir created\r\n");
    } else {
        Serial.printf("mkdir failed\r\n");
    }
}



void removeDir(FatVolume &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.printf("Dir removed\r\n");
    } else {
        Serial.printf("rmdir failed\r\n");
    }
}



void readFile(FatVolume &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File32 file = fs.open(path);
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



String readFile(FatVolume &fs, String fileName)
{
    File32 file = fs.open(fileName);
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



void writeFile(FatVolume &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File32 file = fs.open(path, O_WRITE);
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



void appendFile(FatVolume &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File32 file = fs.open(path, O_WRITE);
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



void renameFile(FatVolume &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.printf("- file renamed\r\n");
    } else {
        Serial.printf("- rename failed\r\n");
    }
}



void deleteFile(FatVolume &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.printf("- file deleted\r\n");
    } else {
        Serial.printf("- delete failed\r\n");
    }
}



// SPIFFS-like write and delete file, better use #define CONFIG_LITTLEFS_SPIFFS_COMPAT 1

void writeFile2(FatVolume &fs, const char * path, const char * message){
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
    File32 file = fs.open(path, O_WRITE);
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



void deleteFile2(FatVolume &fs, const char * path){
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



void testFileIO(FatVolume &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File32 file = fs.open(path, O_WRITE);
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



//--------------------------------------------------------------------+
// fatfs diskio
//--------------------------------------------------------------------+

DSTATUS disk_status ( BYTE pdrv )
{
  (void) pdrv;
	return 0;
}

DSTATUS disk_initialize ( BYTE pdrv )
{
  (void) pdrv;
	return 0;
}

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
  (void) pdrv;
	return flash.readBlocks(sector, buff, count) ? RES_OK : RES_ERROR;
}

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
  (void) pdrv;
  return flash.writeBlocks(sector, buff, count) ? RES_OK : RES_ERROR;
}

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
  (void) pdrv;

  switch ( cmd )
  {
    case CTRL_SYNC:
      flash.syncBlocks();
      return RES_OK;

    case GET_SECTOR_COUNT:
      *((DWORD*) buff) = flash.size()/512;
      return RES_OK;

    case GET_SECTOR_SIZE:
      *((WORD*) buff) = 512;
      return RES_OK;

    case GET_BLOCK_SIZE:
      *((DWORD*) buff) = 8;    // erase block size in units of sector size
      return RES_OK;

    default:
      return RES_PARERR;
  }
}



#endif
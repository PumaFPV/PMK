#ifndef MSCHANDLE_H
#define MSCHANDLE_H

#include "Arduino.h"

//bool msc_ready_callback(void);
//void msc_flush_cb (void);
//int32_t msc_write_cba (uint32_t lba, uint8_t* buffer, uint32_t bufsize);
//int32_t msc_read_cba (uint32_t lba, void* buffer, uint32_t bufsize);
//void refreshMassStorage(void);
//void setupMassStorage(void);

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and 
// return number of copied bytes (must be multiple of block size) 
int32_t msc_read_cba (uint32_t lba, void* buffer, uint32_t bufsize)
{
  // Note: SPIFLash Bock API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.readBlocks(lba, (uint8_t*) buffer, bufsize/512) ? bufsize : -1;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and 
// return number of written bytes (must be multiple of block size)
int32_t msc_write_cba (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
{
#ifdef LED_BUILTIN
  digitalWrite(LED_BUILTIN, HIGH);
#endif

  // Note: SPIFLash Bock API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.writeBlocks(lba, buffer, bufsize/512) ? bufsize : -1;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_cba (void)
{
  // sync with flash
  flash.syncBlocks();

  // clear file system's cache to force refresh
  fatfs.cacheClear();

#ifdef LED_BUILTIN
  digitalWrite(LED_BUILTIN, LOW);
#endif
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool msc_ready_callbacka(void)
{
  // if fs has changed, mark unit as not ready temporarily to force PC to flush cache
  bool ret = !fs_changed;
  fs_changed = false;
  return ret;
}

void refreshMassStorage(void)
{
  fs_changed = true;
}

#endif
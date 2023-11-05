#include <Adafruit_SI5351.h>
#include <asserts.h>
#include <errors.h>

#include <SD.h>
#include "OPL3.h"
#include "Wire.h"


#include <Audio.h>
#include <SPI.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S2           i2s2_1;         //xy=489,211
AudioOutputUSB           usb1;           //xy=721.0056381225586,185.00566291809082
AudioConnection          patchCord1(i2s2_1, 0, usb1, 0);
AudioConnection          patchCord2(i2s2_1, 1, usb1, 1);
// GUItool: end automatically generated code


File vgmFile;
OPL3* opl3;

uint32_t loop_start = 0;

uint8_t* vgm_buf = NULL;
uint32_t volatile buf_pos = 0;


uint8_t volatile cmd,val,addr;
uint32_t volatile wait_time = 0;

IntervalTimer cmdTimer;
Adafruit_SI5351 clockgen = Adafruit_SI5351();

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(115200);
  
  
  // Start the clock generator
  
  clockgen.begin(&Wire2);
  // 14.318MHz
  clockgen.setupPLL(SI5351_PLL_A, 35, 46909, 50000);
  clockgen.setupMultisynth(0, SI5351_PLL_A, 62, 300, 400);
  clockgen.setupMultisynth(1, SI5351_PLL_A, 564, 300, 400);
  
  /* // 33.8688MHz
  clockgen.setupPLL(SI5351_PLL_A, 35, 14077, 15625);
  clockgen.setupMultisynth(0, SI5351_PLL_A, 26, 400, 800);
  */
  
  clockgen.enableOutputs(true);


  opl3 = new OPL3();
  opl3->reset();

  //while(!Serial.available());

  AudioMemory(30);
  
  //SD.begin(BUILTIN_SDCARD);
  SD.sdfs.begin(SdioConfig(FIFO_SDIO));
  

  // Initialize the OPL3


  

}

const char *tracks[] = {"02 Trip Through the Grand Canyon (Extended).vgm", "02 Wondering About My Loved Ones.vgm", "03 OPL3 Demo 003.vgm", "04 OPL3 Demo 004.vgm"};
int volatile playing = 0;
int num_tracks = 1;
int track = 1;


void vgmstep(void) {
  if(wait_time > 0) {
    wait_time--;
    return;          
  }
  cmd = vgm_buf[buf_pos++];
  switch(cmd) {
    case 0x50:
      val = vgm_buf[buf_pos++];
      wait_time = 1;
      break;
    case 0x5A:
    case 0x5E:
    case 0x5F:
      addr = vgm_buf[buf_pos++];
      val = vgm_buf[buf_pos++];
      opl3->writereg(addr, val, cmd & 0x1);
      wait_time = 1;
      break;
    case 0x61:
      wait_time = vgm_buf[buf_pos++];
      wait_time += vgm_buf[buf_pos++] << 8;
      break;
    case 0x62:
      wait_time = 735;
      break;
    case 0x63:
      wait_time = 882;
      break;
    case 0x66:
      buf_pos = loop_start;
      //playing = 0;
      break;
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x7D:
    case 0x7E:
    case 0x7F:
      wait_time = (cmd - 0x70 + 1);
      break;
  }
}

uint32_t vgm_offset(char* header) {
  // Get the VGM Offset
  uint32_t rel_offset = *(uint32_t*)(header+0x34);
  uint32_t abs_offset = rel_offset + 0x34;
  return abs_offset;
}

void loop() {
  if(playing == 0) {
    cmdTimer.end();
    char header[256];
    
    vgmFile.close();
    vgmFile = SD.open(tracks[track]);
    track = (track + 1) % (num_tracks);

    vgmFile.readBytes(header, 256);
    buf_pos = vgm_offset(header);
    
    loop_start = *(uint32_t*)(header+0x1C) + 0x1C;

    uint32_t file_size = vgmFile.size();
    delete[] vgm_buf;
    vgm_buf = new uint8_t[file_size];
    vgmFile.seek(0);
    vgmFile.read(vgm_buf,file_size);

    opl3->reset();
    cmdTimer.begin(vgmstep,22.6757);
    
    playing = 1;
  }
  /*
  si5351.update_status();
  Serial.print("SYS_INIT: ");
  Serial.print(si5351.dev_status.SYS_INIT);
  Serial.print("  LOL_A: ");
  Serial.print(si5351.dev_status.LOL_A);
  Serial.print("  LOL_B: ");
  Serial.print(si5351.dev_status.LOL_B);
  Serial.print("  LOS: ");
  Serial.print(si5351.dev_status.LOS);
  Serial.print("  REVID: ");
  Serial.println(si5351.dev_status.REVID);
  */
}

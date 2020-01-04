//-----------------------------------------------------------------
// Reset the display
//
// Reset the 3 IS31FL3730 chips then set them back in default mode
// 0xff is the chip Reset register
// 0x00 is the chip Configuration register:
//      Default mode (0x18) = 0b00011000,sets display mode to use
//      two matrix displays and sets intensity current to be controlled 
//      by the Lighting Effect Register. Each matrix display is 8x8.
//
// Chips have addresses 0x61, 9x62 and 0x63 on the bus. 
//
// WR
//-----------------------------------------------------------------

void reset(int fd) {
        uint8_t buf1[2]={0xff,0x00}; //Reset
        uint8_t buf2[2]={0x00,0x18}; //Default mode
        uint8_t k,chip_addr=0x61;
        if(fd<0) return;
        //Reset each of the 3 IS31FL3730 chips and put them in default mode
        for(k=0;k<3;k++) {
                ioctl(fd, I2C_SLAVE,chip_addr+k);
                write(fd, buf1,2);
                write(fd, buf2,2);
        }
}

//-------------------------------------------------------------------------
// bright - Change the light intensity for the display
// 0x19 is the pulse width modulation register in the IS31FL3730 chip.
// There are 3 chips on board, 0x61, 0x62 and 0x63
// Valid values for PWM are not obvious, but through trial and error, 0-7
// seem to work. Anything after 8 seems to make no difference.
//-------------------------------------------------------------------------
void bright(int fd, int val) {
        uint8_t buf[2],k,chip_addr=0x61;
        if(fd<0) return;
        buf[0]=0x19;    //Brightness PWM reg
        buf[1]=val;     //Brightness value
        for(k=0;k<3;k++) {
                ioctl(fd, I2C_SLAVE,chip_addr+k);
                write(fd,buf,2);
        }
}

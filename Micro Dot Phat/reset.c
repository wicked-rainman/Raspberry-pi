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

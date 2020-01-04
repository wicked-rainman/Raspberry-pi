void bright(int fd, int val) {
        uint8_t buf[2],k,chip_addr=0x61;
        if(fd<0) return;
        buf[0]=0x19;    //Brightness
        buf[1]=val;     //Brightness value
        for(k=0;k<3;k++) {
                ioctl(fd, I2C_SLAVE,chip_addr+k);
                write(fd,buf,2);
        }
}

void update(int fd) {
        uint8_t buf[2]={CMD_UPDATE,0};
        uint8_t k;
        uint8_t chip_addr=0x61;
        if(fd<0) return;
        for(k=0;k<3;k++) {
                ioctl(fd, I2C_SLAVE,chip_addr+k);
                write(fd, buf, 2);
        }
}

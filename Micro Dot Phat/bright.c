//-------------------------------------------------------------------------
// The 0x0d lighting effect register in the IS31FL3730 chip defines the
// amout of current delivered to each display row. Values are:
//
// 0000 (0) = 40ma
// 0001 (1) = 45ma
// 0010 (2) = 50ma
// 0011 (3) = 55ma
// 0100 (4) = 60ma
// 0101 (5) = 65ma
// 0110 (6) = 70ma
// 0111 (7) = 75ma
//
// Therefore values between 0 and 7 can be used
//-------------------------------------------------------------------------
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

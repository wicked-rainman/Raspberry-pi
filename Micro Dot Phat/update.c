//---------------------------------------------------------------
// update.c - Update the data registers on the three IS31FL3730
// chips using the Update Column register. Null operand.
// 
// WR
//-------------------------------------------------------------

void update(int fd) {
        uint8_t buf[2]={0x0c,0};
        uint8_t k;
        uint8_t chip_addr=0x61;
        if(fd<0) return;
        for(k=0;k<3;k++) {
                ioctl(fd, I2C_SLAVE,chip_addr+k);
                write(fd, buf, 2);
        }
}

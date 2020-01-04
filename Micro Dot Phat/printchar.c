void printchar(int fd, int x, int chr) {
        int i, y, z;
        char buf[7], reg;
        uint8_t cmd[2];
        uint8_t matrix;
        if(fd<0) return;
        reg = x % 2 ? CMD_MATRIX2 : CMD_MATRIX1;
        matrix=0x61+x/2;
        printf("Matrix=%d - %d\n",matrix,x);
        ioctl(fd, I2C_SLAVE,matrix);

        switch (chr) {
        case '\0':
        case '\n':
        case '\r':
                return;
        }
        printf("Printing %c (%d - 33) = %d\n",chr, chr,(chr-33));
        chr=chr-33;
        if (x % 2) {
                // Odd numbered character out of the string
                /* flip */
                for (i = 0, y = 7; i < 7; i++, y--)
                        buf[i] = 0;

                for (i = 0; i < 8; i++)
                        for (y = 0; y < 7; y++)
                                buf[i] |= font[chr][y] & (1 << i) ? 1 << y : 0;

                for (i = 0; i < 7; i++) {
                        cmd[0]=reg+i;
                        cmd[1]=buf[i];
                        write(fd, cmd, 2);
                }
        }
        else {
                // Even numbered character out of the string
                for (i = 0; i < 7; i++) {
                        cmd[0]=reg+i;
                        cmd[1]=font[chr][i];
                        write(fd, cmd, 2);
                }
        }
}

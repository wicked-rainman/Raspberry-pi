void printstr(int fd, char *str) {
        int x = 0;
        char *p = str;
        if(fd<0) return;
        while (*p && x<6) printchar(fd, x++, *p++);
}

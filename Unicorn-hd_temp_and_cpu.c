#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <wiringPiSPI.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SPI_CHANNEL 0
#define SPI_SPEED   9000000

#define PACKET_SIZE (1 + ( 16 * 16 * 3 ))

unsigned char buffer[PACKET_SIZE];
char map[10][49]={
        "..###...#...#..#...#..#...#..#...#..#...#...###..",
        "...#.....##....#.#......#......#......#....#####.",
        "..###...#...#..#...#.....#.....#.....#.....#####.",
        "..###...#...#......#...###.......#..#...#...###..",
        ".#...#..#...#..#...#..#####......#......#......#.",
        ".#####..#......#......#####......#......#..#####.",
        "..###...#...#..#......####...#...#..#...#...###..",
        ".#####......#.....#.....#.....#......#......#....",
        "..###...#...#..#...#...###...#...#..#...#...###..",
        "..###...#...#..#...#...###......#.....#....#....."
};

int cputemp() {
        FILE *cmd;
        char FileBuffer[100]={0x0};
        int temp;
        cmd = popen("vcgencmd measure_temp","r" );
        while (fgets(FileBuffer, sizeof(FileBuffer), cmd) !=NULL);
        sscanf(FileBuffer, "temp=%d.", &temp);
        pclose(cmd);
        return (temp);
}

int cpuload() {
        int filep;
        char buffer[1024];
        float load;
        filep = open("/proc/loadavg", O_RDONLY);
        if(filep < 0) return -1;
        read(filep, buffer, sizeof(buffer) - 1);
        sscanf(buffer, "%f", &load);
        close(filep);
        return (int)(load * 100);
}
//----------
void set_pixel(int x, int y, int r, int g, int b) {
    int offset = 1 + ((y * 16) + x) * 3;
    buffer[offset + 0] = r;         // led red value
    buffer[offset + 1] = g;         // led green value
    buffer[offset + 2] = b;        // led blue value
}


int main( int argc, char *argv[] )  {
size_t x;
int temp,most_temp,least_temp,pixel,load,most_cpu,least_cpu,offset,k,l;
int red_load,green_load,blue_load,red_temp,green_temp,blue_temp;
    if( wiringPiSPISetup( SPI_CHANNEL, SPI_SPEED ) == -1 ) {
        printf("Could not initialise SPI\n");
        return( 1 );
    }
    while (1) {
        for (size_t i = 1; i < 770; i++) buffer[i] = 0;
        buffer[0] = 0x72; // SOF byte
        load=cpuload();
        most_cpu=load/10;
        least_cpu=load%10;
        temp=cputemp();
        most_temp=temp/10;
        least_temp=temp%10;
        if(most_temp <=2) {
                red_temp=0;
                green_temp=0;
                blue_temp=20;
        }
        else if(most_temp <=4) {
                red_temp=0;
                green_temp=20;
                blue_temp=0;
        }
        else {
                red_temp=20;
                green_temp=20;
                blue_temp=0;
        }
        if(most_cpu <1) {
                red_load=0;
                green_load=0;
                blue_load=20;
        }
        else {
                red_load=20;
                green_load=20;
                blue_load=0;
        }
        for(k=0;k<7;k++) {
                //Top busy digits
                for(l=0;l<7;l++) {
                        if(map[most_cpu][(k*7)+l] =='#')  set_pixel(l,k,red_load,green_load,blue_load);
                        if(map[least_cpu][(k*7)+l] =='#')  set_pixel(l+7,k,red_load,green_load,blue_load);
                }
        }
        for(k=0;k<7;k++) {
                //Bottom temp digits
                for(l=0;l<7;l++) {
                        if(map[most_temp][(k*7)+l] =='#')  set_pixel(l,k+8,red_temp,green_temp,blue_temp);
                        if(map[least_temp][(k*7)+l] =='#')  set_pixel(l+7,k+8,red_temp,green_temp,blue_temp);
                }
        }
        wiringPiSPIDataRW( SPI_CHANNEL, buffer, PACKET_SIZE );
        usleep(1000000);
        }
}

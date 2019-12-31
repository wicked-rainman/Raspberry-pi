#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
static int wanted_td(char *);
static int strpos(char *, char *);
static int connect_ip_socket(char *, int);
static void extract_values(char *);
static char AbsPress[10];
static char RelPress[10];
static char outTemp[10];
static char outHumi[10];
static char windir[10];
static char avgwind[10];
static char gustspeed[10];
static char solarrad[10];
static char rainofdaily[10];
static char inTemp[10];
static char inHumi[10];

//---------------------------------------------------------------------
// Program: weather
//
// The Aercus 433Mhz weather station unit seems to run some software known
// as Weather logger (V 2.2).
// It's connected to my local area network, and pushes data to Weather
// Underground. I wanted to be able to access the weather values locally,
// so wrote this bit of code to talk to the Aercus on port 80.
// Eventually (when the parts turn up), rather than printing the interesting
// values out, I'll write them to a dot matrix led panel connected to
// a raspberry pi. That way, I'll have an indors weather display!
//
// Internally, my Aercus has an IP of 192.168.0.23. If you want to chance
// your arm and use this code, you'll have to change this value as required
//
// WR
//---------------------------------------------------------------------------
int main() {
int sockfd=0;
int bytes_returned;
int start_pos, end_pos,start_found=0;
char input_line[9000];
char temp_line[9000];
char reply[2048];
char SERVER_IP[] = "192.168.0.23";
int PORT=80;
char GET_REQUEST[]="GET /livedata.htm HTTP/1.1\r\nHost: 192.168.0.23\r\nUser-Agent: curl/7.53.1\r\nAccept: */*\r\nConnection: close\r\n\r\n";
        sockfd = connect_ip_socket(SERVER_IP, PORT);
        (void) send(sockfd,GET_REQUEST,strlen(GET_REQUEST),0);
        bytes_returned = (int) read(sockfd, reply, 2048);
        input_line[0]=(char) 0x0;
        while(bytes_returned>0) {
                strncat(input_line,reply,(size_t)bytes_returned);
                if(start_found == 0) {
                        start_pos=wanted_td(input_line);
                        if(start_pos>=0) {
                                start_found=1;
                                strcpy(temp_line, &input_line[start_pos]);
                                strcpy(input_line,temp_line);
                        }
                }
                if(start_found == 1) {
                        end_pos=strpos("maxlength",input_line);
                        if(end_pos>0) {
                                strcpy(temp_line,&input_line[end_pos]);
                                input_line[end_pos]=(char) 0x0;
                                extract_values(input_line);
                                strcpy(input_line,temp_line);
                                start_pos=0;
                                end_pos=0;
                                start_found=0;
                        }
                }
                bytes_returned = (int) read(sockfd,reply,2048);
        }
        (void) close(sockfd);
        printf("\n\nInside temp=%sc, outside temp=%sc, inside humidity=%s%, outside humidity=%s%\n", inTemp,outTemp, inHumi, outHumi);
        printf("Wind direction=%s, average wind speed=%smph, wind gusts=%smph\n",windir, avgwind, gustspeed);
        printf("Barometric pressure=%smb, Total rain=%smm, radiation level=%slux\n",RelPress, rainofdaily, solarrad);
        return (EXIT_SUCCESS);
}
//---------------------------------------------------
// wanted_td - Is this cell in the table row wanted?
//---------------------------------------------------
int wanted_td(char *input_line) {
char wanted_fields[11][13] = { "inTemp", "inHumi", "AbsPress", "RelPress", "outTemp", "outHumi", "windir", "avgwind", "gustspeed", "solarrad", "rainofdaily\""};
int k,x;
        for(k=0;k<11;k++) {
                x=strpos(wanted_fields[k],input_line);
                if(x >0 ) return x;
        }
        return -1;
}
//----------------------------------------------------
// strpos
//----------------------------------------------------
int strpos(char *needle, char *haystack) {
char *offset;
        offset=strstr(haystack,needle);
        if(offset !=NULL) {
                return offset-haystack;
        }
        return -1;
}
//---------------------------------------------------
// Extract_values
//---------------------------------------------------
void extract_values(char *input_line) {
char fieldname[20];
char field_value[20];
int end_of_field=0;
        end_of_field=strpos("\"",input_line);
        if(end_of_field>=0)  {
                strncpy(fieldname,input_line,(size_t) end_of_field);
                fieldname[end_of_field]=(char) 0x0;
        }
        else  strcpy(fieldname,"Unknown");
        end_of_field=strpos("value=\"",input_line);
        strcpy(field_value,&input_line[end_of_field+7]);
        field_value[strlen(field_value)-2]=(char) 0x0;
        if(strcmp(fieldname,"AbsPress")==0) strcpy(AbsPress,field_value);
        else if(strcmp(fieldname,"RelPress")==0) strcpy(RelPress,field_value);
        else if(strcmp(fieldname,"outTemp")==0) strcpy(outTemp,field_value);
        else if(strcmp(fieldname,"outHumi")==0) strcpy(outHumi,field_value);
        else if(strcmp(fieldname,"windir")==0) strcpy(windir, field_value);
        else if(strcmp(fieldname,"avgwind")==0) strcpy(avgwind, field_value);
        else if(strcmp(fieldname,"gustspeed")==0) strcpy(gustspeed, field_value);
        else if(strcmp(fieldname,"solarrad")==0) strcpy(solarrad, field_value);
        else if(strcmp(fieldname,"rainofdaily")==0) strcpy(rainofdaily, field_value);
        else if(strcmp(fieldname,"inTemp")==0) strcpy(inTemp, field_value);
        else if(strcmp(fieldname,"inHumi")==0) strcpy(inHumi, field_value);
        else {
                printf("Unrecognised values found (%s)\n",fieldname);
                exit(EXIT_FAILURE);
        }
}
//-----------------------------------------------------------------
// connect_ip_socket
// Connect to a port on the given IP address (no DNS lookups)  
//-----------------------------------------------------------------

int connect_ip_socket(char *host_ip, int port) {
        struct sockaddr_in server_address;
        int sockfd;
                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if(sockfd<0) {
                        fprintf(stderr,"\nCould not create new socket!\n");
                        exit(EXIT_FAILURE) ;
                }
                server_address.sin_family = AF_INET;
                server_address.sin_port = htons((uint16_t)port);
                if(inet_pton(AF_INET,host_ip,&server_address.sin_addr)<=0) {
                        fprintf(stderr, "\nCould not convert %s to binary\n",host_ip);
                        exit(EXIT_FAILURE) ;
                }
                if(connect(sockfd,(struct sockaddr *) &server_address,(socklen_t) sizeof(server_address))<0) {
                        fprintf(stderr,"\nCould not connect to %s:%d",host_ip,port);
                        exit(EXIT_FAILURE) ;
                }
                return (sockfd);
}


#include "uart.h"
extern void put16(unsigned int,unsigned int);
extern void BRANCHTO(unsigned int);
//------------------------------------------------------------------------
void hexstrings ( unsigned int d )
{
    //unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    rb=32;
    while(1)
    {
        rb-=4;
        rc=(d>>rb)&0xF;
        if(rc>9) rc+=0x37; else rc+=0x30;
        uart_send(rc);
        if(rb==0) break;
    }
    uart_send(0x20);
}
//------------------------------------------------------------------------
void hexstring ( unsigned int d )
{
    hexstrings(d);
    uart_send(0x0D);
    uart_send(0x0A);
}

unsigned char xstring[512];
int recv_packet ( void )
{
    unsigned int len;
    unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    while(1)
    {
        ra=uart_recv();
        if(ra!=0x7C)
        {
            continue;
        }
        ra=0;
        xstring[ra++]=0x7C; //sync
        xstring[ra++]=uart_recv(); //datalen
        xstring[ra++]=uart_recv(); //~datalen
        if(xstring[ra-2]!=(~xstring[ra-1]&0xFF))
        {
            continue;
        }
        len=xstring[ra-2];
        while(len--) xstring[ra++]=uart_recv();
        xstring[ra++]=uart_recv(); //sync
        xstring[ra++]=uart_recv(); //checksum
        if(xstring[ra-2]!=0x7D)
        {
            continue;
        }
        for(rb=0,rc=0x00;rb<ra;rb++) rc+=xstring[rb];
        rc&=0xFF;
        if(rc==0xFF) break;
    }
    return(0);
}
//------------------------------------------------------------------------
int send_result ( unsigned char res )
{
    unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    ra=0;
    xstring[ra++]=0x7C; //xstring[ra++]=0x7C; //sync
    xstring[ra++]=0x05; //xstring[ra++]=uart_recv(); //datalen
    xstring[ra++]=~0x05;//xstring[ra++]=uart_recv(); //~datalen
    //datalen:
    ra++; //same as input
    ra++; //same as input
    xstring[ra++]=res;
    xstring[ra++]=0;
    xstring[ra++]=0;
    //datalen
    xstring[ra++]=0x7D;
    for(rb=0,rc=0;rb<ra;rb++) rc+=xstring[rb];
    xstring[ra++]=(~rc)&0xFF;
    for(rb=0;rb<ra;rb++) uart_send(xstring[rb]);
    return(0);
}
void kernel_main(void)
{
    unsigned int ra;
    unsigned int len;
    unsigned int addr;
    unsigned int data;
	uart_init(115200);
	uart_send_string("Hello, RaspPi!\r\n");
	
	hexstring(0x12345678);

	while (1) {
	     if(recv_packet()) break;
	     switch(xstring[3])
        {
            case 0x07: //write to ram
            {
                //validate len
                len=xstring[1];
                if(len<(5+4+2))
                {
                    send_result(1);
                    break;
                }
                if((len-5-4-2)&1)
                {
                    send_result(1);
                    break;
                }
                //[5,6,7,8,9] packet header
                //[8,9,10,11] address
                //[12,13] first halfword
                //[14,15] second halfword
                ra=8;
                addr=0;
                addr<<=8; addr|=xstring[ra++];
                addr<<=8; addr|=xstring[ra++];
                addr<<=8; addr|=xstring[ra++];
                addr<<=8; addr|=xstring[ra++];
                len-=5+4;
                while(len)
                {
                    data=0;
                    data<<=8; data|=xstring[ra++];
                    data<<=8; data|=xstring[ra++];
                    put16(addr,data);
                    addr+=2;
                    len-=2;
                }
                send_result(0);
                break;
            }
            case 0x08: //branch to address
            {
                //validate len
                len=xstring[1];
                if(len!=(5+4))
                {
                    send_result(1);
                    break;
                }
                //[5,6,7,8,9] packet header
                //[8,9,10,11] address
                ra=8;
                addr=0;
                addr<<=8; addr|=xstring[ra++];
                addr<<=8; addr|=xstring[ra++];
                addr<<=8; addr|=xstring[ra++];
                addr<<=8; addr|=xstring[ra++];
                send_result(0);
                BRANCHTO(addr);
                break;
            }
        }
		
	}
	return ;
}

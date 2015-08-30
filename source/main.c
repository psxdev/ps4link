/*
 * PlayStation 4 ps4link basic sample  
 */
#include "ps4.h"
#include "ps4link.h"
#include "debugnet.h"



int _main(void) {
	// Init and resolve libraries
	initLibc();
	initPthread();
	initNetwork();
	initCamera();
	int ret;
	
	
	ret=ps4LinkInit("192.168.1.3",0x4711,0x4712,0x4712);
	if(!ret)
	{
		ps4LinkFinish();
		return ret;
	}
	while(!ps4LinkRequestsIsConnected())
	{
		
	}
	DEBUG("ps4link initialized and connected from pc/mac\n");
	
	
	int fd=ps4LinkOpen("host0:/usr/local/ps4dev/test.txt",O_RDONLY,0);
	if(fd<0)
	{
		DEBUG("ps4LinkOpen returned error %d\n",fd);
		ps4LinkFinish();
	}
	int filesize=ps4LinkLseek(fd,0,SEEK_END);
	if(filesize<0)
	{
		DEBUG("ps4LinkSeek returned error %d\n",fd);
		ps4LinkClose(fd);
		ps4LinkFinish();
	}
	ps4LinkLseek(fd,0,SEEK_SET);
	char buf[100];
	int numread=ps4LinkRead(fd,buf,filesize);
	if(numread!=filesize)
	{
		DEBUG("ps4LinkRead returned error %d\n",numread);
		ps4LinkClose(fd);
		ps4LinkFinish();
		
	}
	DEBUG("Content of file: %s",buf);
	ps4LinkClose(fd);
	
	fd=ps4LinkOpen("host0:/usr/local/ps4dev/test1.txt",O_CREAT|O_WRONLY,0);
	if(fd<0)
	{
		DEBUG("ps4LinkOpen returned error %d\n",fd);
		ps4LinkFinish();
	}
	int numwrites=ps4LinkWrite(fd,"This is a write test",sizeof("This is a write test"));
	DEBUG("ps4LinkWrite wrote %d bytes\n",numwrites);
	ps4LinkClose(fd);
	
	fd=ps4LinkDopen("host0:/usr/local/ps4dev");
	if(fd<0)
	{
		DEBUG("ps4LinkDopen returned error %d\n",fd);
		ps4LinkFinish();
	}
	struct dirent dir;
	DEBUG("List entries\n");
	while(ps4LinkDread(fd,&dir))
	{
		
		if(dir.d_type==DT_DIR)
		{
			DEBUG("%s [DIR]\n",dir.d_name);
			
		}
		if(dir.d_type==DT_REG)
		{
			DEBUG("%s [FILE]\n",dir.d_name);
			
		}
		if(dir.d_type==DT_LNK)
		{
			DEBUG("%s [LINK]\n",dir.d_name);
			
		}
		
		
		
	}
	ps4LinkDclose(fd);

	ps4LinkFinish();


	// Return to browser
	return 0;
}

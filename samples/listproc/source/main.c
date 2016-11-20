/*
 *	listproc sample for PlayStation 4 
 *  Repository https://github.com/psxdev/ps4link
 */
#include <debugnet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/user.h>
void listProcVm(int i)
{
	int mib[4];
	int j,k;
	size_t len;
	void *dump;
	void *aux;
	int ptrwidth;
	const char *str;
	char mio[57];
	ptrwidth = 2*sizeof(void *) + 2;
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_VMMAP;
	mib[3] = i;
	if(sysctl(mib, 4, NULL, &len, NULL, 0) == -1)
	{
		debugNetPrintf(ERROR,"sysctl vm return -1\n");
		return;
	}
	if(len > 0) 
	{
		debugNetPrintf(INFO,"PID %d number of vmaps %d\n",i,len/144);
		
		dump=malloc(len);
		if(dump)
		{
			aux=dump;
		}
		else
		{
			debugNetPrintf(ERROR,"malloc error\n");
			
			return;
		}
		if(sysctl(mib, 4, dump, &len, NULL, 0) == -1) 
		{
			debugNetPrintf(ERROR,"sysctl vm return -1\n");
			return;
		}
		else 
		{
			for(j=0;j<len/144;j++)
			{
				if(j==0)
				{
					debugNetUDPPrintf("%5s %5s %*s %*s %*s %3s %4s %4s %3s %3s %2s %-2s %-s\n","PID", "INDEX", ptrwidth, "START", ptrwidth, "END",ptrwidth, "SIZE", "PRT", "RES",
					"PRES", "REF", "SHD", "FL", "TP", "PATH");
				}
		    	char  *point=(char *) dump ;
				int	 kve_structsize=*(int *)(dump);		/* Variable size of record. */ //0
				int	 kve_type=*(int *)(dump+4);			/* Type of map entry. */ //4
				uint64_t kve_start=*(uint64_t *)(dump+8);			/* Starting address. */ //8
				uint64_t kve_end=*(uint64_t *)(dump+16);			/* Finishing address. */ //16
				uint64_t kve_size=kve_end-kve_start;			/* size. */
				uint64_t kve_offset=*(uint64_t *)(dump+24);			/* Mapping offset in object */ //24
				uint64_t kve_vn_fileid=*(uint64_t *)(dump+32);			/* inode number if vnode */ //32
				uint32_t kve_vn_fsid=*(uint32_t *)(dump+40);			/* dev_t of vnode location */ //40
				int	 kve_flags=*(int *)(dump+44);			/* Flags on map entry. */  //44
				int	 kve_resident=*(int *)(dump+48);			/* Number of resident pages. */ //48
				int	 kve_private_resident=*(int *)(dump+52);		/* Number of private pages. */ //52
				int	 kve_protection=*(int *)(dump+56);		/* Protection bitmask. */ //56
				int	 kve_ref_count=*(int *)(dump+60);			/* VM obj ref count. */ //60
				int	 kve_shadow_count=*(int *)(dump+64);		/* VM obj shadow count. */ //64
				int	 kve_vn_type=*(int *)(dump+68);			/* Vnode type. */ //68
				uint64_t kve_vn_size=*(uint64_t *)(dump+72);			/* File size. */ //72
				uint32_t kve_vn_rdev=*(uint32_t *)(dump+80);			/* Device id if device. */ //80
				uint16_t kve_vn_mode=*(uint16_t *)(dump+84);			/* File mode. */ //84
		 		uint16_t kve_status=*(uint16_t *)(dump+86);			/* Status flags. */ //86
				int	 *_kve_ispare=(int *)(dump+88);		/* Space for more stuff. */ //88
				/* Truncated before copyout in sysctl */
				char *kve_path=(char *)dump+136;//+136;		/* Path to VM obj, if any. */ //88+48=136
				//memcpy(mio,point+88,56);
				switch (kve_type) 
				{
					case KVME_TYPE_NONE:
						str = "--";
						break;
					case KVME_TYPE_DEFAULT:
						str = "df";
						break;
					case KVME_TYPE_VNODE:
						str = "vn";
						break;
					case KVME_TYPE_SWAP:
						str = "sw";
						break;
					case KVME_TYPE_DEVICE:
						str = "dv";
						break;
					case KVME_TYPE_PHYS:
						str = "ph";
						break;
					case KVME_TYPE_DEAD:
						str = "dd";
						break;
					case KVME_TYPE_SG:
						str = "sg";
						break;
					case KVME_TYPE_UNKNOWN:
					default:
						str = "??";
						break;
				}	
			//debugNetPrintf(INFO,"sysctl kve_structsize %d\n",kve_structsize);
			//debugNetPrintf(INFO,"sysctl kve_type %d\n",kve_type);
		
			debugNetUDPPrintf("%5d %5d %#*jx %#*jx %#*jx %s%s%s %4d %4d %3d %3d %-1s%-1s %-2s %-s\n",i,j,ptrwidth, (uintmax_t)kve_start,ptrwidth, (uintmax_t)kve_end,ptrwidth, (uintmax_t)(kve_size),kve_protection & KVME_PROT_READ ? "r" : "-",kve_protection & KVME_PROT_WRITE ? "w" : "-", kve_protection & KVME_PROT_EXEC ? "x" : "-",kve_resident,kve_private_resident,kve_ref_count,kve_shadow_count,kve_flags & KVME_FLAG_COW ? "C" : "-",kve_flags & KVME_FLAG_NEEDS_COPY ? "N" :"-",str,kve_path);
			usleep(500);
	 	
			dump=dump+144;
			}	
			free(aux);
		}
	}
	else
	{
		debugNetPrintf(ERROR,"PID %d number of vmaps is %d \n",i,len/144);
		return;
	}
	
}
void listProc(int i)
{
	
	int pid, mib[4],ret;
	size_t len;
	int j;
	void *aux;
	void *dump;

	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PID;//KERN_PROC_PROC;
	mib[3] = i;//0;

	if(sysctl(mib, 4, NULL, &len, NULL, 0) == -1)
	{
		//debugNetPrintf(ERROR,"sysctl return -1\n");
		return;
	}
	if(len > 0) 
	{
		//debugNetPrintf(INFO,"sysctl len  %d\n",len);
		
		dump=malloc(len);
		aux=dump;
		if(sysctl(mib, 4, dump, &len, NULL, 0) == -1) 
		{
			//debugNetPrintf(ERROR,"sysctl return -1\n");
			return;
		}
		else 
		{
			debugNetUDPPrintf("%5s %5s %5s %5s %5s %-8s %-9s %-17s %-20s %-16s\n",
			    "PID", "PPID", "PGID", "SID", "TSID", "LOGIN",
			    "WCHAN", "EMUL", "COMM", "TDNAME");
			//for(j=0;j<len/1096;j++)
			//{
		    char  *point=(char *) dump ;
			int ki_structsize=*(int *)(dump); //0
		//	debugNetPrintf(INFO,"sysctl ki_structsize %d\n",ki_structsize);
			
			pid_t ki_pid= *(int *)(dump+0x48); //72
			pid_t ki_ppid= *(int *)(dump+0x4c); //76
			pid_t ki_pgid=*(int *)(dump+0x50);	/* process group id */ //80
			pid_t ki_tpgid=*(int *)(dump+0x54);		/* tty process group id */ //84
			pid_t ki_sid=*(int *)(dump+0x58);			/* Process session ID */ //88
			pid_t ki_tsid=*(int *)(dump+0x5c);		/* Terminal session ID */ //92 
			char *ki_tdname=dump+0x18a;
			char *ki_wmesg=dump+0x19b;
			char *ki_login=dump+0x1a4;
			char *ki_comm=dump+0x1bf;
			char *ki_emul=dump+0x1d3;
		//	debugNetPrintf(INFO,"PID %d PPID %d LOGIN: %s WCHAN: %s EMUL: %s COMM: %s TDNAME: %s\n",ki_pid, ki_ppid, ki_login, ki_wmesg, ki_emul, ki_comm, ki_tdname);
			debugNetUDPPrintf("%5d %5d %5d %5d %5d %-8s %-9s %-17s %-20s %-16s\n", ki_pid, ki_ppid,ki_pgid,ki_sid,ki_tsid, strlen(ki_login) ? ki_login : "-", strlen(ki_wmesg) ? ki_wmesg : "-", strcmp(ki_emul, "null") ? ki_emul : "-", ki_comm, ki_tdname);
			usleep(500);
			free(aux);
			usleep(100);
			
			listProcVm(i);
			
		}
	
	}
}

int main(int argc, char *argv[])
{
	int ret;
	int i;
	int cnt;
	struct kinfo_proc *p;
	uintptr_t intptr=0;
	sscanf(argv[1],"%p",&intptr);
	debugNetConfiguration *conf=(debugNetConfiguration *)intptr;
	ret=debugNetInitWithConf(conf);
	sleep(3);
	debugNetPrintf(DEBUG,"[LISTPROC] %d elfname=%s ps4linkconf=%s %p %d\n",argc,argv[0],argv[1],conf,conf->SocketFD);
	
	for(i=0;i<200;i++)
	{
		listProc(i);
	}
	
	debugNetFinish();
	
	return EXIT_SUCCESS;
}

#include "ps4sh.h"
//your default ps4 ip
static char dst_ip[16] = "192.168.1.17";
//default listen in all interfaces
static char src_ip[16] = "0.0.0.0";
static char ps4sh_history[MAXPATHLEN];
static int time1, time2, time_base = 0;
static struct timeval benchtime;
static char device[MAXPATHLEN];
static char dir[MAXPATHLEN];
static char filename[MAXPATHLEN];
//static char device2[MAX_PATH];
//static char dir2[MAX_PATH];
//static char filename2[MAX_PATH];

//debugnet udp log
extern int console_socket;
//ps4link tcp fio service
extern int request_socket;
//ps4link udp command service
extern int command_socket; 
//ps4sh tcp socket for third parties apps
extern int ps4sh_socket; 




int ps4sh_log_read(int fd)
{
	int ret;
	unsigned int size = sizeof(struct sockaddr_in);
	static char buf[1024];
	static int loc = 0;
	struct sockaddr_in dest_addr;

	if(loc == 0)
		memset(buf, 0x0, sizeof(buf));

	memset(&(dest_addr), '\0', size);

	/* Receive from, size must be at least 1 smaller 
		than buffer for a NULL */
	ret = recvfrom(fd, buf + loc, sizeof(buf) - loc - 1, 0,(struct sockaddr *)&dest_addr, &size);

	if (ret == -1) 
	{
		debugNetPrintf(ERROR,"recvfrom error %s\n",strerror(errno));
	}

	loc += ret;

	if (log_to_file) 
	{
		write(log_f_fd, buf, strlen(buf));
		loc = 0;
	/* log to file isn't line orientated */
	} 
	else 
	{
		if(((strchr(buf, '\n')) || (sizeof(buf) - loc - 1) <= 0))
		{
		/* Ideally we want entire lines but just in case check 
		 for NL at end of string */
			int str_size = strlen(buf);
			if(buf[str_size - 1] != '\n')
				buf[str_size - 1] = '\n';

			write_log_line(buf);
			fflush(stdout);
			loc = 0;
		}
	}
	return ret;
}

int ps4sh_srv_read(int fd) 
{
	int length = 0;
	int ret = 0;
	
	struct { unsigned int number; unsigned short length; char buffer[512]; } PACKED packet;
	
	// Read in the request packet header.
	length=network_receive_all(request_socket, &packet, 6);
	if ( length < 0 ) {
		return length;
	} 
	else if ( length == 0 ) 
	{
		if ( fd == request_socket ) 
		{
			close(fd);
			change_prompt();
			while(1) 
			{
				sleep(1);
				request_socket = ps4link_fio_listener(dst_ip, SRV_PORT, 1);
				if (request_socket > 0) 
				{
					break;
				}
			}
			change_prompt();
		} 
		else 
		{
			return -1;
		}
	} 
	else 
	{

		// Read in the rest of the packet.
		network_receive_all(request_socket, packet.buffer, ntohs(packet.length) - 6);
		switch(ntohl(packet.number))
		{
			case PS4LINK_OPEN_CMD:  
				if (VERBOSE) 
				{
					gettimeofday(&benchtime, NULL);
					time1=(benchtime.tv_sec - time_base)*USEC+benchtime.tv_usec;
				}   
				ps4link_request_open(&packet);     
				break;
			case PS4LINK_CLOSE_CMD: 
				if (VERBOSE)
				{
					gettimeofday(&benchtime, NULL);
					time2=(benchtime.tv_sec - time_base)*USEC+benchtime.tv_usec;
					debugNetPrintf(DEBUG,"took %2.3fs\n", ((float)(time2-time1)/(float)USEC));
				}
				ps4link_request_close(&packet);
				break;    
			case PS4LINK_READ_CMD:    
				ps4link_request_read(&packet);    
				break;
			case PS4LINK_WRITE_CMD:    
				ps4link_request_write(&packet);    
				break;
			case PS4LINK_LSEEK_CMD:    
				ps4link_request_lseek(&packet);   
				break;
			case PS4LINK_OPENDIR_CMD:  
				ps4link_request_opendir(&packet);  
				break;
			case PS4LINK_CLOSEDIR_CMD: 
				ps4link_request_closedir(&packet); 
				break;
			case PS4LINK_READDIR_CMD:  
				ps4link_request_readdir(&packet);  
				break;
			case PS4LINK_REMOVE_CMD:   
				ps4link_request_remove(&packet);
				break;
			case PS4LINK_MKDIR_CMD:    
				ps4link_request_mkdir(&packet);  
				break;   
			case PS4LINK_RMDIR_CMD:    
				ps4link_request_rmdir(&packet);  
				break;
			case PS4LINK_EXECUSER_CMD:    
			//	ps4link_command_execelf(&packet);  
			printf("Received execee request/command number (%x)\n",packet.number);
				break;
			case PS4LINK_EXECKERNEL_CMD:    
		//		ps4link_command_execsprx(&packet);  
				break;
			default:
				debugNetPrintf(DEBUG,"Received unsupported request/command number (%x)\n",packet.number);
				break;
		}
	}
	return ret;
}
int client[MAX_CLIENTS];
fd_set master_set, readset, clientset;
int maxfd,maxi;

int main(int argc, char* argv[])
{
	

	//declare variables
    int ret;
    // poll client stuff
    int i, j, connfd, sockfd;
    struct sockaddr_in cliaddr;
    struct timeval clienttimeout;
    int nready=0;
    socklen_t clilen;
    //
    VERBOSE = 0;
    printf("ps4sh version %s\n",PS4SH_VERSION);
	//call read config if exist we need redefine syntax and variables
    read_config();
	//if we call ps4sh with parameter it is the ps4 ip copy it to proper variable
    if (argc == 2) {
        strcpy(dst_ip, argv[1]);
        dst_ip[strlen(argv[1])] = '\0';
    }
    clienttimeout.tv_sec = 0;
    clienttimeout.tv_usec = USEC;
	
	
	
	
    // client stuff
	for(i = 0; i < MAX_CLIENTS; i++) {
		client[i] = -1;
	}
	FD_ZERO(&clientset);
	// end client stuff

	printf("\n");

	//create console log udp socket bind to 0.0.0.0

	console_socket = ps4link_log_listener(src_ip, LOG_PORT);

	if ( console_socket < 0 ) {
		perror("");
		printf("Unable to start log service!\n");
		//if i can't create local udp socket listening in LOG_PORT exit
		return 1;
	}
	//create ps4sh listener to let tools from third parties connect to ps4sh
	ps4sh_socket = ps4link_srv_setup(src_ip, SRV_PORT);
	if ( ps4sh_socket < 0 ) {
		perror("");
		printf("Unable to start command server!\n");
		//if i can't create local tcp socket listening in SRV_PORT exit
		return 1;
	}
	//populate set  standard output, command listener connected to ps4, udp logs listener, local command listener
	FD_ZERO(&master_set);
	//standard output
	FD_SET(0, &master_set);
	//udp log channel
	FD_SET(console_socket, &master_set);
	//ps4sh channel
	FD_SET(ps4sh_socket, &master_set);
	client[0] = 0;
	client[1] = console_socket;
	client[2] = ps4sh_socket;
	request_socket=-1;
	client[3] = request_socket;
	
	maxfd = ps4sh_socket;
	maxi = 2;
	
	
	//initilize readline
	initialize_readline();
	debugNetPrintf(INFO,"Ready\n");
    
	//mail loop
	while(doloop) {
		readset = master_set;
		ret = select(maxfd+1, &readset, NULL, NULL, NULL);
		if ( ret < 0 )
		{
			if ( FD_ISSET(0, &readset) ) {
				continue;
			}
			debugNetPrintf(ERROR,"In select %s\n",strerror(errno));
			break;
		} 
		else if (ret == 0) 
		{
			/* no file desc are ready, lets move on in life */
		} 
		else 
		{
			for(i = 0; i <= maxi; i++) 
			{
				if ( (sockfd = client[i]) < 0) 
				{
					continue;
				}
				if ( !FD_ISSET(sockfd, &readset) ) 
				{
					continue;
				}
				//if we have udp log messages from ps4 debugnet
				if ( sockfd == console_socket) {
					ps4sh_log_read(console_socket);
				} 
				//if we have local standard  messages
				else if (sockfd == 0) 
				{
					rl_callback_read_char();
				}
				//if we have messages related to ps4link fio service
				else if(sockfd == request_socket) 
				{
					ps4sh_srv_read(request_socket);
				} 
				//if we have pending connection related to ps4sh server listener
				else if (sockfd == ps4sh_socket) 
				{
					clilen = sizeof(cliaddr);
					connfd = accept(ps4sh_socket, (struct sockaddr *)&cliaddr, &clilen);
					//search empty space
					for(j = 0; i<FD_SETSIZE; j++) 
					{
						if(client[j] < 0) 
						{
							client[j] = connfd;
							break;
						}
					}
					//populate in the set
					FD_SET(connfd, &master_set);
					//sanity checks
					if(connfd > maxfd) 
					{
						maxfd = connfd;
					}
					if ( j > maxi ) 
					{
						maxi = j;
					}
					if (--nready <= 0) 
					{
						continue;
					}
				} 
				else 
				{
					if ( ps4sh_srv_read(sockfd) < 0 ) {
						close(sockfd);
						FD_CLR(sockfd, &master_set);
						client[i] = -1;
						maxi--;
					}
				}
			}
		}
	}

	rl_callback_handler_remove();
	if(request_socket>0)
	{
		if ( (ret = network_disconnect(request_socket)) == -1 ) 
		{
		debugNetPrintf(ERROR,"From request_socket network_disconect %s\n",strerror(errno));
		}
	}
	if ( (ret = network_disconnect(console_socket)) == -1 ) {
		debugNetPrintf(ERROR,"From console_socket network_disconect %s\n",strerror(errno));		
	}
	if ( log_to_file ) {
		if ((ret = close(log_f_fd)) == -1)
			debugNetPrintf(ERROR,"From file log closing %s\n",strerror(errno));				
	}
    
    



	if (strcmp(ps4sh_history, "") != 0 ) {
		if ( (ret = write_history(ps4sh_history)) != 0) 
			debugNetPrintf(ERROR,"From ps4sh_history %s\n",strerror(errno));				
	}
	printf("\n");
	return(0);
		
}


/* 
 *      CLI commands
 */
static char clicom[1024];

int execute_line(char *line)
{
	int i, ret;
	COMMAND *command;
	char *word;
	char *arg = strdup(line);

	i = 0;
	while (line[i] && whitespace(line[i]))
		i++;
	word = line + i;

	while (line[i] && !whitespace(line[i]))
		i++;

	if (line[i])
		line[i++] = '\0';

	command = find_command(word);

	if (!command) 
	{
		ret = system(arg);
		free(arg);
		return ret;
	}

	while (whitespace(line[i]))
		i++;

	word = line + i;
	free(arg);
	return ((*(command->func))(word));
}

COMMAND * find_command(char *name)
{
	int i = 0;
	for (i = 0; commands[i].name; i++)
		if (strcmp(name, commands[i].name) == 0)
			return(&commands[i]);

	return ((COMMAND *)NULL);
}


int cli_make(char *arg)
{
	if (!arg)
		return 0;
	sprintf(clicom, "make %s", arg);
	return (system(clicom));
}

int cli_gmake(char *arg)
{
	if (!arg)
		return 0;
	sprintf(clicom, "gmake %s", arg);
	return (system(clicom));
}

int cli_list(char *arg)
{
	if (!arg)
		arg = "";

	split_filename(device, dir, filename, arg);
	if(device[0] != '\0') 
	{     
		return 0;
	} 
	else 
	{
		sprintf(clicom, "ls -l %s", arg);
		return (system(clicom));
	}
}

int cli_cd(char *arg)
{
	char line[MAXPATHLEN];
	char key[MAXPATHLEN];
	char value[MAXPATHLEN];
	if (strlen(arg) == 0) 
	{
		get_home(line);
		read_pair(line, key, value);
		chdir(value);
		cli_pwd();
		return 0;
	}
	if (chdir(arg) == -1) 
	{
		debugNetPrintf(ERROR,"%s %s\n",strerror(errno));
		return 1;
	}
	cli_pwd();
	return (0);
}

int cli_help(char *arg)
{
	int i = 0;
	int printed = 0;
	for(i = 0; commands[i].name; i++) 
	{
		if (!*arg || (strcmp(arg, commands[i].name) == 0)) 
		{
			printf("%-10s  %s.\n", commands[i].name, commands[i].doc);
			printed++;
		}
	}
	if (!printed) 
	{
		printf(" No commands match '%s'. Available commands are:\n", arg);
		for (i = 0; commands[i].name; i++) 
		{
			if(printed == 6) 
			{
				printed = 0;
				printf("\n");
			}
			printf(" %s\t", commands[i].name);
			printed++;
		}

		if (printed)
			printf("\n");
	}
	return 0;
}

int cli_pwd()
{
	char dir[1024], *s;
	s = getwd(dir);
	if (s == 0) {
		fprintf(stdout, "Error getting pwd: %s\n", dir);
		return 1;
	}
	fprintf(stdout, "%s\n", dir);
	return 0;
}

int cli_debug() 
{
	if (ps4link_debug()) {
		ps4link_set_debug(0);
		printf(" Debug off\n");
	} 
	else 
	{
		ps4link_set_debug(1);
		printf(" Debug on\n");
	}
	return 0;
}


int cli_status() 
{
	debugNetPrintf(INFO," TCP srv fd = %d\n", request_socket);
	debugNetPrintf(INFO," UDP log fd = %d\n", console_socket);
	debugNetPrintf(INFO," PS4SH cmd fd = %d\n", ps4sh_socket);
	if ( log_to_file )
		debugNetPrintf(INFO," Logging to file\n");
	else
		debugNetPrintf(INFO," Logging to stdout\n");
	if ( VERBOSE )
		debugNetPrintf(INFO," Verbose mode is on\n");
	else
		debugNetPrintf(INFO," Verbose mode is off\n");

	if(ps4link_debug()) {
		debugNetPrintf(INFO," Debug is on\n");
	} 
	else 
	{
		debugNetPrintf(INFO," Debug is off\n");
	}
    /*
	if ( DUMPSCREEN ) {
		printf(" Exception dumps to screen\n");
	} 
	else 
	{
		printf(" Exception dumps to console\n");
	}*/
	return 0;
}

int cli_quit() 
{
	doloop = 0;
	return 0;
}
int cli_connect()
{
	
	if(request_socket<0)
	{
		// create request socket connected to ps4link fio service
		debugNetPrintf(DEBUG,"Connecting to fio ps4link ip %s ", dst_ip);
	
		request_socket = ps4link_fio_listener(dst_ip, SRV_PORT, 10);
		if (request_socket < 0) {
			printf(", failed\n");
		}
		else
		{
			//ps4link fio channel
			FD_SET(request_socket, &master_set);
			client[3] = request_socket;
			maxfd = request_socket;
			maxi=3;
		
		
		}
		//udp socket to send commands to ps4
		command_socket = network_connect(dst_ip, 0x4712, SOCK_DGRAM);
		if (command_socket < 0) 
		{
			printf(", failed\n");
		}
	}
	else
	{
		debugNetPrintf(ERROR,"you are already connected\n");
	}
	
	return 0;
	
	
}
int cli_reconnect() 
{
	if(request_socket>0)
	{
		close(request_socket);
		request_socket = ps4link_fio_listener(dst_ip, SRV_PORT, 10);
		client[3] = request_socket;
	}
	return 0;
}

int cli_log(char *arg)
{
	trim(arg);
	if ( (strcmp(arg, "stdout"))==0 || !*arg)
	{
		if ( log_to_file )
		{
			if (VERBOSE)
				debugNetPrintf(DEBUG,"Closing log file fd\n");
			close(log_f_fd);
		}
		if (VERBOSE)
			debugNetPrintf(DEBUG,"Logging to stdout\n");
		log_to_file = 0;
	} 
	else 
	{
		if (VERBOSE)
			debugNetPrintf(DEBUG,"Open file %s for logging\n", arg);
		log_to_file = 1;
		log_f_fd = open(arg, LOG_F_CREAT, LOG_F_MODE);
	}
	return 0;
}

int cli_verbose() {
	if (VERBOSE) {
		debugNetPrintf(DEBUG," Verbose off\n");
		VERBOSE = 0;
	} else {
		VERBOSE = 1;
		debugNetPrintf(DEBUG," Verbose on\n");
	}
	return 0;
}


int cli_setroot(char *arg)
{
	ps4link_set_root(arg); //do nothing by now
	return(0);
}

int cli_execuser(char *arg)
{
	//char *newarg;
    int argc, argvlen;
    char argv[MAX_PATH];
    argc = fix_cmd_arg(argv, arg, &argvlen);
	debugNetPrintf(DEBUG,"argc=%d argv=%s\n",argc,argv);
	if(request_socket>0)
	{
	    ps4link_command_execuser(argc,argv,argvlen);
	}
	else
	{
		debugNetPrintf(ERROR,"you need to use connect command first\n");
		
	}
    return 0;
}
int cli_execkernel(char *arg)
{
	//char *newarg;
    int argc, argvlen;
    char argv[MAX_PATH];
    argc = fix_cmd_arg(argv, arg, &argvlen);
	debugNetPrintf(DEBUG,"[PS4SH] argc=%d argv=%s\n",argc,argv);
	if(request_socket>0)
	{
	    ps4link_command_execkernel(argc,argv,argvlen);
	}
	else
	{
		debugNetPrintf(ERROR,"you need to use connect command first\n");
		
	}
    return 0;
}
int cli_execdecrypt(char *arg)
{
	//char *newarg;
    int argc, argvlen;
    char argv[MAX_PATH];
    argc = fix_cmd_arg_non_host(argv, arg, &argvlen);
	debugNetPrintf(DEBUG,"[PS4SH] argc=%d argv=%s\n",argc,argv);
	if(request_socket>0)
	{
		ps4link_command_execdecrypt(argc,argv,argvlen);
	}
	else
	{
		debugNetPrintf(ERROR,"you need to use connect command first\n");
		
	}
	
    return 0;
}
int cli_exitps4(char *arg)
{
	//char *newarg;
    int argc, argvlen;
    char argv[MAX_PATH];
    argc = fix_cmd_arg(argv, arg, &argvlen);
	debugNetPrintf(DEBUG,"argc=%d argv=%s\n",argc,argv);
	if(request_socket>0)
	{	
    	ps4link_command_exit(argc,argv,argvlen);
	}
	else
	{
		debugNetPrintf(ERROR,"you need to use connect command first\n");
		
	}
	doloop=0;
    return 0;
}
int cli_execwhoami(char *arg)
{
	//char *newarg;
    int argc, argvlen;
    char argv[MAX_PATH];
    argc = fix_cmd_arg(argv, arg, &argvlen);
	debugNetPrintf(DEBUG,"[PS4SH] argc=%d argv=%s\n",argc,argv);
	if(request_socket>0)
	{
    	ps4link_command_execwhoami(argc,argv,argvlen);
	}
	else
	{
		debugNetPrintf(ERROR,"you need to use connect command first\n");
		
	}
	
    return 0;
}
int cli_execshowdir(char *arg)
{
	//char *newarg;
    int argc, argvlen;
    char argv[MAX_PATH];
	
    argc = fix_cmd_arg_non_host(argv, arg, &argvlen);
	debugNetPrintf(DEBUG,"[PS4SH] argc=%d argv=%s\n",argc,argv);
	if(request_socket>0)
	{
		ps4link_command_execshowdir(argc,argv,argvlen);
	}
	else
	{
		debugNetPrintf(ERROR,"you need to use connect command first\n");
		
	}
    return 0;
}
/*
 * Readline init.
 */
int initialize_readline(void)
{
	rl_bind_key_in_map(META ('p'), cli_pwd, emacs_standard_keymap);
	rl_bind_key_in_map(META ('q'), cli_quit, emacs_standard_keymap);
	rl_bind_key_in_map(META ('s'), cli_status, emacs_standard_keymap);
	rl_bind_key_in_map(META ('v'), cli_verbose, emacs_standard_keymap);
	rl_readline_name = "ps4sh";
	rl_attempted_completion_function = command_completion;
	rl_ignore_some_completions_function = filename_completion_ignore;
	
	if (strcmp(ps4sh_history, "") != 0) {
		if (read_history(ps4sh_history) != 0) {
			perror(ps4sh_history);
		}
	}
	rl_callback_handler_install(get_prompt(), cli_handler);
	return 0;
}




char* command_generator(const char* text, int state)
{
	static int list_index, len;
	char *name;
 
	if (!state) {
		list_index = 0;
		len = strlen (text);
	}
 
	while ((name = commands[list_index].name)) {
		list_index++;
 
		if (strncmp (name, text, len) == 0)
			return (dupstr(name));
	}
 
/* If no names matched, then return NULL. */
	return ((char *)NULL);
 
}
void read_config(void)
{
	static char key[MAXPATHLEN];
	static char value[MAXPATHLEN];
	static char line[MAXPATHLEN];
	char *ptr = value;
	FILE *fd;
	if(get_home(line)) 
	{
		read_pair(line, key, value);
	} 
	else 
	{
		strcpy(value, "./");
	}

	strcat(value, "/.ps4shrc");
	if ( (fd = fopen(value, "rb")) == NULL ) 
	{
		perror(value);
		return;
	}

	while((read_line(fd, line)) != -1) 
	{
		if (line[0] == '#') 
		{
			continue;
		}
		read_pair(line, key, value);
		trim(key);
		ptr = stripwhite(value);
		if (strcmp(key, "ip") == 0) 
		{
			strcpy(dst_ip, ptr);
		} 
		else if (strcmp(key, "log") == 0) 
		{
			if (strcmp(ptr, "") == 0) 
			{
				cli_log("stdout");
			} 
			else {
				cli_log(ptr);
			}
		} 
	/*	else if (strcmp(key, "exception") == 0) 
		{
			if (strcmp(ptr, "screen") == 0) 
			{
				DUMPSCREEN = 1;
			} 
			else 
			{
				DUMPSCREEN = 0;
			}
		} */
		else if (strcmp(key, "verbose") == 0) 
		{
			if (strcmp(ptr, "yes") == 0) 
			{
				printf(" Verbose mode on\n");
				VERBOSE = 1;
			}
		} 
		else if (strcmp(key, "debug") == 0) 
		{
			if (strcmp(ptr, "yes") == 0) 
			{
				ps4link_set_debug(1);
			}
		}
		else if (strcmp(key, "histfile") == 0) 
		{
			if (strcmp(ptr, "") != 0) 
			{
				strcpy(ps4sh_history, ptr);
			}
		} 
		else if (strcmp(key, "bind") == 0) 
		{
			if (strcmp(ptr, "") != 0) 
			{
				strcpy(src_ip, ptr);
			}
		} 
		else if (strcmp(key, "path") == 0) 
		{
			if (strcmp(ptr, "") != 0) 
			{
				ps4link_set_path(ptr);
			}
		} 
		else if (strcmp(key, "suffix") == 0) 
		{
			if (strcmp(ptr, "") != 0) 
			{
				common_set_suffix(path_split(ptr));
			}
		} 
		else if (strcmp(key, "setroot") == 0) 
		{
			if (strcmp(ptr, "") != 0) 
			{
				ps4link_set_root(ptr);
			}
		} 
		else if (strcmp(key, "logprompt") == 0) 
		{
			if (strcmp(ptr, "") != 0) 
			{
				/* pko_set_root(ptr); */
			}
		} 
		else if (strcmp(key, "home") == 0) 
		{
			if (strcmp(ptr, "") != 0) 
			{
				if (chdir(ptr) == -1) 
				{
					perror(ptr);
				}
			}
		}
	}
    return;
}

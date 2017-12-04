
const char * usage =
"                                                                \n"
"httpd-server:                                                   \n"
"                                                                \n"
"Simple server program that serves files to a client over        \n"
"telnet.                                                         \n"
"                                                                \n"
"To use it in one window type:                                   \n"
"                                                                \n"
"   myhttpd <-f|-t|-p> <port>                                    \n"
"                                                                \n"
"Where 1024 < port < 65536 is optional.                          \n"
"And where -f, -t, or -p specifies concurrency mode.             \n"
"                                                                \n"
"In another window type:                                         \n"
"                                                                \n"
"   telnet <host> <port>                                         \n"
"                                                                \n"
"where <host> is the name of the machine where httpd-server      \n"
"is running. <port> is the port number you used when you run     \n"
"myhttpd-server. (65432 by default)                              \n"
"                                                                \n"
"Then type:                                                      \n"
"   GET /<secret key>/<Document Requested> HTTP/1.0<enter><enter>\n"
"                                                                \n"
"You will then be served the requested file.                     \n";


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <regex.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>

#define DEFAULT_PORT 65432

const char * SECRET_KEY = "/test";

int QueueLength = 5;

//Process file request
void processRequest(int socket);
//check location of file
char * checkFileName(char * fileName);
//return type of file
char * getDocType(char * fileName);
//check if request is formatted properly
bool regexCheck(char buffer[]);

//concurrency types
//fork process after each request
void forkServer(int masterSocket);
//zombie process handler
extern "C" void zombieHandler(int sig);


//Create thread after each request
void createThreadServer(int masterSocket);
void processRequestThread(int masterSocket);

//Pool of threads
void poolOfThreads(int masterSocket);
void * loopthread(int masterSocket);

pthread_mutex_t mutex;


int main( int argc, char ** argv ) {
	// Print usage if not correct number of args
	if (argc > 1 && !strcmp(argv[1], "help")) {
		fprintf( stderr, "%s", usage );
		exit(0);
	}

	int port;
	if(argc > 2) {
		port = atoi(argv[2]);
	} else if(argc == 2) {
		if(argv[1][0] >= '0' && argv[1][0] <= '9') {
			port = atoi(argv[1]);
		} else {
			port = DEFAULT_PORT;
		} 
	} else {
		port = DEFAULT_PORT;
	}

	if(port < 1024 || port > 65536) {
		printf("Port out of range. Starting on default port.\n");
		port = DEFAULT_PORT;
	}

	// Set the IP address and port for this server
	struct sockaddr_in serverIPAddress; 
	memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_addr.s_addr = INADDR_ANY;
	serverIPAddress.sin_port = htons((u_short) port);

	// Allocate a socket
	int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
	if ( masterSocket < 0) {
		perror("socket");
		exit( -1 );
	}

	// Set socket options to reuse port. Otherwise we will
	// have to wait about 2 minutes before reusing the sae port number
	int optval = 1; 
	int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
			(char *) &optval, sizeof( int ) );

	// Bind the socket to the IP address and port
	int error = bind( masterSocket,
			(struct sockaddr *)&serverIPAddress,
			sizeof(serverIPAddress) );
	if ( error ) {
		perror("bind");
		exit( -1 );
	}

	// Put socket in listening mode and set the 
	// size of the queue of unprocessed connections
	error = listen( masterSocket, QueueLength);
	if ( error ) {
		perror("listen");
		exit( -1 );
	}

	if(argc >= 2) {
		if(!strcmp(argv[1], "-f")) { //fork mode
			//set up sigaction handler for zombie processes
			struct sigaction sa_zom;
			sa_zom.sa_handler = zombieHandler;
			sigemptyset(&sa_zom.sa_mask);
			sa_zom.sa_flags = SA_RESTART;

			if(sigaction(SIGCHLD, &sa_zom, NULL)) {
				perror("Sigaction zombie");
				exit(2);
			}
			//start in fork mode
			forkServer(masterSocket);
		} else if(!strcmp(argv[1], "-t")) { //new thread mode
			createThreadServer(masterSocket);
		} else if(!strcmp(argv[1], "-p")) { //pool of threads mode
			poolOfThreads(masterSocket);
		}
	}

	//Since all three of those start infinite loops, if you get here, the flags above are not included
	printf("Starting in iterative mode.\n");
	while ( 1 ) {

		// Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof( clientIPAddress );
		int slaveSocket = accept( masterSocket,	(struct sockaddr *)&clientIPAddress,
			(socklen_t*)&alen);

		if ( slaveSocket < 0 ) {
			perror( "accept" );
			exit( -1 );
		}

		// Process request.
		processRequest( slaveSocket );

		// Close socket
		close( slaveSocket );
	}
}

void processRequest(int fd) {
	const int MaxBuffer = 1024;
	char buffer[MaxBuffer + 1];
	int bufferLength = 0;
	int n;

	//Current character read
	unsigned char newChar;
	//Last character read
	unsigned char lastChar = 0;
	//2nd to last character read
	unsigned char secLastChar = 0;
	//3rd to last character read
	unsigned char thiLastChar = 0;


	//The client should sent GET <file location> HTTP/1.0<cr><lf><cr><lf>
	//Read request character by character until <cr><lf><cr><lf> is found
	while(bufferLength < MaxBuffer && (n = read(fd, &newChar, sizeof(newChar))) > 0) {

		if(thiLastChar == '\015' && secLastChar == '\012' && lastChar == '\015' && newChar == '\012') {
			//discard previous <cr><lf><cr> from buffer
			bufferLength -= 3;
			break;
		}
	
		buffer[bufferLength++] = newChar;
		
		thiLastChar = secLastChar;
		secLastChar = lastChar;
		lastChar = newChar; 
	}

	//add null character at end of buffer
	buffer[bufferLength] = '\0';
	printf("Buffer: %s\n", buffer);

	//we only actually care about the first line, so discard the rest.
	if(strchr(buffer, '\015')) {
		*strchr(buffer, '\015') = '\0';
	}

	//Check that request is formatted properly
	if(regexCheck(buffer)) {
		const char * invalid = "Invalid message.\n";
		write(fd, invalid, strlen(invalid));
		return;
	}
	
	//Retrieve file name from buffer
	char * fileName;

	//find beginning and end
	char * begin = strchr(buffer, ' ') + 1;
	char * end = strrchr(begin, ' ');
	fileName = strndup(begin, end - begin);

	//check for secret key in fileName
	if(strlen(fileName) < strlen(SECRET_KEY) || strncmp(fileName, SECRET_KEY, strlen(SECRET_KEY) - 1)) {

		const char * invalidKey = "Invalid key.\n";
		write(fd, invalidKey, strlen(invalidKey));
		free(fileName);
		return;
	}

	fileName += strlen(SECRET_KEY);
	printf("File Name: %s\n", fileName);
	
	//Check that file resides in ./http-root-dir/
	char * realFilePath = checkFileName(fileName);
	
	//get document type
	char * docType = getDocType(realFilePath);

	//send response
	const char * serverType = "Server: CS 252 lab5\n";
	if(realFilePath == NULL) { //not found
		//send header meta information
		const char * notFoundMsg = "HTTP/1.0 404 File Not Found\n";
		write(fd, notFoundMsg, strlen(notFoundMsg));
		write(fd, serverType, strlen(serverType));
		write(fd, docType, strlen(docType));		
		const char * errorMsg = "Could not find the specified URL. The server returned an error.\n";
		write(fd, errorMsg, strlen(errorMsg));
	} else if(!strcmp(realFilePath, "illegal")) {
		//illegal access
		const char * illegalMsg = "HTTP/1.0 403 Forbidden\n";
		write(fd, illegalMsg, strlen(illegalMsg));
		write(fd, serverType, strlen(serverType));
		write(fd, docType, strlen(docType));
		const char * errorMsg = "You do not have permission to access the specified URL.\n";
		write(fd, errorMsg, strlen(errorMsg));
		free(realFilePath);
	} else {
		//send header meta info
		const char * foundMsg = "HTTP/1.0 200 Document follows\n";
		write(fd, foundMsg, strlen(foundMsg));
		write(fd, serverType, strlen(serverType));
		write(fd, docType, strlen(docType));

		const char * crlf = "\n";
		write(fd, crlf, strlen(crlf));
		
		//send file data
		char c;
		int file = open(realFilePath, O_RDONLY);
		int count;
		while(count = read(file, &c, 1)) {
			if(write(fd, &c, 1) != count)
				perror("Write");
		}
		write(fd, crlf, strlen(crlf));		
		close(file);
		free(realFilePath);		
	}
	free(docType);
	free(fileName - strlen(SECRET_KEY)); //we moved this pointer ahead earlier
}

//returns path to file if file resides in ./http-root-dir/ and NULL if it does not
//If path starts with /icons/, will return path to a file in /icons/
//Otherwise, will return path to a file in /htdocs/
char * checkFileName(char * fileName) {
	

	if(*fileName == '\0' || !strcmp(fileName, "/")) {
		//return index.html
		char * filePath = realpath("./http-root-dir/htdocs/index.html", NULL);
		return filePath;
	}

	const char * relativePath = "./http-root-dir/";
	char * rootPath = realpath(relativePath, NULL);
	
	char * relativeFilePath = (char *)calloc(1024, sizeof(char));
	strcat(relativeFilePath, rootPath);
	
	if(strlen(fileName) < strlen("/icons") || strncmp(fileName, "/icons", strlen("/icons"))) {
		strcat(relativeFilePath, "/htdocs");
	}

	strcat(relativeFilePath, fileName);
	
	char * filePath = realpath(relativeFilePath, NULL);
	free(relativeFilePath);

	printf("Root: %s\nFile: %s\n", rootPath, filePath);

	//check that root path is a prefix of filePath
	if(filePath != NULL && !strncmp(rootPath, filePath, strlen(rootPath))) {
		//is a prefix

		//need to free extraneous strings
		free(rootPath);
		
		return filePath;
	} else if (filePath == NULL) {
		//illegal access
		free(rootPath);
		
		return strdup("illegal");
	} else {
		//not a prefix
		free(rootPath);
		return NULL;
	}	
}

char * getDocType(char * fileName) {
	char * extension = strrchr(fileName, '.');
	if(!strcmp(extension, ".html") || !strcmp(extension, ".htm")) {
		char * out = strdup("Content-type: text/html\015\012");
		return out;
	} else if(!strcmp(extension, ".gif")) {
		char * out = strdup("Content-type: image/gif\015\012");
		return out;
	} else if(!strcmp(extension, ".png")) {
		char * out = strdup("Content-type: image/png\015\012");
		return out;
	} else if(!strcmp(extension, ".jpg") || !strcmp(extension, ".jpeg")) {
		char * out = strdup("Content-type: image/jpg\015\012");
		return out;
	} else if(!strcmp(extension, ".svg")) {
		char * out = strdup("Content-type: image/svg\015\012");
		return out;
	} else if(!strcmp(extension, ".txt")) {
		char * out = strdup("Content-type: text/plain\015\012");
		printf("Extension: %s\n", out);
		return out;
	} else if(!strcmp(extension, ".db")) {
		char * out = strdup("Content-type: text/db\015\012");
		return out;
	} else if(!strcmp(extension, ".js")) {
		char * out = strdup("Content-type: text/js\015\012");
		return out;
	} else {
		char * out = strdup("Content-type: test/plain\015\012");
		return out;
	}	
}

bool regexCheck(char buffer[]) {
	const char * regex = "^GET .+ HTTP\\/1.[0-1]$";
	regex_t preg;
	if(regcomp(&preg, regex, REG_EXTENDED|REG_NOSUB)) {
		perror("Regex compiliation failure");
		exit(1);
	}
	bool out;
	out = regexec(&preg, buffer, 1, NULL, 0);
	regfree(&preg);
	return out;
}

void forkServer(int masterSocket) {
	printf("Starting in fork mode.\n");
	while(1) {	
		// Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof( clientIPAddress );
		int slaveSocket = accept( masterSocket, (struct sockaddr *)&clientIPAddress, (socklen_t*)&alen);

		if ( slaveSocket < 0 ) {
			perror( "accept" );
			exit( -1 );
		}
		
		int ret = fork();
		if(ret == 0) {
			// Process request.
			processRequest(slaveSocket);
			exit(0);
		}

		// Close socket
		close( slaveSocket );
	
	}
}
//zombie process handler
extern "C" void zombieHandler(int sig) {
	pid_t pid;
	int status;

	//kill jedi younglings
	while((pid = waitpid(-1, &status, WNOHANG)) >0)
		printf("PID %d exited.\n", pid);	
}


void createThreadServer(int masterSocket) {
	printf("Starting in thread create mode.\n");
	while(1) {
		// Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof( clientIPAddress );
		int slaveSocket = accept( masterSocket, (struct sockaddr *)&clientIPAddress, (socklen_t*)&alen);

		if(slaveSocket < 0) {
			perror("accept");
			exit(-1);
		}

		//when the thread ends resources are recycled
		pthread_t thread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&thread, &attr, (void * (*)(void *))processRequestThread, (void *) slaveSocket);

	}
}

void processRequestThread(int masterSocket) {
	processRequest(masterSocket);
	close(masterSocket);
}

void poolOfThreads(int masterSocket) {
	printf("Starting in pool of threads mode.\n");
	pthread_t thread[4];
	for(int i = 0; i < 4; i++) {
		pthread_create(&thread[i], NULL, (void * (*)(void *))loopthread, (void *) masterSocket);
	}
	loopthread(masterSocket);
}

void * loopthread (int masterSocket) {
	while(1) {
		//Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof(clientIPAddress);
		pthread_mutex_lock(&mutex);
		int slaveSocket = accept(masterSocket, (struct sockaddr *)&clientIPAddress, (socklen_t*)&alen);
		pthread_mutex_unlock(&mutex);		

		if(slaveSocket < 0) {
			perror("accept");
			exit(-1);
		}

		processRequest(slaveSocket);
		close(slaveSocket);

	}
}

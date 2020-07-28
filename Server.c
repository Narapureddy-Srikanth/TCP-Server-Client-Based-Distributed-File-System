#include<stdio.h>
#include<stdlib.h>
#include<sqlite3.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define SA struct sockaddr 

int count=0;
char username[8], password[8];
char *sql;
char rajeev[10000];

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    char string[1000];
    for(i=0;i<argc;i++){
        bzero(string,1000);
        sprintf(string,"%s = %s\n", azColName[i], argv[i] ? argv[i]: "NULL");
        strcat(rajeev,string);
    }
    strcat(rajeev,"\n");
    return 0;
}

static int callback_verify(void *NotUsed, int argc, char **argv, char **azColName){

        int i;
        for(i=0;i<argc;i++){
                count++;
        }
        return 0;
}

int USR_PASS(){

    bzero(username,8);
    bzero(password,8);
    read(sockfd,username,8);
    read(sockfd,password,8);

    sprintf(sql,"SELECT Id FROM LoginDetails WHERE UserName='%s' AND Password='%s';",username,password);
    count=0;
    sqlite3_exec(db,sql,callback_verify,0,&errmsg);

    if(count>0)
        return 1;
    else
        return 0;

}

int USR(){

    bzero(username,8);
    bzero(password,8);
    read(sockfd,username,8);
    read(sockfd,password,8);
    sprintf(sql,"SELECT Id FROM LoginDetails WHERE UserName='%s';",username);
    count=0;
    sqlite3_exec(db,sql,callback_verify,0,&errmsg);

    if(count>0)
        return 0;
    else{
        
        sprintf(sql, "INSERT INTO LoginDetails(UserName,Password) VALUES('%s','%s');",username, password);
        sqlite3_exec(db,sql,0,0,&errmsg);
        return 1;
    }
}





int main(int argc, char *argv[]){
	if(argc<2){
                fprintf(stderr, "Port No not provided. Program Terminated.\n");
                exit(1);
        }
	int sockfd, connfd, len; 
   	struct sockaddr_in servaddr, cli; 
  
    	// socket create and verification 
   	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    	if (sockfd == -1) { 
        	printf("socket creation failed...\n"); 
        	exit(0); 
    	}	 
    	else
        	printf("Socket successfully created..\n"); 
    	bzero(&servaddr, sizeof(servaddr)); 
  
    	// assign IP, PORT 
    	servaddr.sin_family = AF_INET; 
    	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    	servaddr.sin_port =htons(atoi(argv[1])); 
  
    	// Binding newly created socket to given IP and verification 
    	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        	printf("socket bind failed...\n"); 
        	exit(0); 
    	} 
    	else
        	printf("Socket successfully binded..\n"); 
  
    	// Now server is ready to listen and verification 
    	if ((listen(sockfd, 5)) != 0) { 
        	printf("Listen failed...\n"); 
        	exit(0); 
    	} 
    	else
        	printf("Server listening..\n"); 
    	len = sizeof(cli); 
  
    	// Accept the data packet from client and verification 
    	connfd = accept(sockfd, (SA*)&cli, &len); 
    	if (connfd < 0) { 
        	printf("server acccept failed...\n"); 
        	exit(0); 
    	} 
    	else{
        	printf("server acccept the client...\n"); 
		}

        sqlite3 *db;
        sqlite3_open("UserDetails.db",&db);
        char sql[1000];
        sprintf(sql, "CREATE TABLE LoginDetails(Id INTEGER PRIMARY KEY, UserName TEXT UNIQUE, Password TEXT);");
        sqlite3_exec(db,sql,0,0,&errmsg);

    	//finalfuction(connfd);

        char buffer[10];
        bzero(buffer,10)

        read(sockfd,buffer,10);

        if(strcmp(buffer,"VERIFY_USR")==0){
            if(USR_PASS()==1){
                printf("Login Successfull!\n");
            }else{
                printf("Incorrect username and password!");
                close(sockfd);
            }        
        }
        else if(strcmp(buffer,"UNIQUE_USR")==0){
            if(USR()==1){
                printf("Successfully Registered!\n");
            }else{
                printf("Already Registered!\n");
                close(sockfd);
            }


        }else{
            printf("Connection Closed!\n");    
            close(sockfd);        
        }
        
	return 0;
}
#include<stdio.h>
#include<sqlite3.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#define SA struct sockaddr 

void finalfunction(int sockfd){
	int choice;
	char buffer[1],rajeev[10000],username[100],password[100],string[100],cnfrmpassword[100],textfile[10000];
	while(1){
                printf("Press 1 for Login\n");
                printf("New User?, Press 2 for Sign Up\n");
                printf("Press 3 for Exit\n");
                scanf("%d",&choice);
		bzero(buffer,1);
		bzero(username,100);
		bzero(password,100);
		bzero(string,100);
		if(choice==1){
			printf("\t\t\t\t------------------Login-----------------\n");
			buffer[0]='1'; 
			write(sockfd,buffer,1);
			printf("User Name: ");
                        scanf("%s",username);
                        write(sockfd,username,strlen(username));
                        printf("Password: ");
                        scanf("%s",password);
                        write(sockfd,password,strlen(password));
			bzero(string,100);
			read(sockfd,string,100);
			printf("%s",string);
			if(strcmp(string,"Login successfull!\n")==0){
				bzero(rajeev,10000);
				read(sockfd,rajeev,10000);
				printf("%s",rajeev);
				read(sockfd,string,100);
				printf("%s",string);
				char a[1];
				scanf("%s",a);
				write(sockfd,a,1);
				if(a[0]=='0'){
                        		printf("Enter Username to be sent: ");
					bzero(string,100);
					scanf("%s",string);
                        		write(sockfd,string,sizeof(string));
					printf("Enter FileName: ");
					char string1[100];
                        		bzero(string1,100);
                        		scanf("%s",string1);
                        		write(sockfd,string1,sizeof(string));
					bzero(string,100);
					read(sockfd,string,100);
					if(strcmp(string,"File Sent.\n")==0){
						FILE *fp;
						fp=fopen(string1,"r");
						fscanf(fp,"%[^EOF]s",textfile);
						write(sockfd,textfile,10000);
						fclose(fp);
					}
					printf("File Sent!\n");	
				}
				else if(a[0]=='1'){
					bzero(string,100);
					printf("Enter FileName  you want to Download: ");																			    scanf("%s",string);
					write(sockfd,string,sizeof(string));
					bzero(rajeev,10000);
					read(sockfd,rajeev,10000);
					FILE *f;
					f=fopen(string,"w");
					fputs(rajeev,f);
					fclose(f);
					printf("File Downloaded!\n");
				}
				else{
                        		break;
				}
			}
		}
		else if(choice==2){
			printf("\t\t\t\t------------------Sign Up-----------------\n");
                        buffer[0]='2';
                        write(sockfd,buffer,1);
                        printf("User Name: ");
                        scanf("%s",username);
			write(sockfd,username,strlen(username));
                        printf("Password: ");
                        scanf("%s",password);
                        printf("Confirm Password: ");
                        scanf("%s",cnfrmpassword);
                        if(strcmp(password, cnfrmpassword)==0){
                                write(sockfd,password,strlen(password));
				read(sockfd,string,100);
				printf("%s",string);
                        }
                        else{
                                printf("Password and Confirm Password should be same.\nTry Again\n");
                        }
		}
		else{
		  	buffer[0]='3';
                        write(sockfd,buffer,1);
			break;
		}
	}
}

int main(int argc, char *argv[]){
	if(argc<3){

                fprintf(stderr, "usage %s hostname port\n",argv[0]);
                exit(0);
        }
	int sockfd, connfd; 
    	struct sockaddr_in servaddr, cli; 
 	struct hostent *server; 
    	// socket create and varification
	int portno=atoi(argv[2]); 
    	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    	if (sockfd == -1) { 
        	printf("socket creation failed...\n"); 
        	exit(0); 
    	} 
    	else
        	printf("Socket successfully created..\n"); 

	server=gethostbyname(argv[1]);

    	bzero(&servaddr, sizeof(servaddr)); 
  
    	// assign IP, PORT 
    	servaddr.sin_family = AF_INET; 
	bcopy((char*)server->h_addr,(char*) &servaddr.sin_addr.s_addr, server->h_length);
    	servaddr.sin_port = htons(portno); 
  
    	// connect the client socket to server socket 
    	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
       	 	printf("connection with the server failed...\n"); 
        	exit(0); 
    	}	 
    	else
        	printf("connected to the server..\n"); 
  
    	// function for chat 
    	finalfunction(sockfd); 
  
    	// close the socket 
    	close(sockfd); 	
	return 0;
}

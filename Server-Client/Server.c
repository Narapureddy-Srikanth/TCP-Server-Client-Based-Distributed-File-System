#include<stdio.h>
#include<stdlib.h>
#include<sqlite3.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#define SA struct sockaddr

char rajeev[10000];
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	int i;
	char string[1000];
	for (i = 0; i < argc; i++) {
		bzero(string, 1000);
		sprintf(string, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		strcat(rajeev, string);
	}
	strcat(rajeev, "\n");
	return 0;
}

int count = 0;
static int callback_verify(void *NotUsed, int argc, char **argv, char **azColName) {

	int i;
	for (i = 0; i < argc; i++) {
		count++;
	}
	return 0;
}


void finalfuction(int sockfd) {
	sqlite3 *db;
	sqlite3_open("UserDetails.db", &db);
	char sql[1000];
	sprintf(sql, "CREATE TABLE LoginDetails(Id INTEGER PRIMARY KEY, UserName TEXT UNIQUE, Password TEXT);");
	char sql1[1000];
	sprintf(sql1, "CREATE TABLE Graph(Id INTEGER PRIMARY KEY, Sender TEXT,Reciver TEXT,Title TEXT,File TEXT);");
	char *errmsg = 0;
	sqlite3_exec(db, sql, 0, 0, &errmsg);
	sqlite3_exec(db, sql1, 0, 0, &errmsg);
	char buffer[1], username[100], password[100], string[100], reciver[100], sender[100], textfile[10000], filename[100];
	while (1) {
		bzero(buffer, 1);
		bzero(username, 100);
		bzero(password, 100);
		bzero(string, 100);
		bzero(sender, 100);
		bzero(reciver, 100);
		bzero(textfile, 10000);
		bzero(filename, 100);
		bzero(sql, 1000);
		bzero(sql1, 1000);
		read(sockfd, buffer, 1);
		if (buffer[0] == '1') {
			read(sockfd, username, 100);
			read(sockfd, password, 100);
			//encrypt(username);
			//encrypt(password);
			sprintf(sql, "SELECT Id FROM LoginDetails WHERE UserName='%s' AND Password='%s';", username, password);
			count = 0;
			sqlite3_exec(db, sql, callback_verify, 0, &errmsg);
			if (count > 0) {
				sprintf(string, "Login successfull!\n");
				write(sockfd, string, 100);
				sprintf(sql1, "SELECT Id,Sender,Title FROM Graph WHERE Reciver='%s';", username);
				sqlite3_exec(db, sql1, callback, 0, &errmsg);
				write(sockfd, rajeev, sizeof(rajeev));
				strcpy(string, "Press 0 for sending a file to Server\nPress 1 to extract a file\nPress 2 for Exit \n");
				write(sockfd, string, sizeof(string));
				bzero(string, 100);
				read(sockfd, string, 1);
				if (string[0] == '0') {
					bzero(string, 100);
					read(sockfd, reciver, 100);
					read(sockfd, filename, 100);
					sprintf(sql, "SELECT Id FROM LoginDetails WHERE UserName='%s';", reciver);
					count = 0;
					sqlite3_exec(db, sql, callback_verify, 0, &errmsg);
					if (count > 0) {
						sprintf(sql, "SELECT Id FROM Graph WHERE Reciver='%s' AND Title='%s';", reciver, filename);
						count = 0;
						sqlite3_exec(db, sql, callback_verify, 0, &errmsg);
						if (count > 0) {
							bzero(string, 100);
							sprintf(string, "File is already present in Database.\n");
							write(sockfd, string, sizeof(string));
						}
						else {

							sprintf(string, "File Sent.\n");
							write(sockfd, string, sizeof(string));
							read(sockfd, textfile, 10000);
							bzero(sql1, 1000);
							sprintf(sql1, "INSERT INTO Graph(Sender,Reciver,Title,File) VALUES ('%s','%s','%s','%s');", username, reciver, filename, textfile);
							sqlite3_exec(db, sql1, 0, 0, &errmsg);
							bzero(string, 100);
						}
					}
					else {
						bzero(string, 100);
						sprintf(string, "Reciver Does not exist.\n");
						write(sockfd, string, sizeof(string));

					}

				}
				else if (string[0] == '1') {
					bzero(string, 100);
					read(sockfd, string, 100);
					bzero(sql1, 1000);
					bzero(rajeev, 10000);
					sprintf(sql1, "SELECT File FROM Graph WHERE Title='%s';", string);
					sqlite3_exec(db, sql1, 0, 0, &errmsg);
					printf("%s", rajeev);
					write(sockfd, rajeev, sizeof(rajeev));
				}
				else {
					break;
				}
			}
			else {
				sprintf(string, "Username or password not exist!\n");
				write(sockfd, string, strlen(string));
			}
		}
		else if (buffer[0] == '2') {
			read(sockfd, username, 100);
			read(sockfd, password, 100);
			//encrypt(username);
			//encrypt(password);
			sprintf(sql, "SELECT Id FROM LoginDetails WHERE UserName='%s';", username);
			count = 0;
			int rcc = sqlite3_exec(db, sql, callback_verify, 0, &errmsg);
			if (count > 0) {
				sprintf(string, "Username already exist!\n");
				write(sockfd, string, strlen(string));
			}
			else {
				sprintf(sql, "INSERT INTO LoginDetails(UserName,Password) VALUES('%s','%s');", username, password);
				sqlite3_exec(db, sql, 0, 0, &errmsg);
				sprintf(string, "Successfully Signed Up\n");
				write(sockfd, string, strlen(string));
			}
		}
		else {
			break;
		}
	}
}


int main(int argc, char *argv[]) {
	if (argc < 2) {
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
	servaddr.sin_port = htons(atoi(argv[1]));

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
	else {
		printf("server acccept the client...\n");
	}

	// Function for client and server
	finalfuction(connfd);
	// After chatting close the socket
	close(sockfd);
	return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include<fcntl.h>
#include<sys/stat.h>

#define BUFSIZE		4096
int isQuiz;
int connectsock( char *host, char *service, char *protocol );

/*
 **    Client
 */

void *writeToServer(void *args){
    char buf[BUFSIZE];
    int csock = (int) args;
    //     Start the loop
    while ( fgets( buf, BUFSIZE, stdin ) != NULL )
    {
        // If user types 'q' or 'Q', end the connection
    
    if ( write( csock, buf, strlen(buf) ) < 0 )
    {
        fprintf( stderr, "client write: %s\n", strerror(errno) );
        exit( -1 );
    }
        if ( buf[0] == 'q' || buf[0] == 'Q' )
        {
            exit(-1);
        }
    }
    pthread_exit(NULL);
}

void *readFromSercer(void *args){
    int csock = (int) args;
    char buf[BUFSIZE];
    int cc;
    
    while(1)
    {
        //printf( "Start reading\n" );
        
    if ( (cc = read( csock, buf, BUFSIZE )) <= 0 )
    {
        printf( "The server has gone.\n" );
        close(csock);
        exit(-1);
    }
    else
    {
        char quiz[100] = "";
        buf[cc] = '\0';
        printf( "%s\n", buf );
//        if(strcmp(buf, "SENDQUIZ\r\n")==0){
//            printf( "Start quiz\n" );
//            fflush(stdout);
//            int tempFile;
//            if((tempFile = open("quiz.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | O_APPEND)) < -1){
//                exit(-1);
//            }
//
//            long size = lseek(tempFile, 0, SEEK_END); //in order to write to the end of file
//            lseek(tempFile, 0, SEEK_SET);
//            sprintf(quiz, "QUIZ|%li|", size);
//            if ( write( csock, quiz, strlen(quiz)) < 0 )
//            {
//                fprintf( stderr, "client write: %s\n", strerror(errno) );
//                exit( -1 );
//            }
//
//            while(size > 0){
//                printf( "Send questions\n" );
//                read(tempFile, buf, 8);
//                if ( write( csock, buf, 8 ) < 0 )
//                {
//                    fprintf( stderr, "client write: %s\n", strerror(errno) );
//                    exit( -1 );
//                }
//                //lseek(tempFile, 0, 8);
//                size++;
//            }
//            //printf( "%s", begin); //print prompt
//
//            //isQuiz = 1;
//            close(tempFile);
//        }
    }
    
    }
    pthread_exit(NULL);
}

int
main( int argc, char *argv[] )
{
    char        buf[BUFSIZE];
    char        *service;
    char        *host = "localhost";
    int        cc;
    int        csock;
    pthread_t thread[2];
    int status1, status2;
    
    switch( argc )
    {
        case    2:
            service = argv[1];
            break;
        case    3:
            host = argv[1];
            service = argv[2];
            break;
        default:
            fprintf( stderr, "usage: chat [host] port\n" );
            exit(-1);
    }
    
    /*    Create the socket to the controller  */
    if ( ( csock = connectsock( host, service, "tcp" )) == 0 )
    {
        fprintf( stderr, "Cannot connect to server.\n" );
        exit( -1 );
    }
    
    printf( "The server is ready, please start sending to the server.\n" );
    printf( "Type q or Q to quit.\n" );
    fflush( stdout );

        // Send to the server
        status1 = pthread_create(&thread[0], NULL, writeToServer, (void *)csock);
        if ( status1 != 0 )
        {
            printf( "pthread_create error %d.\n", status1 );
            exit( -1 );
        }
        
        // Read the echo and print it out to the screen
    
        status2 = pthread_create(&thread[1], NULL, readFromSercer, (void *)csock);
        if ( status2 != 0 )
        {
            printf( "pthread_create error %d.\n", status2 );
            exit( -1 );
        }
    for (int j = 0; j < 2; j++ )
        pthread_join( thread[j], NULL );
    close( csock );
    pthread_exit( 0 );
}


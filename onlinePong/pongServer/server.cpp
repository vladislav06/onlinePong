// server.c
#include <enet/enet.h>
#include <stdio.h>
#include <iostream>
#include <random>
#include <time.h>
#include "../vector.h"

using namespace std;
using namespace VectMath;

#define LOOP_TIME 30 //ms

#define X_WALL 490
#define Y_WALL 990
#define RACK_H 30

struct user {
    bool occuped = false;
    int id;
    ENetAddress address;
};



struct ball {
    VectMath::vector2::vector vect;
    int x, y = 0;
};

struct data {
    int h1, h2 = 0 ; // use 8 for h of racket
    struct ball ball;  //struct ball use 16 bytes
    uint8_t id;     //one byte for id
                    // we have 3 bytes left for future
};



VectMath::vector2::vector  ballInit(float angle);
int main(int argc, char** argv)
{
    
    clock_t tick;
    user users[2];  //users registration


    struct data dataRecive;

    struct data dataSend;
    float senddata[7];
    struct ball ball;

    VectMath::vector2::vector ballcord;

    


    ENetAddress address;
    ENetHost* server;
    ENetEvent event;
    ENetPacket* packet;
    int data[1];
    //int tmp[1];
    int eventStatus;
    float recivedata[7];
    // a. Initialize enet
    if (enet_initialize() != 0) {
        fprintf(stderr, "An error occured while initializing ENet.\n");
        return EXIT_FAILURE;
    }

    atexit(enet_deinitialize);

    // b. Create a host using enet_host_create
    enet_address_set_host(&address, "192.168.88.245");
   
    address.port = 1234;

    server = enet_host_create(&address, 32, 2, 0, 0);

    if (server == NULL) {
        fprintf(stderr, "An error occured while trying to create an ENet server host\n");
        exit(EXIT_FAILURE);
    }

    // c. Connect and user service
    eventStatus = 1;

    ball.x = 500;
    ball.y = 250;
    ball.vect = ballInit(rand() % 360);

    while (1) {
        tick = clock(); //get tick before loop

        eventStatus = enet_host_service(server, &event,0);

        // If we had some event that interested us
        if (eventStatus > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                printf("(Server) We got a new connection from %x\n",
                    event.peer->address.host);
                for (int i = 0; i < 2; i++)
                {
                    if (!users[i].occuped)//if user is free
                    {       
                        users[i].occuped = true;//say that user is occupied 
                        users[i].id = i; //sed user id
                        users[i].address = event.peer->address;   //set user adres to identify it
                        data[0] = i;       //send id to user

                        packet = enet_packet_create(data, //send it
                            sizeof(data),
                            ENET_PACKET_FLAG_RELIABLE);

                        enet_peer_send(event.peer, 0, packet);
                        enet_host_flush(server);

                        cout << i << endl; //print id
                        break;
                    }
                }   
                

                break;

            case ENET_EVENT_TYPE_RECEIVE:
                printf("(Server) Message  ");
                memcpy(&recivedata, event.packet->data, event.packet->dataLength);

                dataRecive.id = recivedata[0];
                dataRecive.h1 = recivedata[1];
                dataRecive.h2 = recivedata[2];
                dataRecive.ball.x = recivedata[3];
                dataRecive.ball.y = recivedata[4];
                dataRecive.ball.vect.x = recivedata[5];
                dataRecive.ball.vect.y = recivedata[6];

                std::cout<<"id: "<<int(dataRecive.id)<<"    h1: "<<int(dataRecive.h1)<< endl;
                // Lets broadcast this message to all
                // enet_host_broadcast(server, 0, event.packet);
                enet_packet_destroy(event.packet);


                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("%s disconnected.\n", event.peer->data);
                for (int i = 0; i < 2; i++)
                {
                    if (users[i].address.host == event.peer->address.host) //if user id and id of disconected user is equal
                    {
                        users[i].occuped = false; //say that user is free
                        break; 
                    }
                }
                // Reset client's information
              //  event.peer->data = NULL;
                break;

            }




        }
        tick = clock() - tick; //get ticks after loop

        /* <---------clamp ball to y(vertical) of raket---------><clamp to x(horizont) of racket>  */
        if (ball.y <= dataRecive.h1 + RACK_H && ball.y >= dataRecive.h1 && ball.x >= 100 && ball.x <= 105) { ball.vect.x *= -1; }
        if (ball.y <= dataRecive.h2 + RACK_H && ball.y >= dataRecive.h2 && ball.x >= 900 && ball.x <= 905) { ball.vect.x *= -1; }

        cout << ball.x << "    " << ball.y << endl;
        if (ball.x >= Y_WALL || ball.x <= 0) ball.vect.x *= -1; //if ball gets to the wall invert vector
        if (ball.y >= X_WALL || ball.y <= 0) ball.vect.y *= -1;

        ballcord.x = ball.x; //convert ball.x,y to vector for future calculations
        ballcord.y = ball.y;
        /*if we divide ticks to looptime we get difrenece coef wich shows vector lenght coef */
        ballcord = vector2::sum(ballcord, vector2::mult(ball.vect, tick / LOOP_TIME));    //ball.x,y += (ball.vect*tick/loop_time)
        ball.x = ballcord.x;//deconvert ballcord to ball.x,y
        ball.y = ballcord.y;

        
        dataSend.ball = ball;
        dataSend.id = -1; //set id of server 
        dataSend.h1 = dataRecive.h1;
        dataSend.h2 = dataRecive.h1;


        senddata[0] = dataSend.id;
        senddata[1] = dataSend.h1;
        senddata[2] = dataSend.h2;
        senddata[3] = dataSend.ball.x;
        senddata[4] = dataSend.ball.y;
        senddata[5] = dataSend.ball.vect.x;
        senddata[6] = dataSend.ball.vect.y;



        packet = enet_packet_create(&senddata, //send it
            sizeof(senddata)+1,
            ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(server, 0, packet);
        
        enet_host_flush(server);
    }

}


vector2::vector  ballInit(float angle) {
    VectMath::vector2::vector vector;
    int speed = 2;
    vector.x = speed * sin(angle);  // a = c sin(alpha)
    vector.y = speed * cos(angle);  //  b = c cos(alpha)
    return vector;
}
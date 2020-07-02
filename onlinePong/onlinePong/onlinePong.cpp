#pragma comment( lib, "wsock32.lib" )
#include <sdl.h>
#include <cstdio>
#include <iostream>
#include <string>
#include "../vector.h"
#include "../pongServer/UDP.h"

#define TICK_INTERVAL    30 //update rate
static Uint32 next_time;


#define X_WALL 490
#define Y_WALL 990
#define RACK_H 30

using namespace std;
using namespace VectMath;

struct ball {
    VectMath::vector2::vector vect;
    int x, y = 0;
};

struct data {
    int h1, h2 = 0; // use 8 for h of racket
    struct ball ball;  //struct ball use 16 bytes
    uint8_t id = 0;     //one byte for id
                    // we have 3 bytes left for future
};

struct user {
    bool occuped = false;
    int id;
    Address address;
};


int SDLCheck(SDL_Window* window, SDL_Renderer* render) {
    //check all sdl components
    if (window == nullptr) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return 1;
    }
   
    
    if (render == nullptr) {
        SDL_DestroyWindow(window);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    return 0;
}

void SDLDestroy(SDL_Window* window, SDL_Renderer* render) {
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Uint32 time_left(void)
{
    Uint32 now;

    now = SDL_GetTicks();
    if (next_time <= now)
        return 0;
    else
        return next_time - now;
}

int main(int argc, char** argv)
{



    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS)) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("pong", 100, 100, 1000, 500, SDL_WINDOW_SHOWN);
    SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (SDLCheck(window, render)) {
        std::cout << "SDL error\n";
        SDLDestroy(window, render);
    }
    //create rockets
    SDL_Rect racket1;
    SDL_Rect racket2;

    racket1.w = 5;
    racket1.h = RACK_H;

    racket2.w = 5;
    racket2.h = RACK_H;

    //init ball
    SDL_Rect ballS;
    ballS.w = 10;
    ballS.h = 10;
    ballS.x = 250;
    ballS.y = 500;

    struct ball ball;

    ball.x = 250;
    ball.y = 250;

    ball.vect.x = -2;
    ball.vect.y = 2;

    vector2::vector ballcord;

    SDL_Event e;
   
    /*==================sdl setup done=================*/
    
    Socket socket;

    struct data data;
    //struct data reciveData;

    struct user user;

    float senddata[7];
    float reciveData[7];
    for (int i = 0; i < 7; i++)
    {
        senddata[i] = 0;
    }
  
    bool server = false;

 
    int id[1];
   


    

  
    //get port and ip
    int ip1;
    int ip2;
    int ip3;
    int ip4;
    int port;
    std::cout << "enter server ip (-1 to be host) : ";
    cin >> ip1;
    cin >> ip2;
    cin >> ip3;
    cin >> ip4;
    std::cout << endl << "ender port: ";
    cin >> port;

    //open socket

    socket.Open(port);

    
    if (ip1 == -1) {       //if we are host
        char buf[256];
        Address sender;
        server = true;
        while (1)       //wait for a client
        {
            int bytes_read = socket.Receive(sender, buf, sizeof(buf));

            if (bytes_read >= 0)
            {
                std::cout << "got conection from : " << std::endl;
                std::cout << "ip: " <<
                    int(sender.GetA()) << "." <<
                    int(sender.GetB()) << "." <<
                    int(sender.GetC()) << "." <<
                    int(sender.GetD()) <<
                    " port:" <<
                    sender.GetPort() <<
                    std::endl;
                // set user
                user.address = sender;
                user.id = 1;
                user.occuped = true;

                break;
            }

        }

    } else {

        Address send;
        send.SetAddress(ip1,ip2,ip3,ip4);
        send.SetPortU(port);
        user.address = send;
        const char data[] = "hi";
        std::cout << "ip: " <<
            int(user.address.GetA()) << "." <<
            int(user.address.GetB()) << "." <<
            int(user.address.GetC()) << "." <<
            int(user.address.GetD()) <<
            " ip2 :" <<
            user.address.GetAddress() <<
            " port:" <<
            user.address.GetPort() <<
            std::endl;
        if (!socket.Send(user.address, data, sizeof(data)))
        {
            std::cout << "errr";
            // return false;
        }
    }
    
    
   
    

   // eventStatus = 1;
    bool quit = false;
    next_time = SDL_GetTicks() + TICK_INTERVAL;

    Address sender;

    racket1.x = 100;
    racket1.y = 50;
    racket2.x = 900;
    racket2.y = 50;

 //   racket1.
    if (!server) {
        int bytes_read = socket.Receive(sender, reciveData, sizeof(reciveData));
    }
    
    bool useData = false;
    while (1) {
        socket.NonBlock();
        
        int bytes_read = socket.Receive(sender, reciveData, sizeof(reciveData));
        
        if (bytes_read <= 0)
        {
            useData = false;
        }
        else {
            useData = true;
        }
      
      
        if (SDL_PollEvent(&e)) {
            //If user closes the window
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            //If user presses any key
            if (e.type == SDL_KEYDOWN) {

            }
            //If user clicks the mouse
            if (e.type == SDL_MOUSEBUTTONDOWN) {

            }
        }
        if (quit) {
            SDLDestroy(window, render);
            break;  //exit from loop
        }

        const Uint8* key = SDL_GetKeyboardState(NULL);
        if (key[SDL_SCANCODE_UP])
        {
            // cout << "up";
            racket1.y -= 10;  //move up
        }
        if (key[SDL_SCANCODE_DOWN])
        {
            racket1.y += 10;    //move down
            //cout << "down";
        }
        
        racket2.y = reciveData[1];
        if (!server) {

            //if (useData) {
              //  racket1.y = reciveData[2];
            //}

            ball.x = 1000 - reciveData[3];
            ball.y = reciveData[4];
            ball.vect.x = reciveData[5];
            ball.vect.y = reciveData[6];
        }
        


        /* <---------clamp ball to y(vertical) of raket---------><clamp to x(horizont) of racket>  */
        if (ball.y >= racket1.y && ball.y <= racket1.y + RACK_H && ball.x >= 100 && ball.x <= 105) { ball.vect.x *= -1; }

        if (ball.y <= racket2.y + RACK_H && ball.y >= racket2.y && ball.x >= 900 && ball.x <= 905) { ball.vect.x *= -1; }


        if (ball.x >= Y_WALL || ball.x <= 0) { ball.vect.x *= -1; }     //if ball gets to the wall invert vector
        if (ball.y >= X_WALL || ball.y <= 0) { ball.vect.y *= -1; }

        ballcord.x = ball.x;    //convert ball.x,y to vector for future calculations
        ballcord.y = ball.y;
        /*if we divide ticks to looptime we get difrenece coef wich shows vector lenght coef */
        ballcord = vector2::sum(ballcord, ball.vect);    //ball.x,y += ball.vect
        ball.x = ballcord.x;    //deconvert ballcord to ball.x,y
        ball.y = ballcord.y;


        ballS.x = ball.x;
        ballS.y = ball.y;


        //rendering
        SDL_SetRenderDrawColor(render,255,255,255,255);
        SDL_RenderClear(render);

        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);


        SDL_RenderFillRect(render, &racket1);
        SDL_RenderFillRect(render, &racket2);

        SDL_RenderFillRect(render, &ballS);

        SDL_RenderPresent(render);

        data.h1 = racket1.y;
        data.h2 = racket2.y;
        data.id = id[0];
       //cout << int(data.id)<<endl;


        senddata[0] = data.id;
        senddata[1] = data.h1;
        senddata[2] = data.h2;
        senddata[3] = ball.x;
        senddata[4] = ball.y;
        senddata[5] = ball.vect.x;
        senddata[6] = ball.vect.y;

        if (true) {
            
            if (!socket.Send(user.address, senddata, sizeof(senddata)))
            {
                std::cout << "errror";
            }
          
        }

        
       




       





        //event = NULL;


        SDL_Delay(time_left());
        next_time += TICK_INTERVAL; //calc next game update 
       

    }
    return 0;
}
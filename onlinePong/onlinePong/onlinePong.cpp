#pragma comment( lib, "wsock32.lib" )
#include <sdl.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include "../vector.h"
#include "../pongServer/UDP.h"
#include <math.h>
#include <time.h>
#include <sstream>
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
    int h1v, h2v = 0;
    struct ball ball;  //struct ball use 16 bytes
    uint8_t id = 0;     //one byte for id
                    // we have 3 bytes left for future
};

struct user {
    bool occuped = false;
    int id;
    Address address;
};
struct counters {
    int i;
};
struct settings {
    int BallSpeed;
    int RacketSpeed;
    bool MLGMode;
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


Uint32 time_left(void)
{
    Uint32 now;

    now = SDL_GetTicks();
    if (next_time <= now)
        return 0;
    else
        return next_time - now;
}

bool Overlap(SDL_Rect rect ,ball ball);

struct settings FileRead(string filename);
struct settings settings;
int main(int argc, char** argv)
{
   
    struct counters counters;
    Socket socket;

    struct data data;
    //struct data reciveData;

    struct user user;

    float sendData[9];
    float reciveData[9];
    for (int i = 0; i < 9; i++)
    {
        sendData[i] = 0;
        reciveData[i] = 0;
    }

    bool server = false;

    settings = FileRead("settings.dat");

    int id[1];

    //get port and ip
    int ip1;
    int ip2;
    int ip3;
    int ip4;
    int port;
    std::cout << "enter server ip (-1 to be host) : ";
    cin >> ip1;
    if (ip1 == -1)
    {
        goto port;
    }
    cin >> ip2;
    cin >> ip3;
    cin >> ip4;

port:

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

    }
    else {

        Address send;
        send.SetAddress(ip1, ip2, ip3, ip4);
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

    /*==================UDP setup done=================*/


    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS)) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("pong", 100, 100, 1000, 500, SDL_WINDOW_SHOWN);
    SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (SDLCheck(window, render)) {
        std::cout << "SDL error\n";
       // SDL_DestroyTexture(DVD);
        SDL_DestroyRenderer(render);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    SDL_Texture* DVD = nullptr;
    if (settings.MLGMode)
    {//loading image

        SDL_Surface* bmp = SDL_LoadBMP("DVD.bmp");
        if (bmp == nullptr) {
            std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
            return 1;
        }

       DVD = SDL_CreateTextureFromSurface(render, bmp);
        SDL_FreeSurface(bmp);
        if (DVD == nullptr) {
            std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
            return 1;
        }
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
    if (settings.MLGMode)
    {
        ballS.w = 50;
        ballS.h = 50;
    }
    ballS.x = 250;
    ballS.y = 500;

    struct ball ball;

    ball.x = 250;
    ball.y = 250;

    ball.vect.x = -settings.BallSpeed;
    ball.vect.y = settings.BallSpeed;

    vector2::vector ballcord;

    SDL_Event e;
   
    /*==================sdl setup done=================*/
    
   
    
    
   
    

   // eventStatus = 1;

    bool quit = false;

    next_time = SDL_GetTicks() + TICK_INTERVAL;

    Address sender;
    //set cords 
    racket1.x = 100;
    racket1.y = 250;
    racket2.x = 900;
    racket2.y = 250;

 //   racket1.
    if (!server) {
        int bytes_read = socket.Receive(sender, reciveData, sizeof(reciveData));
    }
    
    bool useData = false;
    counters.i = 0;

    int tick;

   

        socket.NonBlock(); //disable waiting
    
    while (!quit) {
        // render last frame for lower lags
        SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
        SDL_RenderClear(render);

        SDL_SetRenderDrawColor(render, 0, 0, 0, 255);


        SDL_RenderFillRect(render, &racket1);
        SDL_RenderFillRect(render, &racket2);

        if (settings.MLGMode) {
            //SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
            SDL_RenderCopy(render, DVD, NULL,&ballS);
        }
        else {
            SDL_RenderFillRect(render, &ballS);
        }

        

        SDL_RenderPresent(render);

        tick = clock(); //get tick before loop
      //  counters.i++;

        // if we are server, we listen all data what is incoming to get latest update
        if (server) {
            int bytes_read = 1;

            while (bytes_read > 0) {
                counters.i++;
                bytes_read = socket.Receive(sender, reciveData, sizeof(reciveData));

            }
            cout << counters.i << "   |   "; //print reciving loops count
            counters.i = 0;
            //check for data
            if (bytes_read <= 0)
            {
                useData = false;
            }
            else {
                useData = true;

            }

        }
        else {
            //if we are client simply read data 
           int bytes_read = socket.Receive(sender, reciveData, sizeof(reciveData));
        }
        
        //check for events
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
        //if (quit) {
          //  SDLDestroy(window, render);
          //  break;  //exit from loop
        //}

        const Uint8* key = SDL_GetKeyboardState(NULL);
        if (key[SDL_SCANCODE_UP])
        {
            // cout << "up";
            data.h1v = -settings.RacketSpeed; //move up
        }
        if (key[SDL_SCANCODE_DOWN])
        {
            data.h1v = settings.RacketSpeed;    //move down
            //cout << "down";
        }
        //===========+ update +===========
        //transform recived data
      

        racket2.y = reciveData[1];

        data.ball.x = reciveData[3];
        data.ball.y = reciveData[4];

        data.ball.vect.x = reciveData[5];
        data.ball.vect.y = reciveData[6];

    
        data.h2v = reciveData[7];
 

        
        if (!server) { //update if we client


            ball.x = 1000 - data.ball.x;  //invert ball position
            ball.y = data.ball.y;
            ball.vect.x = data.ball.vect.x * -1; //and vector
            ball.vect.y = data.ball.vect.y;

            //counters.i = 0;

        }

        if (Overlap(racket1,ball)) { ball.vect.x *= -1; } //if bal hits racket then invert x vector
        if (Overlap(racket2, ball)) { ball.vect.x *= -1; }


        if ((ball.x+ballS.w) > Y_WALL || ball.x < 0) { ball.vect.x *= -1; }     //if ball hits to the wall invert vector
        if ((ball.y+ballS.h) > X_WALL || ball.y < 0) { ball.vect.y *= -1; }

        if (racket1.y > X_WALL) { data.h1v = 0; racket1.y--; }
        if (racket2.y > X_WALL) { data.h2v = 0; racket2.y--; }

        if (racket1.y < 0) { data.h1v = 0; racket1.y = 0; }
        if (racket2.y < 0) { data.h2v = 0; racket2.y = 0; }
        //update ball position

        ballcord.x = ball.x;    //convert ball.x,y to vector for future calculations
        ballcord.y = ball.y;
        /*if we divide ticks to looptime we get difrenece coef wich shows vector lenght coef */
        ballcord = vector2::sum(ballcord, ball.vect);    //ball.x,y += ball.vect
        ball.x = ballcord.x;    //deconvert ballcord to ball.x,y
        ball.y = ballcord.y;
        //update ball render model
        ballS.x = ball.x;
        ballS.y = ball.y;
        //update racket position
        racket1.y += data.h1v;
        racket2.y += data.h2v;
        data.h1v = 0;
        data.h2v = 0;


        //set our racket cords
        data.h1 = racket1.y;

      
        //prepare for transmission
        sendData[0] = data.id;
        sendData[1] = data.h1;
        sendData[2] = data.h2;
        sendData[3] = ball.x;
        sendData[4] = ball.y;
        sendData[5] = ball.vect.x;
        sendData[6] = ball.vect.y;
        sendData[7] = data.h1v;
        sendData[8] = data.h2v;


        if (true) {
            
            if (!socket.Send(user.address, sendData, sizeof(sendData)))
            {
                std::cout << "errror";
            }
          
        }
       
        
       

        //event = NULL;
        tick = clock() - tick; //get ticks after loop
        cout << tick << endl; //print ticks
        if (tick >= 20)
        {
            socket.Close();
            socket.Open(port);
            socket.NonBlock(); //disable waiting

        }
        SDL_Delay(10);

    }

    SDL_DestroyTexture(DVD);
    
   
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
   // SDLDestroy(window, render);

    return 0;
}

struct settings FileRead(string filename) {

    struct settings settings;

    string line;
    string set;
    string dat;

    ifstream file("settings.dat");
    
    if (file.is_open())
    {
        while (getline(file, line))
        {
            istringstream iss(line);



            if (!(iss >> set >> dat)) {

                break;
            }
            else {


                if (set == "BallSpeed") {

                    settings.BallSpeed = stoi(dat); //set value
                    cout << "BallSpeed : " << settings.BallSpeed << endl;

                }
                else if (set == "RacketSpeed") {

                    settings.RacketSpeed = stoi(dat);
                    cout << "RacketSpeed : " << settings.RacketSpeed << endl;
                }
                else if (set == "MLGMode") {
                    if (stoi(dat) == 1)
                    {
                        settings.MLGMode = true;
                        cout << "MLGMode : " << settings.MLGMode << endl;
                    }
                    else {
                        settings.MLGMode = false;
                        cout << "MLGMode : " << settings.MLGMode << endl;
                    }
                }
            }
        }
        
        file.close();
    }
    else {
        cout << "file read err" << endl;
    }

    return settings;

}
bool Overlap (SDL_Rect rect , ball ball){
    int balh;
    if (settings.MLGMode) {
        balh = 50;
    }
    else {
        balh = 10;
    }
    
    if (rect.x > (ball.x + balh) || ball.x > (rect.x + rect.w)) {
        return false;
    }
    if (rect.y > (ball.y + balh) || ball.y > (rect.y + rect.h)) {
        return false;
    }

    return true;
}
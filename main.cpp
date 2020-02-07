#include "common.h"
#include "cmath"
#include "vector"
#include "string"

bool Init();
void CleanUp();
void Run();
void Draw();
void Connect(int i, int j, vector<vector<double>> pps);
void Rots();
vector<vector<double>> MultMatrixs(vector<vector<double>> mat1, vector<vector<double>> mat2);

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

int screenWidth = 500;
int screenHeight = 500;
double x = 1;
double y = 1;
double z = 1;
double o = 19;
double a = 28;
double b = 8/3;
double dt = .01;
double xang = -.82;
double yang = -.93;
double zang = -.93;
double zoom = 7;
double xoff = 0;
double yoff = -25;

vector<vector<double>> points;
vector<vector<double>> rotx;
vector<vector<double>> roty;
vector<vector<double>> rotz;
vector<vector<double>> projection;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));

    int time = clock();

    Rots();
    projection.push_back({1, 0, 0});
    projection.push_back({0, 1, 0});
    
    while (gameLoop)
    {   
        int ctime = clock();
    
        // if(ctime - time > 17){
            pos.x = 0;
            pos.y = 0;
            pos.w = screenWidth;
            pos.h = screenHeight;
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &pos);
            time = clock();

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            Draw();
            SDL_RenderPresent(renderer);
        //}

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    case SDLK_w:
                        xang += .03;
                        break;
                    case SDLK_s:
                        xang -= .03;
                        break;
                    case SDLK_a:
                        yang += .03;
                        break;
                    case SDLK_d:
                        yang -= .03;
                        break;
                    case SDLK_q:
                        zang += .03;
                        break;
                    case SDLK_e:
                        zang -= .03;
                        break;
                    case SDLK_UP:
                        yoff += .1;
                        break;
                    case SDLK_DOWN:
                        yoff -= .1;
                        break;
                    case SDLK_LEFT:
                        xoff += .1;
                        break;
                    case SDLK_RIGHT:
                        xoff -= .1;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    default:
                        break;
                }
            }
        }
    }
}

void Draw(){
    double dx = a*(y - x);
    x += dx * dt;
    double dy = x*(a - z) - y;
    y += dy * dt;
    double dz = x*y - b*z;
    z += dz * dt;

    points.push_back({x, y, z});
    if(points.size() > 200){
        points.erase(points.begin());
    }

    vector<vector<double>> pps;
    vector<vector<double>> xyz;
    vector<double> temp;
    Rots();
    for(int i = 0; i < points.size(); i++){
        xyz.clear();
        temp.clear();
        for(int j = 0; j < points[i].size(); j++){
            xyz.push_back({points[i][j]});
        }
        vector<vector<double>> rotated = MultMatrixs(roty, xyz);
        rotated = MultMatrixs(rotx, rotated);
        rotated = MultMatrixs(rotz, rotated);
        vector<vector<double>> xys = MultMatrixs(projection, rotated);

        for(int j = 0; j < xys.size(); j++){
            temp.push_back(xys[j][0]);
        }
        pps.push_back(temp);

        // pos.x = (pps[i][0] + xoff) * zoom + screenWidth / 2;
        // pos.y = (pps[i][1] + yoff)* zoom + screenHeight / 2;
        // pos.w = 1;
        // pos.h = 1;
        // SDL_RenderFillRect(renderer, &pos);
    }
    if(pps.size() > 1){
        for(int i = 1; i < pps.size(); i++){
            Connect(i, i-1, pps);
        }
    }
}

void Connect(int i, int j, vector<vector<double>> pps){
    int ix = (pps[i][0]) * zoom + screenWidth / 2;
    int iy = (pps[i][1] + yoff) * zoom + screenHeight / 2;
    int jx = (pps[j][0]) * zoom + screenWidth / 2;
    int jy = (pps[j][1] + yoff) * zoom + screenHeight / 2;
    SDL_RenderDrawLine(renderer, ix, iy, jx, jy);
}

vector<vector<double>> MultMatrixs(vector<vector<double>> mat1, vector<vector<double>> mat2){
    vector<vector<double>> result;
    vector<double> temp;
    double a = 0;
    for(int j = 0; j < mat1.size(); j++){
        for(int k = 0; k < mat2[0].size(); k++){
            for(int i = 0; i < mat1[j].size(); i++){
                a+= mat1[j][i] * mat2[i][k];
            }
            temp.push_back(a);
            a = 0;
        }
        result.push_back(temp);
        temp.clear();
    }
    return result;
}

void Rots(){
    vector<double> temp;
    rotx.clear();
    roty.clear();
    rotz.clear();

    rotx.push_back({1, 0, 0});
    rotx.push_back({0, cos(xang), -1*sin(xang)});
    rotx.push_back({0, sin(xang), cos(xang)});

    roty.push_back({cos(yang), 0, sin(yang)});
    roty.push_back({0, 1, 0});
    roty.push_back({-1*sin(yang), 0, cos(yang)});

    rotz.push_back({cos(zang), -1*sin(zang), 0});
    rotz.push_back({sin(zang), cos(zang), 0});
    rotz.push_back({0, 0, 1});
}
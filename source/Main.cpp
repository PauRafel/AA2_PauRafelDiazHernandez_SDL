#include "Game.h"
#include "TimeManager.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <exception>

int main(int argc, char* argv[])
{
    Game game;

    try
    {
        game.Init();
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        game.Release();
        return -1;
    }

    while (game.IsRunning())
    {
        TM.Update();

        if (TM.ShouldUpdateGame())
        {
            game.HandleEvents();
            game.Update();
            game.Render();
            TM.ResetDeltaTime();
        }
    }

    game.Release();

    return 0;
}
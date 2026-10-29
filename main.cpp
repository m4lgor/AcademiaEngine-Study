#include <memory>

#include "src/Engine/AcademiaEngine.h"
#include "src/Game/GameManager.h"

int main(int argc, char* argv[])
{
    AcademiaEngine* engine = AcademiaEngine::Instantiate();
    if(!engine)
    {
        return -1;
    }

    // The engine does not know the game: this is where they are connected
    engine->SetGame(std::make_unique<GameManager>());

    if (engine->Construct(1024, 1024, 1, 1))
        engine->Start();
 
    return 0;
}

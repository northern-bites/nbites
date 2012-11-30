#include "Scenes.h"

int main(int argc, char* argv[])
{
    SceneBuffer sb(5);
    int i;
    for (i = 0; i < 7; i++)
    {
        std::cout << "Insert!" << std::endl;
        sb.push_back(new Scene(1, 2));
        sb.printInfo();
        std::cout << std::endl;
    }

    return 0;
}

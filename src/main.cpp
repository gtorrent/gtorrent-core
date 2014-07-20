#include "Application.hpp"

int main(int argc, char **argv)
{
    return Application::getSingleton()->run(argc, argv);
}
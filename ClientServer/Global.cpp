#include "Global.h"
#include <iostream>
Global* Global::Instance = NULL;

Global& Global::instance()
{
    if (Instance == NULL)
    {
        Instance = new Global;
    }
    return *Instance;
}

Global::Global()
{
    log = new Logger;
}

Global::~Global()
{
    delete log;
}
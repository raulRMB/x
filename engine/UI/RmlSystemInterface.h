#ifndef X_RMLSYSTEM_INTERFACE
#define X_RMLSYSTEM_INTERFACE

#include "../vendor/RmlUi/Include/RmlUi/Core/Core.h"

class RmlSystemInterface : public Rml::SystemInterface
{
	double GetElapsedTime() {
        return 0;
    }
};

#endif
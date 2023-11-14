#include <SDL>

class RmlRenderInterface : public Rml::RenderInterface
{
	double GetElapsedTime() {
        return 0;
    }
};
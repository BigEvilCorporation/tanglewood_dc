
#include "Tanglewood.h"
#include <ion/Core/time/Time.h>

int main(int numargs, char** args)
{
	Tanglewood app;

	if(app.Initialise())
	{
		float deltaTime = 0.0f;
		bool run = true;
		while(run)
		{
			u64 startTicks = ion::time::GetSystemTicks();

			if(run = app.Update(deltaTime))
			{
				app.Render();
			}

			u64 endTicks = ion::time::GetSystemTicks();
			deltaTime = ion::maths::Clamp((float)ion::time::TicksToSeconds(endTicks - startTicks), 0.0f, 0.1f);

#if defined ION_PLATFORM_DREAMCAST
			deltaTime = 0.03f;
#endif
		}

		app.Shutdown();
	}
}

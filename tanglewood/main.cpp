
#include "Tanglewood.h"
#include <ion/core/time/Time.h>
#include <ion/core/debug/Debug.h>

int main(int numargs, char** args)
{
	ion::debug::InitExceptionHandling();

	Tanglewood app;

	if(app.Initialise())
	{
		float deltaTime = 0.0f;
		bool run = true;
		while(run)
		{
			u64 startTicks = ion::time::GetSystemTicks();

            run = app.Update(deltaTime);
			if(run)
			{
				app.Render();
			}

			u64 endTicks = ion::time::GetSystemTicks();
			deltaTime = (float)ion::time::TicksToSeconds(endTicks - startTicks);

#if defined ION_PLATFORM_DREAMCAST
			deltaTime = 0.015f;
#endif
		}

		app.Shutdown();
	}
}

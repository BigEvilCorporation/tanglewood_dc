
#include "Tanglewood.h"
#include <ion/core/Platform.h>
#include <ion/core/time/Time.h>
#include <ion/core/debug/Debug.h>
#include <ion/core/debug/CrashHandler.h>

int main(int numargs, char** args)
{
	ion::platform::Initialise();
	ion::debug::InstallDefaultCrashHandler();

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
			deltaTime = ion::maths::Clamp(deltaTime, 0.0f, 1.0f / 15.0f);

#if defined ION_PLATFORM_DREAMCAST
			deltaTime = 0.015f;
#endif
		}

		app.Shutdown();
	}

	ion::platform::Shutdown();
}

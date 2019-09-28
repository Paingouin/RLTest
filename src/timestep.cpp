#include "main.h"

void calcFrameTimes(int targetFramerate, int& mainFrameTime, int& alternateFrameTime, int& alternateFrameInterval)
{
	// Calculate the actual frame time, and the approximate (integer divided) frame time
	double realFrameTime = 1000.0f / targetFramerate;
	int approxFrameTime = 1000 / targetFramerate;

	// Find the fractional part of the real frame time, and use it to determine how many
	// frames should be rendered at the high frame time, and how many at the low frame time
	double frameTimeDiff = realFrameTime - approxFrameTime;
	int framesAtHigherTime = (int)(frameTimeDiff * targetFramerate + 0.5f);
	int framesAtLowerTime = targetFramerate - framesAtHigherTime;

	if (framesAtHigherTime >= framesAtLowerTime)
	{
		mainFrameTime = approxFrameTime + 1;
		alternateFrameTime = approxFrameTime;

		// Number of times to use the main amount before using the alternate amount, plus 1
		alternateFrameInterval = (int)(((double)framesAtHigherTime / framesAtLowerTime) + 0.5f) + 1;
	}
	else
	{
		mainFrameTime = approxFrameTime;
		alternateFrameTime = approxFrameTime + 1;

		// Number of times to use the main amount before using the alternate amount, plus 1
		alternateFrameInterval = (int)(((double)framesAtLowerTime / framesAtHigherTime) + 0.5f) + 1;
	}
}

struct GameTimer
{
	enum
	{
		FIXED_TIMESTEP,
		VARIABLE_TIMESTEP
	};
	//std::chrono::time_point<std::chrono::steady_clock> timeCurrentFrame;
	//std::chrono::time_point<std::chrono::steady_clock> timeLastFrame;
	//std::chrono::time_point<std::chrono::steady_clock> timeBeginGame;

	//std::chrono::time_point<std::chrono::steady_clock> timeLastGameUpdate;

	uint32 deltaTime;
	int timestepMode;

	int numberOfFrames = 1;

	double dtimeGame = 1000000.0 / 60.f; //GAME_LIMIT
	double dtimeRender = 1000000.0 / 60.0f; //FPS_LIMIT
	double lag = 0;


	sf::Time timeCurrentFrame;
	sf::Time timeLastFrame;
	sf::Time timeBeginGame;

	sf::Time timeLastGameUpdate;

	sf::Clock time;
	sf::Clock FPStimer;

	/*int32 getDurationMicrosec(std::chrono::steady_clock::time_point From, std::chrono::steady_clock::time_point To)
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(To - From).count();

	}

	int32 getDurationMillisec(std::chrono::steady_clock::time_point From, std::chrono::steady_clock::time_point To)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(To - From).count();

	}*/

	void initialize()
	{
		timestepMode = FIXED_TIMESTEP;
		dtimeGame = 1000000.0 / 60.f; //nbOfMicrosec per update
		dtimeRender = 1000000.0 / 60.f;//nbOfMicrosec per frame
		lag = 0.0F;

		timeLastFrame = time.getElapsedTime();
		timeCurrentFrame = timeLastFrame;
		timeLastGameUpdate = timeCurrentFrame;
	}

	 void startRenderFrame() //Put at the start of the while render loop
	{
	//	 if (numberOfFrames >= 60) FPStimer.restart();

		timeCurrentFrame = time.getElapsedTime();
		deltaTime = timeCurrentFrame.asMicroseconds() - timeLastFrame.asMicroseconds();
		
		//Prevent spiral of death
		if (deltaTime > dtimeGame * 5) deltaTime = dtimeGame * 5;

		lag += deltaTime;
	}

	inline void endRenderFrame()
	{
		timeLastFrame = timeCurrentFrame;
		//if (numberOfFrames < 60)
			numberOfFrames += 1;
		//else
		//{
		//	numberOfFrames = 1;
		//}
	}

	int doUpdate()
	{
		if (lag >= dtimeGame)
		{
			lag -= dtimeGame;
			return 1;
		}
		return 0;

	}

	double getLastUpdateTime()
	{
		//double t = getDurationMicrosec(timeLastGameUpdate, std::chrono::high_resolution_clock::now()) /1000000.0F;
		//timeLastGameUpdate = std::chrono::high_resolution_clock::now();
		//return t;

	}

	double getInterpolationAlpha()
	{
		return (lag < dtimeGame) ? lag / dtimeGame : 1.0f;
	}
	

	double getFPS()
	{
		return numberOfFrames / FPStimer.getElapsedTime().asSeconds();
	}

	double getLastFrameTime()
	{
		return (deltaTime != 0.0) ? 1000000.f / deltaTime : 0;
	}

	void sleepAfterRender()
	{
		//get Time to sleep in microseconds
		int timeSleep =( dtimeRender -deltaTime >3)? dtimeRender - deltaTime : 0;
		sf::sleep(sf::microseconds(timeSleep));
		//while (timeSleep > 2000) //If we can sleep at least 1 ms
		//{
		//	timeBeginPeriod(1);
		//	//99% of the time : sleep will sleep between timesleep and timesleep +2 or 3 ms...
		//	std::this_thread::sleep_for(std::chrono::microseconds(timeSleep));
		//	timeEndPeriod(1);
		//	timeSleep = std::round(timeRender - getDurationMicrosec(timeCurrentFrame, std::chrono::high_resolution_clock::now()));
		//}

		//while (getDurationMicrosec(timeCurrentFrame, std::chrono::high_resolution_clock::now()) < timeRender);
	}



	//int mainFrameTime, alternateFrameTime, alternateFrameInterval;
	//int frames = 0;
	//calcFrameTimes(FPS_LIMIT, mainFrameTime, alternateFrameTime, alternateFrameInterval);
	////////////////CLOCK

		/*DO  NOT ERASE
		int frameTime;
		if (frames % alternateFrameInterval == 0)
		{
			frameTime = alternateFrameTime;
			frames = 0;
		}
		else
		{
			frameTime = mainFrameTime;
		}
		*/

		//frames++;
};
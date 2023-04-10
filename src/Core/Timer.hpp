#include <SDL2/SDL.h>
#include <string>
#include <functional>
#include <variant>


#define TIMER_CALLBACK_PARAM_DEFAULT_TYPES \
    int, float, double, bool, Uint32, Uint64, char, std::string

#ifndef TIMER_CALLBACK_PARAM_TYPES
#   ifndef TIMER_CALLBACK_PARAM_ADDITIONAL_TYPES
#       define TIMER_CALLBACK_PARAM_TYPES \
            TIMER_CALLBACK_PARAM_DEFAULT_TYPES
#   else
#       define TIMER_CALLBACK_PARAM_TYPES \
            TIMER_CALLBACK_PARAM_DEFAULT_TYPES, \
            TIMER_CALLBACK_PARAM_ADDITIONAL_TYPES
#   endif // TIMER_CALLBACK_PARAM_ADDITIONAL_TYPES
#endif // TIMER_CALLBACK_PARAM_TYPES



using TimerCallbackData = std::variant<TIMER_CALLBACK_PARAM_TYPES>;

using TimerCallback = std::function<Uint32(Uint32, TimerCallbackData*)>;

/*PRIVATE FUNCTION*/ extern "C" Uint32 _TimerCallbackWrapper(Uint32 interval, void* data);

class Timer {
public:
    Timer(Uint32 const& interval, TimerCallback const callback, TimerCallbackData* const callbackData, bool deleteDataUponDestruction);
    bool dataIsDeletedUponDestruction() const;
    bool run(bool runOnceRightNow = false);
    bool isRunning() const;
    Uint32 getCurrentInterval() const;
    bool pause();
    bool resume();
    bool toggle();
    ~Timer();

private:
	// Adding :: here seems legit according to C++ standard,
	// since it denotes the function should be found in global
	// namespace.
	// While CodeBlocks accept this syntax, Visual Studio
	// throws out error E0276 that reads:
	// > name followed by '::' must be a class or namespace name
	// Therefore, I leave the operator commented here.
    friend Uint32 /*::*/_TimerCallbackWrapper(Uint32 interval, void* data);
    Uint32 m_currentInterval;
    TimerCallback const m_callback;
    TimerCallbackData* const m_callbackDataPtr;
    bool const m_deleteDataUponDestruction;
    SDL_TimerID m_SDLTimerID;
};

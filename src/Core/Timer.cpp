#include "Timer.hpp"
#include "Exception.hpp"

// TODO: Make use of Application log mechanism

Timer::Timer(Uint32 const& interval, TimerCallback const callback, TimerCallbackData* const callbackDataPtr, bool deleteDataUponDestruction)
    : m_currentInterval(interval),
    m_callback(callback),
    m_callbackDataPtr(callbackDataPtr),
    m_deleteDataUponDestruction(deleteDataUponDestruction),
    m_SDLTimerID(0) {
}

bool Timer::dataIsDeletedUponDestruction() const {
    return m_deleteDataUponDestruction;
}

/*PRIVATE FUNCTION*/ extern "C" Uint32 _TimerCallbackWrapper(Uint32 interval, void* TimerInstanceCastedToVoidPtr) {
    Timer* timer = (Timer*)TimerInstanceCastedToVoidPtr;
    timer->m_currentInterval = timer->m_callback(timer->m_currentInterval, timer->m_callbackDataPtr);
    return timer->m_currentInterval;
}

bool Timer::run(bool runOnceRightNow) {
    if (isRunning()) return false;
    m_SDLTimerID = SDL_AddTimer(
        (runOnceRightNow ? 0 : m_currentInterval),
        ::_TimerCallbackWrapper,
        (void*)this
    );
    if (m_SDLTimerID == 0) {
        throw Exception(std::string("SDL_AddTimer() failed. Reason: ") + SDL_GetError());
    }
    return true;
}

bool Timer::isRunning() const {
    return m_SDLTimerID != 0;
}

Uint32 Timer::getCurrentInterval() const {
    return m_currentInterval;
}

bool Timer::pause() {
    // SDL Timer subsystem actually doesn't support pausing timer.
    // As a result, this method just removes the underlying
    // SDL timer by its ID.
    // The resume() method will therefore just call SDL_AddTimer()
    // again.
    if (!isRunning()) return false;
    if ( SDL_RemoveTimer(m_SDLTimerID) != SDL_TRUE ) {
        // throw Exception(std::string("SDL_RemoveTimer() failed. Reason: ") + SDL_GetError());
		// When SDL_Quit() is called, removing timer may fail.
		// Should we consider that trivial ? I guess yes.
    }
    m_SDLTimerID = 0;
    return true;
}

inline bool Timer::resume() {
    return run();
}

bool Timer::toggle() {
    return (isRunning() ? pause() : resume());
}

Timer::~Timer() {
    pause();
    m_currentInterval = 0;
    if (dataIsDeletedUponDestruction()) {
        delete m_callbackDataPtr;
    }
    // m_callbackDataPtr = nullptr;
    // m_SDLTimerID = 0;
}

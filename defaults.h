#ifndef DEFAULTS_H
#define DEFAULTS_H
#include <mutex>
#include <condition_variable>

extern std::mutex camBufferMutex;
extern std::condition_variable camBufferCond;
extern std::mutex encBufferMutex;
extern std::condition_variable encBufferCond;
extern int encoderBusy;
extern int cameraBusy;


#endif
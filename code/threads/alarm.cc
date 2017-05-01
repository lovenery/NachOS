// alarm.cc
//	Routines to use a hardware timer device to provide a
//	software alarm clock.  For now, we just provide time-slicing.
//
//	Not completely implemented.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "alarm.h"
#include "main.h"

//----------------------------------------------------------------------
// Alarm::Alarm
//      Initialize a software alarm clock.  Start up a timer device
//
//      "doRandom" -- if true, arrange for the hardware interrupts to 
//		occur at random, instead of fixed, intervals.
//----------------------------------------------------------------------

Alarm::Alarm(bool doRandom)
{
    timer = new Timer(doRandom, this);
}

//----------------------------------------------------------------------
// Alarm::CallBack
//	Software interrupt handler for the timer device. The timer device is
//	set up to interrupt the CPU periodically (once every TimerTicks).
//	This routine is called each time there is a timer interrupt,
//	with interrupts disabled.
//
//	Note that instead of calling Yield() directly (which would
//	suspend the interrupt handler, not the interrupted thread
//	which is what we wanted to context switch), we set a flag
//	so that once the interrupt handler is done, it will appear as 
//	if the interrupted thread called Yield at the point it is 
//	was interrupted.
//
//	For now, just provide time-slicing.  Only need to time slice 
//      if we're currently running something (in other words, not idle).
//	Also, to keep from looping forever, we check if there's
//	nothing on the ready list, and there are no other pending
//	interrupts.  In this case, we can safely halt.
//----------------------------------------------------------------------

// kernel有alarm，代表每隔固定一段間，就會呼叫Alarm::CallBack()
void 
Alarm::CallBack() 
{
    Interrupt *interrupt = kernel->interrupt;
    MachineStatus status = interrupt->getStatus();
    bool woken = _bedroom.MorningCall(); // PROJECT2

    // PROJECT2: add && !woken && _bedroom.IsEmpty()
    if (status == IdleMode && !woken && _bedroom.IsEmpty()) {	// is it time to quit?
        if (!interrupt->AnyFutureInterrupts()) {
	    timer->Disable();	// turn off the timer
	}
    } else {			// there's someone to preempt
	interrupt->YieldOnReturn();
    }
}

// PROJECT2
void Alarm::WaitUntil(int x) {
    // turn off interrupt temporarily.
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
    Thread* t = kernel->currentThread;

    cout << "Thread " << (int)t << " go sleep" << endl;
    
    _bedroom.PutToBed(t, x);
    kernel->interrupt->SetLevel(oldLevel); // set the original interrupt level back.
}

// PROJECT2
bool Bedroom::IsEmpty() {
    return _beds.size() == 0;
}

// 某一時間 time = _current_interrupt 呼叫 Sleep(x) 後，
// 將 (Thread address, _current_interrupt + x) 丟入 List，
// 期望在 _current_interrupt + x 的時候甦醒。
// PROJECT2
void Bedroom::PutToBed(Thread* t, int x) {
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    _beds.push_back(Bed(t, _current_interrupt + x));
    t->Sleep(false);
}

// 每當 _current_interrupt++ 就去檢查 List 中，誰的預期時間小於 _current_interrupt，就將其從 List 中清除。
// PROJECT2
bool Bedroom::MorningCall() {
    bool woken = false;

    _current_interrupt++;

    for(std::list<Bed>::iterator it = _beds.begin(); it != _beds.end(); ) {
        if(_current_interrupt >= it->when) {
            woken = true;
            cout << "Bedroom::MorningCall Thread woken" << endl;
            kernel->scheduler->ReadyToRun(it->sleeper);
            it = _beds.erase(it);
        } else {
            it++;
        }
    }
    return woken;
}

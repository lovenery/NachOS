#ifndef BEDROOM_H
#define BEDROOM_H

#include "thread.h"  // sleeper Thread* DONE
#include <list>      // _sleeping_list        DONE

class Bedroom {
    public:
        Bedroom():_current_interrupt(0) {};
        void PutToBed(Thread *t, int x); // put a thread to sleep and sets when to wake it up.
        bool MorningCall();
        bool IsEmpty();

    private:
        // entry for every sleeping thread
        class Bed {
            public:
                Bed(Thread* t, int x): sleeper(t), when(x) { };
                Thread* sleeper; // the thread that lies on bed.
                int when; // when to wake up
        };
    
    int _current_interrupt; // 鬧鐘累加器, 每加一次過了1 ms
    std::list<Bed> _beds;
};

#endif

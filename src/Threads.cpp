/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// #define LOG_NDEBUG 0
#include "Debug.h"
#include <assert.h>
#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(HAVE_PTHREADS)
# include <pthread.h>
# include <sched.h>
# include <sys/resource.h>
#ifdef HAVE_ANDROID_OS
//# include <private/bionic_pthread.h>
#endif
#elif defined(HAVE_WIN32_THREADS)
//# include <windows.h>
//# include <stdint.h>
//# include <process.h>
//# define HAVE_CREATETHREAD  // Cygwin, vs. HAVE__BEGINTHREADEX for MinGW
#endif

#if defined(HAVE_PRCTL)
#include <sys/prctl.h>
#endif

#include <utils/Thread.h>
//#include <utils/Log.h>

//#include <cutils/sched_policy.h>

#ifdef HAVE_ANDROID_OS
//# define __android_unused
#else
# define __android_unused __attribute__((__unused__))
#endif

/*
 * ===========================================================================
 *      Thread wrappers
 * ===========================================================================
 */

//using namespace mlib;

// ----------------------------------------------------------------------------
#if defined(HAVE_PTHREADS)
// ----------------------------------------------------------------------------

/*
 * Create and run a new thread.
 *
 * We create it "detached", so it cleans up after itself.
 */

typedef void* (*_pthread_entry)(void*);

//struct thread_data_t {
//    thread_func_t   entryFunction;
//    void*           userData;
//    int             priority;
//    char *          threadName;
//
//    // we use this trampoline when we need to set the priority with
//    // nice/setpriority, and name with prctl.
//    static int trampoline(const thread_data_t* t) {
//        thread_func_t f = t->entryFunction;
//        void* u = t->userData;
//        int prio = t->priority;
//        char * name = t->threadName;
//        delete t;
//        setpriority(PRIO_PROCESS, 0, prio);
//        if (prio >= ANDROID_PRIORITY_BACKGROUND) {
//            set_sched_policy(0, SP_BACKGROUND);
//        } else {
//            set_sched_policy(0, SP_FOREGROUND);
//        }
//        
//        if (name) {
//            androidSetThreadName(name);
//            free(name);
//        }
//        return f(u);
//    }
//};
//
//void androidSetThreadName(const char* name) {
//#if defined(HAVE_PRCTL)
//    // Mac OS doesn't have this, and we build libutil for the host too
//    int hasAt = 0;
//    int hasDot = 0;
//    const char *s = name;
//    while (*s) {
//        if (*s == '.') hasDot = 1;
//        else if (*s == '@') hasAt = 1;
//        s++;
//    }
//    int len = s - name;
//    if (len < 15 || hasAt || !hasDot) {
//        s = name;
//    } else {
//        s = name + len - 15;
//    }
//    prctl(PR_SET_NAME, (unsigned long) s, 0, 0, 0);
//#endif
//}

int createRawThreadEtc(_thread_func_t entryFunction,void *userData,  size_t threadStackSize, _thread_id_t *threadId)
{
    pthread_attr_t attr; 
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

//#ifdef HAVE_ANDROID_OS  /* valgrind is rejecting RT-priority create reqs */
// //   if (threadPriority != PRIORITY_DEFAULT || threadName != NULL) {
// //       // Now that the pthread_t has a method to find the associated
// //       // _thread_id_t (pid) from pthread_t, it would be possible to avoid
// //       // this trampoline in some cases as the parent could set the properties
// //       // for the child.  However, there would be a race condition because the
// //       // child becomes ready immediately, and it doesn't work for the name.
// //       // prctl(PR_SET_NAME) only works for self; prctl(PR_SET_THREAD_NAME) was
// //       // proposed but not yet accepted.
// //       thread_data_t* t = new thread_data_t;
// //       t->priority = threadPriority;
// //       t->threadName = threadName ? strdup(threadName) : NULL;
// //       t->entryFunction = entryFunction;
// //       t->userData = userData;
// //       entryFunction = (_thread_func_t)&thread_data_t::trampoline;
// //       userData = t;            
// //   }
//#endif

    if (threadStackSize) {
        pthread_attr_setstacksize(&attr, threadStackSize);
    }
    
    errno = 0;
    pthread_t thread;
    int result = pthread_create(&thread, &attr,
                    (_pthread_entry)entryFunction, userData);
    pthread_attr_destroy(&attr);
    if (result != 0) {
        return 0;
    }

    // Note that *threadID is directly available to the parent only, as it is
    // assigned after the child starts.  Use memory barrier / lock if the child
    // or other threads also need access.
    if (threadId != NULL) {
        *threadId = (_thread_id_t)thread; // XXX: this is not portable
    }
    return 1;
}
_thread_id_t getThreadId()
{
    return (_thread_id_t)pthread_self();
}
//#ifdef HAVE_ANDROID_OS
////static pthread_t _thread_id_t_to_pthread(_thread_id_t thread)
////{
////    return (pthread_t) thread;
////}
//#endif
//
//_thread_id_t androidGetThreadId()
//{
//    return (_thread_id_t)pthread_self();
//}
//
//// ----------------------------------------------------------------------------
//#elif defined(HAVE_WIN32_THREADS)
////// ----------------------------------------------------------------------------
////
/////*
//// * Trampoline to make us __stdcall-compliant.
//// *
//// * We're expected to delete "vDetails" when we're done.
//// */
////struct threadDetails {
////    int (*func)(void*);
////    void* arg;
////};
////static __stdcall unsigned int threadIntermediary(void* vDetails)
////{
////    struct threadDetails* pDetails = (struct threadDetails*) vDetails;
////    int result;
////
////    result = (*(pDetails->func))(pDetails->arg);
////
////    delete pDetails;
////
////    ALOG(LOG_VERBOSE, "thread", "thread exiting\n");
////    return (unsigned int) result;
////}
////
/////*
//// * Create and run a new thread.
//// */
////static bool doCreateThread(_thread_func_t fn, void* arg, _thread_id_t *id)
////{
////    HANDLE hThread;
////    struct threadDetails* pDetails = new threadDetails; // must be on heap
////    unsigned int thrdaddr;
////
////    pDetails->func = fn;
////    pDetails->arg = arg;
////
////#if defined(HAVE__BEGINTHREADEX)
////    hThread = (HANDLE) _beginthreadex(NULL, 0, threadIntermediary, pDetails, 0,
////                    &thrdaddr);
////    if (hThread == 0)
////#elif defined(HAVE_CREATETHREAD)
////    hThread = CreateThread(NULL, 0,
////                    (LPTHREAD_START_ROUTINE) threadIntermediary,
////                    (void*) pDetails, 0, (DWORD*) &thrdaddr);
////    if (hThread == NULL)
////#endif
////    {
////        ALOG(LOG_WARN, "thread", "WARNING: thread create failed\n");
////        return false;
////    }
////
////#if defined(HAVE_CREATETHREAD)
////    /* close the management handle */
////    CloseHandle(hThread);
////#endif
////
////    if (id != NULL) {
////      	*id = (_thread_id_t)thrdaddr;
////    }
////
////    return true;
////}
////
////int createRawThreadEtc(_thread_func_t fn,
////                               void *userData,
////                               const char* /*threadName*/,
////                               int32_t /*threadPriority*/,
////                               size_t /*threadStackSize*/,
////                               _thread_id_t *threadId)
////{
////    return doCreateThread(  fn, userData, threadId);
////}
////
////_thread_id_t androidGetThreadId()
////{
////    return (_thread_id_t)GetCurrentThreadId();
////}
////
////// ----------------------------------------------------------------------------
//#else
//#error "Threads not supported"
#endif
//
//// ----------------------------------------------------------------------------
//
//int androidCreateThread(_thread_func_t fn, void* arg)
//{
//    return createThreadEtc(fn, arg);
//}
//
//int androidCreateThreadGetID(_thread_func_t fn, void *arg, _thread_id_t *id)
//{
//    return createThreadEtc(fn, arg, "android:unnamed_thread",
//                           PRIORITY_DEFAULT, 0, id);
//}
//
//static android_create_thread_fn gCreateThreadFn = createRawThreadEtc;
//
//int androidCreateThreadEtc(_thread_func_t entryFunction,
//                            void *userData,
//                            const char* threadName,
//                            int32_t threadPriority,
//                            size_t threadStackSize,
//                            _thread_id_t *threadId)
//{
//    return gCreateThreadFn(entryFunction, userData, threadName,
//        threadPriority, threadStackSize, threadId);
//}
//
//void androidSetCreateThreadFunc(android_create_thread_fn func)
//{
//    gCreateThreadFn = func;
//}
//
//pid_t androidGetTid()
//{
//#ifdef HAVE_GETTID
//    return gettid();
//#else
//    return getpid();
//#endif
//}
//
//#ifdef HAVE_ANDROID_OS
//int androidSetThreadPriority(pid_t tid, int pri)
//{
//    int rc = 0;
//    
//#if defined(HAVE_PTHREADS)
//    int lasterr = 0;
//
//    if (pri >= ANDROID_PRIORITY_BACKGROUND) {
//        rc = set_sched_policy(tid, SP_BACKGROUND);
//    } else if (getpriority(PRIO_PROCESS, tid) >= ANDROID_PRIORITY_BACKGROUND) {
//        rc = set_sched_policy(tid, SP_FOREGROUND);
//    }
//
//    if (rc) {
//        lasterr = errno;
//    }
//
//    if (setpriority(PRIO_PROCESS, tid, pri) < 0) {
//        rc = INVALID_OPERATION;
//    } else {
//        errno = lasterr;
//    }
//#endif
//    
//    return rc;
//}
//
//int androidGetThreadPriority(pid_t tid) {
//#if defined(HAVE_PTHREADS)
//    return getpriority(PRIO_PROCESS, tid);
//#else
//    return ANDROID_PRIORITY_NORMAL;
//#endif
//}

//#endif

namespace mlib {

/*
 * ===========================================================================
 *      Mutex class
 * ===========================================================================
 */

#if defined(HAVE_PTHREADS)
// implemented as inlines in threads.h
#endif


/*
 * ===========================================================================
 *      Condition class
 * ===========================================================================
 */

#if defined(HAVE_PTHREADS)
// implemented as inlines in threads.h
#endif

// ----------------------------------------------------------------------------

/*
 * This is our thread object!
 */

Thread::Thread()
      : mThread(thread_id_t(-1)),
        mLock("Thread::mLock"),
        mStatus(NO_ERROR),
        mExitPending(false), mRunning(false)
#ifdef HAVE_ANDROID_OS
        , mTid(-1)
#endif
{
}

Thread::~Thread()
{
}

status_t Thread::readyToRun()
{
    return NO_ERROR;
}

status_t Thread::run( size_t stack)
{
    Mutex::Autolock _l(mLock);

    if (mRunning) {
        // thread already started
        return INVALID_OPERATION;
    }

    // reset status and exitPending to their default value, so we can
    // try again after an error happened (either below, or in readyToRun())
    mStatus = NO_ERROR;
    mExitPending = false;
    //mThread = thread_id_t(-1);
    
    // hold a strong reference on ourself
    //mHoldSelf = this;

    mRunning = true;

    bool res;
        res = createRawThreadEtc(_threadLoop,		\
                this,  stack, &mThread);
    
    if (res == false) {
        mStatus = UNKNOWN_ERROR;   // something happened!
        mRunning = false;
        //mThread = thread_id_t(-1);
        //mHoldSelf.clear();  // "this" may have gone away after this.

        return UNKNOWN_ERROR;
    }
    
    // Do not refer to mStatus here: The thread is already running (may, in fact
    // already have exited with a valid mStatus result). The NO_ERROR indication
    // here merely indicates successfully starting the thread and does not
    // imply successful termination/execution.
    return NO_ERROR;

    // Exiting scope of mLock is a memory barrier and allows new thread to run
}

int Thread::_threadLoop(void* user)
{
    Thread* const self = static_cast<Thread*>(user);

//    sp<Thread> strong(self->mHoldSelf);
//    wp<Thread> weak(strong);
//    self->mHoldSelf.clear();

#ifdef HAVE_ANDROID_OS
//    // this is very useful for debugging with gdb
//    self->mTid = gettid();
#endif

    bool first = true;

    do {
        bool result;
        if (first) {
            first = false;
            self->mStatus = self->readyToRun();
            result = (self->mStatus == NO_ERROR);

            if (result && !self->exitPending()) {
                // Binder threads (and maybe others) rely on threadLoop
                // running at least once after a successful ::readyToRun()
                // (unless, of course, the thread has already been asked to exit
                // at that point).
                // This is because threads are essentially used like this:
                //   (new ThreadSubclass())->run();
                // The caller therefore does not retain a strong reference to
                // the thread and the thread would simply disappear after the
                // successful ::readyToRun() call instead of entering the
                // threadLoop at least once.
                result = self->threadLoop();
            }
        } else {
            result = self->threadLoop();
        }

        // establish a scope for mLock
        {
        Mutex::Autolock _l(self->mLock);
        if (result == false || self->mExitPending) {
            self->mExitPending = true;
            self->mRunning = false;
            // clear thread ID so that requestExitAndWait() does not exit if
            // called by a new thread using the same thread ID as this one.
            //self->mThread = thread_id_t(-1);
            // note that interested observers blocked in requestExitAndWait are
            // awoken by broadcast, but blocked on mLock until break exits scope
	    printf("wait 5s for test\n");
	    sleep(5);//test code
            self->mThreadExitedCondition.broadcast();
            break;
        }
        }
        
        // Release our strong reference, to let a chance to the thread
        // to die a peaceful death.
//        strong.clear();
        // And immediately, re-acquire a strong reference for the next loop
 //       strong = weak.promote();
    //} while(strong != 0);
    } while(self->mThread!=0);
    
    return 0;
}

void Thread::requestExit()
{
    Mutex::Autolock _l(mLock);
    mExitPending = true;
}

status_t Thread::requestExitAndWait()
{
    Mutex::Autolock _l(mLock);
    DEBUG("mThread=%p",mThread) ;
    DEBUG("getThreadId=%p",getThreadId()) ;
    if (mThread == getThreadId()) {
        ERROR(
        "Thread (this=%p): don't call waitForExit() from this "
        "Thread object's thread. It's a guaranteed deadlock!",
        this);

        return WOULD_BLOCK;
    }
    mExitPending = true;

    while (mRunning == true) {
        mThreadExitedCondition.wait(mLock);
    }
    // This next line is probably not needed any more, but is being left for
    // historical reference. Note that each interested party will clear flag.
    mExitPending = false;
	DEBUG("get thread exit signal \n");
    return mStatus;
}

status_t Thread::join()
{
    Mutex::Autolock _l(mLock);
    DEBUG("mThread=%p",mThread) ;
    DEBUG("getThreadId=%p",getThreadId()) ;
    if (mThread == getThreadId()) {
        ERROR(
        "Thread (this=%p): don't call join() from this "
        "Thread object's thread. It's a guaranteed deadlock!",
        this);

        return WOULD_BLOCK;
    }

    while (mRunning == true) {
        mThreadExitedCondition.wait(mLock);
    }

    return mStatus;
}

bool Thread::isRunning() const {
    Mutex::Autolock _l(mLock);
    return mRunning;
}

#ifdef HAVE_ANDROID_OS
//pid_t Thread::getTid() const
//{
//    // mTid is not defined until the child initializes it, and the caller may need it earlier
//    Mutex::Autolock _l(mLock);
//    pid_t tid;
//    if (mRunning) {
//        pthread_t pthread = _thread_id_t_to_pthread(mThread);
//        tid = __pthread_gettid(pthread);
//    } else {
//        ALOGW("Thread (this=%p): getTid() is undefined before run()", this);
//        tid = -1;
//    }
//    return tid;
//}
#endif

bool Thread::exitPending() const
{
    Mutex::Autolock _l(mLock);
    return mExitPending;
}



};  // namespace android

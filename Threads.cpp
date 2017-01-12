
#include <assert.h>
#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if !defined(_WIN32)
# include <pthread.h>
# include <sched.h>
# include <sys/resource.h>
#else
# include <windows.h>
# include <stdint.h>
# include <process.h>
# define HAVE_CREATETHREAD  // Cygwin, vs. HAVE__BEGINTHREADEX for MinGW
#endif

#if defined(__linux__)
#include <sys/prctl.h>
#endif

#include "Thread.h"


#if defined(__ANDROID__)
# define __android_unused
#else
# define __android_unused __attribute__((__unused__))
#endif

/*
 * ===========================================================================
 *      Thread wrappers
 * ===========================================================================
 */


// ----------------------------------------------------------------------------
#if !defined(_WIN32)
// ----------------------------------------------------------------------------

/*
 * Create and run a new thread.
 *
 * We create it "detached", so it cleans up after itself.
 */

typedef void* (*android_pthread_entry)(void*);

struct thread_data_t {
    uint32_t   entryFunction;
    void*           userData;
    int             priority;
    char *          threadName;

    // we use this trampoline when we need to set the priority with
    // nice/setpriority, and name with prctl.
    static int trampoline(const thread_data_t* t) {
        uint32_t f = t->entryFunction;
        void* u = t->userData;
        int prio = t->priority;
        char * name = t->threadName;
        delete t;
        setpriority(PRIO_PROCESS, 0, prio);
        if (prio >= ANDROID_PRIORITY_BACKGROUND) {
            set_sched_policy(0, SP_BACKGROUND);
        } else {
            set_sched_policy(0, SP_FOREGROUND);
        }

        if (name) {
            androidSetThreadName(name);
            free(name);
        }
        return f(u);
    }
};

void androidSetThreadName(const char* name) {
#if defined(__linux__)
    // Mac OS doesn't have this, and we build libutil for the host too
    int hasAt = 0;
    int hasDot = 0;
    const char *s = name;
    while (*s) {
        if (*s == '.') hasDot = 1;
        else if (*s == '@') hasAt = 1;
        s++;
    }
    int len = s - name;
    if (len < 15 || hasAt || !hasDot) {
        s = name;
    } else {
        s = name + len - 15;
    }
    prctl(PR_SET_NAME, (unsigned long) s, 0, 0, 0);
#endif
}

int androidCreateRawThreadEtc(android_uint32_t entryFunction,
                               void *userData,
                               const char* threadName __android_unused,
                               int32_t threadPriority,
                               size_t threadStackSize,
                               android_thread_id_t *threadId)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

#if defined(__ANDROID__)  /* valgrind is rejecting RT-priority create reqs */
    if (threadPriority != PRIORITY_DEFAULT || threadName != NULL) {
        // Now that the pthread_t has a method to find the associated
        // android_thread_id_t (pid) from pthread_t, it would be possible to avoid
        // this trampoline in some cases as the parent could set the properties
        // for the child.  However, there would be a race condition because the
        // child becomes ready immediately, and it doesn't work for the name.
        // prctl(PR_SET_NAME) only works for self; prctl(PR_SET_THREAD_NAME) was
        // proposed but not yet accepted.
        thread_data_t* t = new thread_data_t;
        t->priority = threadPriority;
        t->threadName = threadName ? strdup(threadName) : NULL;
        t->entryFunction = entryFunction;
        t->userData = userData;
        entryFunction = (android_uint32_t)&thread_data_t::trampoline;
        userData = t;
    }
#endif

    if (threadStackSize) {
        pthread_attr_setstacksize(&attr, threadStackSize);
    }

    errno = 0;
    pthread_t thread;
    int result = pthread_create(&thread, &attr,
                    (android_pthread_entry)entryFunction, userData);
    pthread_attr_destroy(&attr);
    if (result != 0) {
        ALOGE("androidCreateRawThreadEtc failed (entry=%p, res=%d, %s)\n"
             "(android threadPriority=%d)",
            entryFunction, result, strerror(errno), threadPriority);
        return 0;
    }

    // Note that *threadID is directly available to the parent only, as it is
    // assigned after the child starts.  Use memory barrier / lock if the child
    // or other threads also need access.
    if (threadId != NULL) {
        *threadId = (android_thread_id_t)thread; // XXX: this is not portable
    }
    return 1;
}

#if defined(__ANDROID__)
static pthread_t android_thread_id_t_to_pthread(android_thread_id_t thread)
{
    return (pthread_t) thread;
}
#endif

android_thread_id_t androidGetThreadId()
{
    return (android_thread_id_t)pthread_self();
}
// ----------------------------------------------------------------------------
#endif // !defined(_WIN32)

// ----------------------------------------------------------------------------

int androidCreateThread(android_uint32_t fn, void* arg)
{
    return createThreadEtc(fn, arg);
}

int androidCreateThreadGetID(android_uint32_t fn, void *arg, android_thread_id_t *id)
{
    return createThreadEtc(fn, arg, "android:unnamed_thread",
                           PRIORITY_DEFAULT, 0, id);
}

static android_create_thread_fn gCreateThreadFn = androidCreateRawThreadEtc;

int androidCreateThreadEtc(android_uint32_t entryFunction,
                            void *userData,
                            const char* threadName,
                            int32_t threadPriority,
                            size_t threadStackSize,
                            android_thread_id_t *threadId)
{
    return gCreateThreadFn(entryFunction, userData, threadName,
        threadPriority, threadStackSize, threadId);
}

void androidSetCreateThreadFunc(android_create_thread_fn func)
{
    gCreateThreadFn = func;
}

#if defined(__ANDROID__)
int androidSetThreadPriority(pid_t tid, int pri)
{
    int rc = 0;
    int lasterr = 0;

    if (pri >= ANDROID_PRIORITY_BACKGROUND) {
        rc = set_sched_policy(tid, SP_BACKGROUND);
    } else if (getpriority(PRIO_PROCESS, tid) >= ANDROID_PRIORITY_BACKGROUND) {
        rc = set_sched_policy(tid, SP_FOREGROUND);
    }

    if (rc) {
        lasterr = errno;
    }

    if (setpriority(PRIO_PROCESS, tid, pri) < 0) {
        rc = INVALID_OPERATION;
    } else {
        errno = lasterr;
    }

    return rc;
}

int androidGetThreadPriority(pid_t tid) {
    return getpriority(PRIO_PROCESS, tid);
}

#endif

namespace android {

/*
 * ===========================================================================
 *      Mutex class
 * ===========================================================================
 */

#if !defined(_WIN32)
// implemented as inlines in threads.h
#else

Mutex::Mutex()
{
    HANDLE hMutex;

    assert(sizeof(hMutex) == sizeof(mState));

    hMutex = CreateMutex(NULL, FALSE, NULL);
    mState = (void*) hMutex;
}

Mutex::Mutex(const char* name)
{
    // XXX: name not used for now
    HANDLE hMutex;

    assert(sizeof(hMutex) == sizeof(mState));

    hMutex = CreateMutex(NULL, FALSE, NULL);
    mState = (void*) hMutex;
}

Mutex::Mutex(int type, const char* name)
{
    // XXX: type and name not used for now
    HANDLE hMutex;

    assert(sizeof(hMutex) == sizeof(mState));

    hMutex = CreateMutex(NULL, FALSE, NULL);
    mState = (void*) hMutex;
}

Mutex::~Mutex()
{
    CloseHandle((HANDLE) mState);
}

status_t Mutex::lock()
{
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject((HANDLE) mState, INFINITE);
    return dwWaitResult != WAIT_OBJECT_0 ? -1 : NO_ERROR;
}

void Mutex::unlock()
{
    if (!ReleaseMutex((HANDLE) mState))
        ALOG(LOG_WARN, "thread", "WARNING: bad result from unlocking mutex\n");
}

status_t Mutex::tryLock()
{
    DWORD dwWaitResult;

    dwWaitResult = WaitForSingleObject((HANDLE) mState, 0);
    if (dwWaitResult != WAIT_OBJECT_0 && dwWaitResult != WAIT_TIMEOUT)
        ALOG(LOG_WARN, "thread", "WARNING: bad result from try-locking mutex\n");
    return (dwWaitResult == WAIT_OBJECT_0) ? 0 : -1;
}

#endif // !defined(_WIN32)


/*
 * ===========================================================================
 *      Condition class
 * ===========================================================================
 */



// ----------------------------------------------------------------------------

/*
 * This is our thread object!
 */

Thread::Thread(bool canCallJava)
    :   mCanCallJava(canCallJava),
        mThread(thread_id_t(-1)),
        mLock("Thread::mLock"),
        mStatus(NO_ERROR),
        mExitPending(false), mRunning(false)
#if defined(__ANDROID__)
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

status_t Thread::run(const char* name, int32_t priority, size_t stack)
{
    LOG_ALWAYS_FATAL_IF(name == nullptr, "thread name not provided to Thread::run");

    Mutex::Autolock _l(mLock);

    if (mRunning) {
        // thread already started
        return INVALID_OPERATION;
    }

    // reset status and exitPending to their default value, so we can
    // try again after an error happened (either below, or in readyToRun())
    mStatus = NO_ERROR;
    mExitPending = false;
    mThread = thread_id_t(-1);

    // hold a strong reference on ourself
    mHoldSelf = this;

    mRunning = true;

    bool res;
    if (mCanCallJava) {
        res = createThreadEtc(_threadLoop,
                this, name, priority, stack, &mThread);
    } else {
        res = androidCreateRawThreadEtc(_threadLoop,
                this, name, priority, stack, &mThread);
    }

    if (res == false) {
        mStatus = UNKNOWN_ERROR;   // something happened!
        mRunning = false;
        mThread = thread_id_t(-1);
        mHoldSelf.clear();  // "this" may have gone away after this.

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

    sp<Thread> strong(self->mHoldSelf);
    wp<Thread> weak(strong);
    self->mHoldSelf.clear();

#if defined(__ANDROID__)
    // this is very useful for debugging with gdb
    self->mTid = gettid();
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
            self->mThread = thread_id_t(-1);
            // note that interested observers blocked in requestExitAndWait are
            // awoken by broadcast, but blocked on mLock until break exits scope
            self->mThreadExitedCondition.broadcast();
            break;
        }
        }

        // Release our strong reference, to let a chance to the thread
        // to die a peaceful death.
        strong.clear();
        // And immediately, re-acquire a strong reference for the next loop
        strong = weak.promote();
    } while(strong != 0);

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
    if (mThread == getThreadId()) {
        ALOGW(
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

    return mStatus;
}

status_t Thread::join()
{
    Mutex::Autolock _l(mLock);
    if (mThread == getThreadId()) {
        ALOGW(
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

#if defined(__ANDROID__)
pid_t Thread::getTid() const
{
    // mTid is not defined until the child initializes it, and the caller may need it earlier
    Mutex::Autolock _l(mLock);
    pid_t tid;
    if (mRunning) {
        pthread_t pthread = android_thread_id_t_to_pthread(mThread);
        tid = pthread_gettid_np(pthread);
    } else {
        ALOGW("Thread (this=%p): getTid() is undefined before run()", this);
        tid = -1;
    }
    return tid;
}
#endif

bool Thread::exitPending() const
{
    Mutex::Autolock _l(mLock);
    return mExitPending;
}




//
// Created by Gil Osher on 2/16/25.
//

#include <jni.h>
#include <signal.h>
#include <dlfcn.h>
#include <android/log.h>
#include <string.h>

#define LOG_TAG "JNILoader"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Define a type for the function pointer (matching the signature in your variants)
typedef jboolean (*InitType)(JNIEnv *env, jclass this, jobject core, jobject romBuffer);
typedef void (*NotifyDiskInsertedType)(jint drive, jboolean locked);
typedef void (*NotifyDiskEjectedType)(jint drive);
typedef void (*NotifyDiskCreatedType)();
typedef jint (*GetFirstFreeDiskType)();
typedef jint (*GetNumDrivesType)();
typedef void (*MySound_Start0Type)();
typedef jint (*ScreenWidthType)();
typedef jint (*ScreenHeightType)();
typedef jint (*ScreenDepthType)();
typedef jintArray (*GetScreenUpdateType)();
typedef void (*MoveMouseType)(jint dx, jint dy);
typedef void (*SetMousePosType)(jint x, jint y);
typedef void (*SetMouseButtonType)(jboolean down);
typedef jint (*GetMouseXType)();
typedef jint (*GetMouseYType)();
typedef jboolean (*GetMouseButtonType)();
typedef void (*SetKeyDownType)(jint key);
typedef void (*SetKeyUpType)(jint key);
typedef void (*SetWantMacResetType)();
typedef void (*SetWantMacInterruptType)();
typedef void (*SetRequestMacOffType)();
typedef void (*SetForceMacOffType)();
typedef void (*ResumeEmulationType)();
typedef void (*PauseEmulationType)();
typedef jboolean (*IsPausedType)();
typedef void (*SetSpeedType)(jint speed);
typedef jint (*GetSpeedType)();

// Global variables to keep the current variant handle and function pointer.
static void* variantHandle = NULL;
static InitType initPtr = NULL;
static NotifyDiskInsertedType notifyDiskInsertedPtr = NULL;
static NotifyDiskEjectedType notifyDiskEjectedPtr = NULL;
static NotifyDiskCreatedType notifyDiskCreatedPtr = NULL;
static GetFirstFreeDiskType getFirstFreeDiskPtr = NULL;
static GetNumDrivesType getNumDrivesPtr = NULL;
static MySound_Start0Type mySound_Start0Ptr = NULL;
static ScreenWidthType screenWidthPtr = NULL;
static ScreenHeightType screenHeightPtr = NULL;
static ScreenDepthType screenDepthPtr = NULL;
static GetScreenUpdateType getScreenUpdatePtr = NULL;
static MoveMouseType moveMousePtr = NULL;
static SetMousePosType setMousePosPtr = NULL;
static SetMouseButtonType setMouseButtonPtr = NULL;
static GetMouseXType getMouseXPtr = NULL;
static GetMouseYType getMouseYPtr = NULL;
static GetMouseButtonType getMouseButtonPtr = NULL;
static SetKeyDownType setKeyDownPtr = NULL;
static SetKeyUpType setKeyUpPtr = NULL;
static SetWantMacResetType setWantMacResetPtr = NULL;
static SetWantMacInterruptType setWantMacInterruptPtr = NULL;
static SetRequestMacOffType setRequestMacOffPtr = NULL;
static SetForceMacOffType setForceMacOffPtr = NULL;
static ResumeEmulationType resumeEmulationPtr = NULL;
static PauseEmulationType pauseEmulationPtr = NULL;
static IsPausedType isPausedPtr = NULL;
static SetSpeedType setSpeedPtr = NULL;
static GetSpeedType getSpeedPtr = NULL;

// Helper: Unload any currently loaded variant library.
void unloadCurrentVariant() {
    if (variantHandle) {
        dlclose(variantHandle);
        variantHandle = NULL;
        initPtr = NULL;
        notifyDiskInsertedPtr = NULL;
        notifyDiskEjectedPtr = NULL;
        notifyDiskCreatedPtr = NULL;
        getFirstFreeDiskPtr = NULL;
        getNumDrivesPtr = NULL;
        mySound_Start0Ptr = NULL;
        screenWidthPtr = NULL;
        screenHeightPtr = NULL;
        screenDepthPtr = NULL;
        getScreenUpdatePtr = NULL;
        moveMousePtr = NULL;
        setMousePosPtr = NULL;
        setMouseButtonPtr = NULL;
        getMouseXPtr = NULL;
        getMouseYPtr = NULL;
        getMouseButtonPtr = NULL;
        setKeyDownPtr = NULL;
        setKeyUpPtr = NULL;
        setWantMacResetPtr = NULL;
        setWantMacInterruptPtr = NULL;
        setRequestMacOffPtr = NULL;
        setForceMacOffPtr = NULL;
        resumeEmulationPtr = NULL;
        pauseEmulationPtr = NULL;
        isPausedPtr = NULL;
        setSpeedPtr = NULL;
        getSpeedPtr = NULL;
        LOGI("Variant library unloaded.");
    }
}

// JNI function to load a variant library.
// The parameter "libPath" should be the absolute path to the .so file.
JNIEXPORT jboolean JNICALL
Java_name_osher_gil_minivmac_Core_loadVariant(JNIEnv* env, jobject this, jstring libPath) {
    // Get the library path string from Java.
    const char* path = (*env)->GetStringUTFChars(env, libPath, NULL);
    if (!path) {
        LOGE("Invalid library path.");
        return JNI_FALSE;
    }

    // Unload any previously loaded variant.
    unloadCurrentVariant();

    // Try to load the new variant library.
    variantHandle = dlopen(path, RTLD_NOW);
    if (!variantHandle) {
        LOGE("dlopen failed: %s", dlerror());
        (*env)->ReleaseStringUTFChars(env, libPath, path);
        return JNI_FALSE;
    }

    // Look up the symbol "myFunction" from the variant.
    initPtr = (InitType)dlsym(variantHandle, "init");
    notifyDiskInsertedPtr = (NotifyDiskInsertedType)dlsym(variantHandle, "notifyDiskInserted");
    notifyDiskEjectedPtr = (NotifyDiskEjectedType)dlsym(variantHandle, "notifyDiskEjected");
    notifyDiskCreatedPtr = (NotifyDiskCreatedType)dlsym(variantHandle, "notifyDiskCreated");
    getFirstFreeDiskPtr = (GetFirstFreeDiskType)dlsym(variantHandle, "getFirstFreeDisk");
    getNumDrivesPtr = (GetNumDrivesType)dlsym(variantHandle, "getNumDrives");
    mySound_Start0Ptr = (MySound_Start0Type)dlsym(variantHandle, "MySound_Start0");
    screenWidthPtr = (ScreenWidthType)dlsym(variantHandle, "screenWidth");
    screenHeightPtr = (ScreenHeightType)dlsym(variantHandle, "screenHeight");
    screenDepthPtr = (ScreenDepthType)dlsym(variantHandle, "screenDepth");
    getScreenUpdatePtr = (GetScreenUpdateType)dlsym(variantHandle, "getScreenUpdate");
    moveMousePtr = (MoveMouseType)dlsym(variantHandle, "moveMouse");
    setMousePosPtr = (SetMousePosType)dlsym(variantHandle, "setMousePos");
    setMouseButtonPtr = (SetMouseButtonType)dlsym(variantHandle, "setMouseButton");
    getMouseXPtr = (GetMouseXType)dlsym(variantHandle, "getMouseX");
    getMouseYPtr = (GetMouseYType)dlsym(variantHandle, "getMouseY");
    getMouseButtonPtr = (GetMouseButtonType)dlsym(variantHandle, "getMouseButton");
    setKeyDownPtr = (SetKeyDownType)dlsym(variantHandle, "setKeyDown");
    setKeyUpPtr = (SetKeyUpType)dlsym(variantHandle, "setKeyUp");
    setWantMacResetPtr = (SetWantMacResetType)dlsym(variantHandle, "setWantMacReset");
    setWantMacInterruptPtr = (SetWantMacInterruptType)dlsym(variantHandle, "setWantMacInterrupt");
    setRequestMacOffPtr = (SetRequestMacOffType)dlsym(variantHandle, "setRequestMacOff");
    setForceMacOffPtr = (SetForceMacOffType)dlsym(variantHandle, "setForceMacOff");
    resumeEmulationPtr = (ResumeEmulationType)dlsym(variantHandle, "resumeEmulation");
    pauseEmulationPtr = (PauseEmulationType)dlsym(variantHandle, "pauseEmulation");
    isPausedPtr = (IsPausedType)dlsym(variantHandle, "isPaused");
    setSpeedPtr = (SetSpeedType)dlsym(variantHandle, "setSpeed");
    getSpeedPtr = (GetSpeedType)dlsym(variantHandle, "getSpeed");

    const char* error = dlerror();
    if (error != NULL) {
        LOGE("dlsym failed: %s", error);
        dlclose(variantHandle);
        variantHandle = NULL;
        initPtr = NULL;
        notifyDiskInsertedPtr = NULL;
        notifyDiskEjectedPtr = NULL;
        notifyDiskCreatedPtr = NULL;
        getFirstFreeDiskPtr = NULL;
        getNumDrivesPtr = NULL;
        mySound_Start0Ptr = NULL;
        screenWidthPtr = NULL;
        screenHeightPtr = NULL;
        screenDepthPtr = NULL;
        getScreenUpdatePtr = NULL;
        moveMousePtr = NULL;
        setMousePosPtr = NULL;
        setMouseButtonPtr = NULL;
        getMouseXPtr = NULL;
        getMouseYPtr = NULL;
        getMouseButtonPtr = NULL;
        setKeyDownPtr = NULL;
        setKeyUpPtr = NULL;
        setWantMacResetPtr = NULL;
        setWantMacInterruptPtr = NULL;
        setRequestMacOffPtr = NULL;
        setForceMacOffPtr = NULL;
        resumeEmulationPtr = NULL;
        pauseEmulationPtr = NULL;
        isPausedPtr = NULL;
        setSpeedPtr = NULL;
        getSpeedPtr = NULL;
        (*env)->ReleaseStringUTFChars(env, libPath, path);
        return JNI_FALSE;
    }

    LOGI("Loaded variant library: %s", path);
    (*env)->ReleaseStringUTFChars(env, libPath, path);
    return JNI_TRUE;
}

// JNI function that delegates a call to myFunction in the loaded variant.

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    init
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT jboolean JNICALL Java_name_osher_gil_minivmac_Core_init (JNIEnv * env, jclass this, jobject core, jobject romBuffer) {
    if (!initPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return 0;
    }
    return initPtr(env, this, core, romBuffer);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    notifyDiskInserted
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_notifyDiskInserted (JNIEnv * env, jclass class, jint drive, jboolean locked) {
    if (!notifyDiskInsertedPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    notifyDiskInsertedPtr(drive, locked);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    notifyDiskEjected
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_notifyDiskEjected (JNIEnv * env, jclass class, jint drive) {
    if (!notifyDiskEjectedPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    notifyDiskEjectedPtr(drive);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    notifyDiskEjected
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_notifyDiskCreated (JNIEnv * env, jclass class) {
    if (!notifyDiskCreatedPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    notifyDiskCreatedPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getFirstFreeDisk
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_getFirstFreeDisk (JNIEnv * env, jclass class) {
    if (!getFirstFreeDiskPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return -1;
    }
    return getFirstFreeDiskPtr();
}

JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_getNumDrives (JNIEnv * env, jclass class) {
    if (!getNumDrivesPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return -1;
    }
    return getNumDrivesPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    MySound_Start0
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_MySound_1Start0 (JNIEnv * env, jclass class) {
    if (!mySound_Start0Ptr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    mySound_Start0Ptr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    screenWidth
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_screenWidth (JNIEnv * env, jclass class) {
    if (!screenWidthPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return -1;
    }
    return screenWidthPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    screenHeight
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_screenHeight (JNIEnv * env, jclass class) {
    if (!screenHeightPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return -1;
    }
    return screenHeightPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    screenDepth
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_screenDepth (JNIEnv * env, jclass class) {
    if (!screenDepthPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return -1;
    }
    return screenDepthPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getScreenUpdate
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_name_osher_gil_minivmac_Core_getScreenUpdate (JNIEnv * env, jclass class) {
    if (!getScreenUpdatePtr) {
        LOGE("No variant loaded, or symbol missing.");
        return NULL;
    }
    return getScreenUpdatePtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    moveMouse
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_moveMouse (JNIEnv * env, jclass class, jint dx, jint dy) {
    if (!moveMousePtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    moveMousePtr(dx, dy);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setMousePos
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setMousePos (JNIEnv * env, jclass class, jint x, jint y) {
    if (!setMousePosPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    setMousePosPtr(x, y);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setMouseButton
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setMouseButton (JNIEnv * env, jclass class, jboolean down) {
    if (!setMouseButtonPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    setMouseButtonPtr(down);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getMouseX
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_getMouseX (JNIEnv * env, jclass class) {
    if (!getMouseXPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return -1;
    }
    return getMouseXPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getMouseY
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core_getMouseY (JNIEnv * env, jclass class) {
    if (!getMouseYPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return -1;
    }
    return getMouseYPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getMouseButton
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_name_osher_gil_minivmac_Core_getMouseButton (JNIEnv * env, jclass class) {
    if (!getMouseButtonPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return 0;
    }
    return getMouseButtonPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setKeyDown
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setKeyDown (JNIEnv * env, jclass class, jint key) {
    if (!setKeyDownPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    setKeyDownPtr(key);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setKeyUp
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setKeyUp (JNIEnv * env, jclass class, jint key) {
    if (!setKeyUpPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    setKeyUpPtr(key);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setWantMacReset
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setWantMacReset (JNIEnv * env, jclass class) {
    if (!setWantMacResetPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    setWantMacResetPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setWantMacReset
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setWantMacInterrupt (JNIEnv * env, jclass class) {
    if (!setWantMacInterruptPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    setWantMacInterruptPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setRequestMacOff
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setRequestMacOff (JNIEnv * env, jclass class) {
    if (!setRequestMacOffPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    setRequestMacOffPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setForceMacOff
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core_setForceMacOff (JNIEnv * env, jclass class) {
    if (!setForceMacOffPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    setForceMacOffPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    _resumeEmulation
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core__1resumeEmulation (JNIEnv * env, jclass class) {
    if (!resumeEmulationPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    resumeEmulationPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    _pauseEmulation
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core__1pauseEmulation (JNIEnv * env, jclass class) {
    if (!pauseEmulationPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    pauseEmulationPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    isPaused
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_name_osher_gil_minivmac_Core_isPaused (JNIEnv * env, jclass class) {
    if (!isPausedPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return 0;
    }
    return isPausedPtr();
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    setSpeed
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_name_osher_gil_minivmac_Core__1setSpeed (JNIEnv * env, jclass class, jint value) {
    if (!setSpeedPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return;
    }
    setSpeedPtr(value);
}

/*
 * Class:     name_osher_gil_minivmac_Core
 * Method:    getSpeed
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_name_osher_gil_minivmac_Core__1getSpeed (JNIEnv * env, jclass class) {
    if (!getSpeedPtr) {
        LOGE("No variant loaded, or symbol missing.");
        return -1;
    }
    return getSpeedPtr();
}

static struct sigaction old_sa[NSIG];

void android_sigaction(int signal, siginfo_t *info, void *reserved)
{
    __android_log_print(ANDROID_LOG_ERROR, "NativeCrash", "Signal %d received", signal);
    old_sa[signal].sa_handler(signal);
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    // Try to catch crashes...
    struct sigaction handler;
    memset(&handler, 0, sizeof(sigaction));
    handler.sa_sigaction = android_sigaction;
    handler.sa_flags = SA_RESETHAND;
#define CATCHSIG(X) sigaction(X, &handler, &old_sa[X])
    CATCHSIG(SIGILL);
    CATCHSIG(SIGABRT);
    CATCHSIG(SIGBUS);
    CATCHSIG(SIGFPE);
    CATCHSIG(SIGSEGV);
    //CATCHSIG(SIGSTKFLT);
    CATCHSIG(SIGPIPE);

    return JNI_VERSION_1_2;
}
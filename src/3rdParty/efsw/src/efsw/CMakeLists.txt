project(efsw)
cmake_minimum_required(VERSION 2.6)

#set(BUILD_SHARED_LIBS 0)
#add_definitions(-static)

add_definitions(-D'EFSW_EXPORTS')

include_directories("../../include" "../")

set(SOURCE_FILES "Debug.cpp"
                 "DirectorySnapshot.cpp"
                 "DirectorySnapshotDiff.cpp"
                 "DirWatcherGeneric.cpp"
                 "FileInfo.cpp"
                 "FileSystem.cpp"
                 "FileWatcher.cpp"
                 "FileWatcherCWrapper.cpp"
                 "FileWatcherFSEvents.cpp"
                 "FileWatcherGeneric.cpp"
                 "FileWatcherImpl.cpp"
                 "FileWatcherKqueue.cpp"
                 "FileWatcherWin32.cpp"
                 "FileWatcherInotify.cpp"
                 "Log.cpp"
                 "Mutex.cpp"
                 "String.cpp"
                 "System.cpp"
                 "Thread.cpp"
                 "Watcher.cpp"
                 "WatcherFSEvents.cpp"
                 "WatcherGeneric.cpp"
                 "WatcherInotify.cpp"
                 "WatcherKqueue.cpp"
                 "WatcherWin32.cpp"
#if(APPLE)
                 "platform/posix/FileSystemImpl.cpp"
                 "platform/posix/MutexImpl.cpp"
                 "platform/posix/SystemImpl.cpp"
                 "platform/posix/ThreadImpl.cpp"
#endif(apple)

#if(WIN32)
                 "platform/win/FileSystemImpl.cpp"
                 "platform/win/MutexImpl.cpp"
                 "platform/win/SystemImpl.cpp"
                 "platform/win/ThreadImpl.cpp"
#endif(WIN32)
)
                 
add_library(efsw ${SOURCE_FILES})

set_target_properties(efsw PROPERTIES OUTPUT_NAME "efsw")

IF(UNIX)
	set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS -fPIC)
ENDIF(UNIX)
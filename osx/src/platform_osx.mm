#ifdef PLATFORM_OSX

#import <Foundation/Foundation.h>
#import <utility>
#import <cstdio>
#import <cstdarg>
#import <fstream>
#include <sys/resource.h>
#include <sys/syscall.h>

#include "platform.h"
#include "gl.h"

static bool s_isContinuousRendering = false;

NSURLSession* defaultSession;

void logMsg(const char* fmt, ...) {
    
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    
}

void requestRender() {
    
    glfwPostEmptyEvent();
    
}

void setContinuousRendering(bool _isContinuous) {
    
    s_isContinuousRendering = _isContinuous;
    
}

bool isContinuousRendering() {
    
    return s_isContinuousRendering;
    
}

NSString* resolveResourcePath(const char* _path) {
    
    NSString* resourcePath = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:@"/"];
    NSString* internalPath = [NSString stringWithUTF8String:_path];
    return [resourcePath stringByAppendingString:internalPath];
    
}

std::string stringFromResource(const char* _path) {
    
    NSString* path = resolveResourcePath(_path);
    NSString* str = [NSString stringWithContentsOfFile:path
                                              encoding:NSASCIIStringEncoding
                                                 error:NULL];
    
    if (str == nil) {
        logMsg("Failed to read file at path: %s\n", _path);
        return std::move(std::string());
    }
    
    return std::move(std::string([str UTF8String]));
}

unsigned char* bytesFromResource(const char* _path, unsigned int* _size) {

    NSString* path = resolveResourcePath(_path);
    std::ifstream resource([path UTF8String], std::ifstream::ate | std::ifstream::binary);

    if(!resource.is_open()) {
        logMsg("Failed to read file at path: %s\n", _path);
        *_size = 0;
        return nullptr;
    }

    *_size = resource.tellg();

    resource.seekg(std::ifstream::beg);

    char* cdata = (char*) malloc(sizeof(char) * (*_size));

    resource.read(cdata, *_size);
    resource.close();

    return reinterpret_cast<unsigned char *>(cdata);
}

void NSurlInit() {
    NSURLSessionConfiguration *defaultConfigObject = [NSURLSessionConfiguration defaultSessionConfiguration];
    NSString *cachePath = [NSTemporaryDirectory() stringByAppendingPathComponent:@"/tile_cache"];
    NSURLCache *tileCache = [[NSURLCache alloc] initWithMemoryCapacity: 4 * 1024 * 1024 diskCapacity: 30 * 1024 * 1024 diskPath: cachePath];
    defaultConfigObject.URLCache = tileCache;
    defaultConfigObject.requestCachePolicy = NSURLRequestUseProtocolCachePolicy;
    defaultConfigObject.timeoutIntervalForRequest = 30;
    defaultConfigObject.timeoutIntervalForResource = 60;
    
    defaultSession = [NSURLSession sessionWithConfiguration: defaultConfigObject];
}

bool startUrlRequest(const std::string& _url, UrlCallback _callback) {

    NSString* nsUrl = [NSString stringWithUTF8String:_url.c_str()];
    
    void (^handler)(NSData*, NSURLResponse*, NSError*) = ^void (NSData* data, NSURLResponse* response, NSError* error) {
        
        if(error == nil) {
            
            int dataLength = [data length];
            std::vector<char> rawDataVec;
            rawDataVec.resize(dataLength);
            memcpy(rawDataVec.data(), (char *)[data bytes], dataLength);
            _callback(std::move(rawDataVec));
            
        } else {
            
            logMsg("ERROR: response \"%s\" with error \"%s\".\n", response, std::string([error.localizedDescription UTF8String]).c_str());

        }
        
    };
    
    NSURLSessionDataTask* dataTask = [defaultSession dataTaskWithURL:[NSURL URLWithString:nsUrl] completionHandler:handler];
    
    [dataTask resume];
    
    return true;
    
}

void cancelUrlRequest(const std::string& _url) {
    
    NSString* nsUrl = [NSString stringWithUTF8String:_url.c_str()];
   
    [defaultSession getTasksWithCompletionHandler:^(NSArray* dataTasks, NSArray* uploadTasks, NSArray* downloadTasks) {
        for(NSURLSessionTask* task in dataTasks) {
            if([[task originalRequest].URL.absoluteString isEqualToString:nsUrl]) {
                [task cancel];
                break;
            }
        }
    }];
}

void setCurrentThreadPriority(int priority) {
    int tid = syscall(SYS_gettid);
    setpriority(PRIO_PROCESS, tid, priority);
}

#endif //PLATFORM_OSX

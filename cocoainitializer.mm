#import <AppKit/AppKit.h>

#include "cocoainitializer.h"



class CocoaInitializer::Private
{
  public:
    NSAutoreleasePool* autoReleasePool;
};

CocoaInitializer::CocoaInitializer()
{
  d = new CocoaInitializer::Private();
  NSApplicationLoad();
  d->autoReleasePool = [[NSAutoreleasePool alloc] init];
}

CocoaInitializer::~CocoaInitializer()
{
  [d->autoReleasePool release];
  delete d;
}

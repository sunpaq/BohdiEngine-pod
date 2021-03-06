//
//  BERunLoop.h
//  Pods
//
//  Created by Sun YuLi on 2017/4/22.
//
//

#include "TargetConditionals.h"
#if TARGET_OS_IOS

#import <Foundation/Foundation.h>
#import <GLKit/GLKit.h>

@interface BERunLoop : NSObject

-(instancetype) initWithTarget:(id)target Selector:(SEL)selector;
-(void) startRunloop;
-(void) stopRunloop;

@end

#endif

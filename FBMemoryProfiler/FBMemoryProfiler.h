/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the LICENSE file in
 * the root directory of this source tree.
 */

#import <Foundation/Foundation.h>

//! Project version number for FBMemoryProfiler.
FOUNDATION_EXPORT double FBMemoryProfilerVersionNumber;

//! Project version string for FBMemoryProfiler.
FOUNDATION_EXPORT const unsigned char FBMemoryProfilerVersionString[];

#import <UIKit/UIKit.h>

#import "FBMemoryProfilerPluggable.h"
#import "FBMemoryProfilerPresentationModeDelegate.h"

#import <FBRetainCycleDetector/FBObjectGraphConfiguration.h>

/**
 This will protect some internal parts that could use dangerous paths like private API's.
 Memory Profiler should be safe to use even without them, it's just going to have missing features.
 
 因为有一些地方使用了【私有API】，时不能上【AppStore】审核的，所以只能在【Debug】阶段使用【私有API】：
 - 1）如果 【有】#define FB_MEMORY_TOOLS xxx => #define _INTERNAL_IMP_ENABLED xxx
 - 2）如果【没有】#define FB_MEMORY_TOOLS xxx => #define _INTERNAL_IMP_ENABLED DEBUG
 
 所以默认也就是：#define _INTERNAL_IMP_ENABLED DEBUG => _INTERNAL_IMP_ENABLED = DEBUG
 
 #if _INTERNAL_IMP_ENABLED => #if DEBUG
 */
#ifdef FB_MEMORY_TOOLS
#define _INTERNAL_IMP_ENABLED (FB_MEMORY_TOOLS)
#else
#define _INTERNAL_IMP_ENABLED DEBUG
#endif // FB_MEMORY_TOOLS

/**

 FBMemoryProfilerPluggable describes plugin for memory profiler. Whenever we want to support
 additional behavior for memory profiler such as clearing caches on generation mark, or
 logging retain cycles to backend, we can create a plugin and attach it to memory profiler.
 
 ------------------------------------------------------------------------------------
 @protocol FBMemoryProfilerPluggable <NSObject>

 @optional

 Lifecycle events, when Memory profiler is enabled/disabled
 - (void)memoryProfilerDidEnable;
 - (void)memoryProfilerDidDisable;

 When memory profiler finds retain cycles plugins can subscribe to get them.
 - (void)memoryProfilerDidFindRetainCycles:(nonnull NSSet *)retainCycles;

 If you want to do additional cleanup after marking new generations.
 - (void)memoryProfilerDidMarkNewGeneration;

 @end
*/
@protocol FBMemoryProfilerPluggable;

/**
 FBMemoryProfiler is a tool helping with memory profiling on iOS. It leverages FBAllocationTracker
 and FBRetainCycleDetector. It can track all NSObject subclasses allocations. You can scan every
 class' instances for retain cycles. It also supports generations similar to Instruments, so you
 can observe objects created in between given generation marks.
 
 实现 FBMemoryProfilerPresentationModeDelegate 协议，完成UI展示的效果：
 -----------------------------------------------------------------------------------
 typedef NS_ENUM(NSUInteger, FBMemoryProfilerPresentationMode) {
     // Overlay window with main memory profiler interface
     FBMemoryProfilerPresentationModeFullWindow,
 
     // Small translucent button that can be dragged and dropped around
     FBMemoryProfilerPresentationModeCondensed,
     FBMemoryProfilerPresentationModeDisabled,
 };
 
 Presentation mode delegate owns memory profiler presentation and it can change it if requested.
 @protocol FBMemoryProfilerPresentationModeDelegate <NSObject>
 - (void)presentationDelegateChangePresentationModeToMode:(FBMemoryProfilerPresentationMode)mode;
 @end
 */
@interface FBMemoryProfiler : NSObject <FBMemoryProfilerPresentationModeDelegate>

/**
 Designated initializer
 @param plugins Plugins can take up some behavior like cache cleaning when we are working with profiler.
 Check FBMemoryProfilerPluggable for details.
 @param retainCycleDetectorConfiguration Retain cycle detector will use this configuration to determine how it should
 walk on object graph, check FBObjectGraphConfiguration to see what options you can define.
 @see FBObjectGraphConfiguration
 */
- (nonnull instancetype)initWithPlugins:(nullable NSArray<id<FBMemoryProfilerPluggable>> *)plugins
       retainCycleDetectorConfiguration:(nullable FBObjectGraphConfiguration *)retainCycleDetectorConfiguration NS_DESIGNATED_INITIALIZER;

/**
 Enable Memory Profiler. It will show a button that after clicking will bring up the memory profiler.
 */
- (void)enable;

/**
 Disable Memory Profiler. The window will be destroyed.
 */
- (void)disable;

/**
 Presentation mode for FBMemoryProfiler. It currently supports three presentation modes.
 It can be a small floating button, that we can keep around, or a full window for actual
 profiling, or it can simply be disabled.
 */
@property (nonatomic, assign) FBMemoryProfilerPresentationMode presentationMode;
@property (nonatomic, assign, getter=isEnabled) BOOL enabled;

@end

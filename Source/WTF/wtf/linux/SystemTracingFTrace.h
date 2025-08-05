/*
 * Copyright (C) 2024 Igalia, S.L.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#pragma once

#if USE(LINUX_FTRACE)

#include <wtf/Assertions.h>
#include <wtf/SystemTracing.h>
#include <wtf/text/ASCIILiteral.h>

#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <fcntl.h>
#include <unistd.h>

namespace WTF {

class SystemTracingFTrace {
public:
    static SystemTracingFTrace& instance() {
        static SystemTracingFTrace instance;
        return instance;
    }

    inline void tracePoint(TracePointCode code, uint64_t cookie) {
        // ftrace disabled in runtime
        if (m_traceMarkerFd < 0) return;

        switch (code) {
        case VMEntryScopeStart:
        case WebAssemblyCompileStart:
        case WebAssemblyExecuteStart:
        case DumpJITMemoryStart:
        case FromJSStart:
        case IncrementalSweepStart:
        case FetchCookiesStart:
        case StyleRecalcStart:
        case RenderTreeBuildStart:
        case PerformLayoutStart:
        case PaintLayerStart:
        case AsyncImageDecodeStart:
        case RAFCallbackStart:
        case MemoryPressureHandlerStart:
        case UpdateTouchRegionsStart:
        case DisplayListRecordStart:
        case ComputeEventRegionsStart:
        case RenderingUpdateStart:
        case CompositingUpdateStart:
        case DispatchTouchEventsStart:
        case ParseHTMLStart:
        case DisplayListReplayStart:
        case ScrollingThreadRenderUpdateSyncStart:
        case ScrollingThreadDisplayDidRefreshStart:
        case RenderTreeLayoutStart:
        case PerformOpportunisticallyScheduledTasksStart:
        case WebXRLayerStartFrameStart:
        case WebXRLayerEndFrameStart:
        case WebXRSessionFrameCallbacksStart:
        case WebHTMLViewPaintStart:
        case BackingStoreFlushStart:
        case BuildTransactionStart:
        case SyncMessageStart:
        case SyncTouchEventStart:
        case InitializeWebProcessStart:
        case RenderingUpdateRunLoopObserverStart:
        case LayerTreeFreezeStart:
        case FlushRemoteImageBufferStart:
        case CreateInjectedBundleStart:
        case PaintSnapshotStart:
        case RenderServerSnapshotStart:
        case TakeSnapshotStart:
        case SyntheticMomentumStart:
        case CommitLayerTreeStart:
        case ProcessLaunchStart:
        case InitializeSandboxStart:
        case WebXRCPFrameWaitStart:
        case WebXRCPFrameStartSubmissionStart:
        case WebXRCPFrameEndSubmissionStart:
        case WakeUpAndApplyDisplayListStart:
#if PLATFORM(GTK) || PLATFORM(WPE)
        case FlushPendingLayerChangesStart:
        case WaitForCompositionCompletionStart:
        case RenderLayerTreeStart:
        case LayerFlushStart:
        case UpdateLayerContentBuffersStart:
#endif
            beginSyncMark(code);
            return;

        case VMEntryScopeEnd:
        case WebAssemblyCompileEnd:
        case WebAssemblyExecuteEnd:
        case DumpJITMemoryStop:
        case FromJSStop:
        case IncrementalSweepEnd:
        case FetchCookiesEnd:
        case StyleRecalcEnd:
        case RenderTreeBuildEnd:
        case PerformLayoutEnd:
        case PaintLayerEnd:
        case AsyncImageDecodeEnd:
        case RAFCallbackEnd:
        case MemoryPressureHandlerEnd:
        case UpdateTouchRegionsEnd:
        case DisplayListRecordEnd:
        case ComputeEventRegionsEnd:
        case RenderingUpdateEnd:
        case CompositingUpdateEnd:
        case DispatchTouchEventsEnd:
        case ParseHTMLEnd:
        case DisplayListReplayEnd:
        case ScrollingThreadRenderUpdateSyncEnd:
        case ScrollingThreadDisplayDidRefreshEnd:
        case RenderTreeLayoutEnd:
        case PerformOpportunisticallyScheduledTasksEnd:
        case WebXRLayerStartFrameEnd:
        case WebXRLayerEndFrameEnd:
        case WebXRSessionFrameCallbacksEnd:
        case ProgrammaticScroll:
        case FixedContainerEdgeSamplingStart:
        case FixedContainerEdgeSamplingEnd:
        case WebHTMLViewPaintEnd:
        case BackingStoreFlushEnd:
        case BuildTransactionEnd:
        case SyncMessageEnd:
        case SyncTouchEventEnd:
        case InitializeWebProcessEnd:
        case RenderingUpdateRunLoopObserverEnd:
        case LayerTreeFreezeEnd:
        case FlushRemoteImageBufferEnd:
        case CreateInjectedBundleEnd:
        case PaintSnapshotEnd:
        case RenderServerSnapshotEnd:
        case TakeSnapshotEnd:
        case SyntheticMomentumEnd:
        case CommitLayerTreeEnd:
        case ProcessLaunchEnd:
        case InitializeSandboxEnd:
        case WebXRCPFrameWaitEnd:
        case WebXRCPFrameStartSubmissionEnd:
        case WebXRCPFrameEndSubmissionEnd:
        case WakeUpAndApplyDisplayListEnd:
#if PLATFORM(GTK) || PLATFORM(WPE)
        case FlushPendingLayerChangesEnd:
        case WaitForCompositionCompletionEnd:
        case RenderLayerTreeEnd:
        case LayerFlushEnd:
        case UpdateLayerContentBuffersEnd:
#endif
            endSyncMark(code);
            return;

        case MainResourceLoadDidStartProvisional:
        case SubresourceLoadWillStart:
            beginAsyncMark(code, cookie);
            return;

        case MainResourceLoadDidEnd:
        case SubresourceLoadDidEnd:
            endAsyncMark(code, cookie);
            return;

        case DisplayRefreshDispatchingToMainThread:
        case ScheduleRenderingUpdate:
        case TriggerRenderingUpdate:
        case ScrollingTreeDisplayDidRefresh:
        case SyntheticMomentumEvent:
        case RemoteLayerTreeScheduleRenderingUpdate:
        case DisplayLinkUpdate:
            instantMark(code);
            return;

        case WTFRange:
        case JavaScriptRange:
        case WebCoreRange:
        case WebKitRange:
        case WebKit2Range:
        case UIProcessRange:
        case GPUProcessRange:
#if PLATFORM(GTK) || PLATFORM(WPE)
        case GTKWPEPortRange:
#endif
            break;
        }

        WTFLogAlways("Invalid trace point code %d", code);
    }

    ~SystemTracingFTrace() {
        if (m_traceMarkerFd >= 0) {
            close(m_traceMarkerFd);
        }
    }

private:

    inline void beginSyncMark(TracePointCode code) {
        // "B|<pid>|<name>"
        std::string message = std::string("B|") + std::to_string(m_pid) + "|" + tracePointCodeName(code).characters();
        writeFTraceMarker(message.c_str());
    }

    inline void endSyncMark(TracePointCode code) {
        UNUSED_PARAM(code);
        // "E|<pid>"
        std::string message = std::string("E|") + std::to_string(m_pid);
        writeFTraceMarker(message.c_str());
    }

    inline void beginAsyncMark(TracePointCode code, uint64_t cookie) {
        // "S|<pid>|<name>|<cookie>"
        std::string message = std::string("S|") + std::to_string(m_pid) + "|" + tracePointCodeName(code).characters() + "|" + std::to_string(cookie);
        writeFTraceMarker(message.c_str());
    }

    inline void endAsyncMark(TracePointCode code, uint64_t cookie) {
        // "F|<pid>|<name>|<cookie>"
        std::string message = std::string("F|") + std::to_string(m_pid) + "|" + tracePointCodeName(code).characters() + "|" + std::to_string(cookie);
        writeFTraceMarker(message.c_str());
    }

    inline void instantMark(TracePointCode code) {
        // "I|<pid>|<name>"
        std::string message = std::string("I|") + std::to_string(m_pid) + "|" + tracePointCodeName(code).characters();
        writeFTraceMarker(message.c_str());
    }

    inline void writeFTraceMarker(const char* message) {
        RELEASE_ASSERT(m_traceMarkerFd >= 0);

        // make sure no other thread is writing at the same time
        std::lock_guard<std::mutex> lock(m_mutex);
        write(m_traceMarkerFd, message, strlen(message));
    }

    SystemTracingFTrace() {
        // Need to enable in runtime with WEBKIT_USE_FTRACE
        const char* env = getenv("WEBKIT_USE_FTRACE");
        if (!env || strcmp(env, "1") != 0) {
            return;
        }

        static constexpr const char* trace_marker = "/sys/kernel/debug/tracing/trace_marker";
        m_traceMarkerFd = open(trace_marker, O_WRONLY);
        if (m_traceMarkerFd < 0) {
            WTFLogAlways("Failed to open %s", trace_marker);
            return;
        }

        m_pid = getpid();
    }

    inline ASCIILiteral tracePointCodeName(TracePointCode code) {
        switch (code) {
        case VMEntryScopeStart:
        case VMEntryScopeEnd:
            return "VMEntryScope"_s;
        case WebAssemblyCompileStart:
        case WebAssemblyCompileEnd:
            return "WebAssemblyCompile"_s;
        case WebAssemblyExecuteStart:
        case WebAssemblyExecuteEnd:
            return "WebAssemblyExecute"_s;
        case DumpJITMemoryStart:
        case DumpJITMemoryStop:
            return "DumpJITMemory"_s;
        case FromJSStart:
        case FromJSStop:
            return "FromJS"_s;
        case IncrementalSweepStart:
        case IncrementalSweepEnd:
            return "IncrementalSweep"_s;

        case MainResourceLoadDidStartProvisional:
        case MainResourceLoadDidEnd:
            return "MainResourceLoad"_s;
        case SubresourceLoadWillStart:
        case SubresourceLoadDidEnd:
            return "SubresourceLoad"_s;
        case FetchCookiesStart:
        case FetchCookiesEnd:
            return "FetchCookies"_s;
        case StyleRecalcStart:
        case StyleRecalcEnd:
            return "StyleRecalc"_s;
        case RenderTreeBuildStart:
        case RenderTreeBuildEnd:
            return "RenderTreeBuild"_s;
        case PerformLayoutStart:
        case PerformLayoutEnd:
            return "PerformLayout"_s;
        case PaintLayerStart:
        case PaintLayerEnd:
            return "PaintLayer"_s;
        case AsyncImageDecodeStart:
        case AsyncImageDecodeEnd:
            return "AsyncImageDecode"_s;
        case RAFCallbackStart:
        case RAFCallbackEnd:
            return "RAFCallback"_s;
        case MemoryPressureHandlerStart:
        case MemoryPressureHandlerEnd:
            return "MemoryPressureHandler"_s;
        case UpdateTouchRegionsStart:
        case UpdateTouchRegionsEnd:
            return "UpdateTouchRegions"_s;
        case DisplayListRecordStart:
        case DisplayListRecordEnd:
            return "DisplayListRecord"_s;
        case DisplayRefreshDispatchingToMainThread:
            return "DisplayRefreshDispatchingToMainThread"_s;
        case ComputeEventRegionsStart:
        case ComputeEventRegionsEnd:
            return "ComputeEventRegions"_s;
        case ScheduleRenderingUpdate:
            return "ScheduleRenderingUpdate"_s;
        case TriggerRenderingUpdate:
            return "TriggerRenderingUpdate"_s;
        case RenderingUpdateStart:
        case RenderingUpdateEnd:
            return "RenderingUpdate"_s;
        case CompositingUpdateStart:
        case CompositingUpdateEnd:
            return "CompositingUpdate"_s;
        case DispatchTouchEventsStart:
        case DispatchTouchEventsEnd:
            return "DispatchTouchEvents"_s;
        case ParseHTMLStart:
        case ParseHTMLEnd:
            return "ParseHTML"_s;
        case DisplayListReplayStart:
        case DisplayListReplayEnd:
            return "DisplayListReplay"_s;
        case ScrollingThreadRenderUpdateSyncStart:
        case ScrollingThreadRenderUpdateSyncEnd:
            return "ScrollingThreadRenderUpdateSync"_s;
        case ScrollingThreadDisplayDidRefreshStart:
        case ScrollingThreadDisplayDidRefreshEnd:
            return "ScrollingThreadDisplayDidRefresh"_s;
        case ScrollingTreeDisplayDidRefresh:
            return "ScrollingTreeDisplayDidRefresh"_s;
        case RenderTreeLayoutStart:
        case RenderTreeLayoutEnd:
            return "RenderTreeLayout"_s;
        case PerformOpportunisticallyScheduledTasksStart:
        case PerformOpportunisticallyScheduledTasksEnd:
            return "PerformOpportunisticallyScheduledTasks"_s;
        case WebXRLayerStartFrameStart:
        case WebXRLayerStartFrameEnd:
            return "WebXRLayerStartFrame"_s;
        case WebXRLayerEndFrameStart:
        case WebXRLayerEndFrameEnd:
            return "WebXRLayerEndFrame"_s;
        case WebXRSessionFrameCallbacksStart:
        case WebXRSessionFrameCallbacksEnd:
            return "WebXRSessionFrameCallbacks"_s;
        case ProgrammaticScroll:
            return "ProgrammaticScroll"_s;
        case FixedContainerEdgeSamplingStart:
        case FixedContainerEdgeSamplingEnd:
            return "FixedContainerEdgeSampling"_s;

        case WebHTMLViewPaintStart:
        case WebHTMLViewPaintEnd:
            return "WebHTMLViewPaint"_s;

        case BackingStoreFlushStart:
        case BackingStoreFlushEnd:
            return "BackingStoreFlush"_s;
        case BuildTransactionStart:
        case BuildTransactionEnd:
            return "BuildTransaction"_s;
        case SyncMessageStart:
        case SyncMessageEnd:
            return "SyncMessage"_s;
        case SyncTouchEventStart:
        case SyncTouchEventEnd:
            return "SyncTouchEvent"_s;
        case InitializeWebProcessStart:
        case InitializeWebProcessEnd:
            return "InitializeWebProcess"_s;
        case RenderingUpdateRunLoopObserverStart:
        case RenderingUpdateRunLoopObserverEnd:
            return "RenderingUpdateRunLoopObserver"_s;
        case LayerTreeFreezeStart:
        case LayerTreeFreezeEnd:
            return "LayerTreeFreeze"_s;
        case FlushRemoteImageBufferStart:
        case FlushRemoteImageBufferEnd:
            return "FlushRemoteImageBuffer"_s;
        case CreateInjectedBundleStart:
        case CreateInjectedBundleEnd:
            return "CreateInjectedBundle"_s;
        case PaintSnapshotStart:
        case PaintSnapshotEnd:
            return "PaintSnapshot"_s;
        case RenderServerSnapshotStart:
        case RenderServerSnapshotEnd:
            return "RenderServerSnapshot"_s;
        case TakeSnapshotStart:
        case TakeSnapshotEnd:
            return "TakeSnapshot"_s;
        case SyntheticMomentumStart:
        case SyntheticMomentumEnd:
            return "SyntheticMomentum"_s;
        case SyntheticMomentumEvent:
            return "SyntheticMomentumEvent"_s;
        case RemoteLayerTreeScheduleRenderingUpdate:
            return "RemoteLayerTreeScheduleRenderingUpdate"_s;
        case DisplayLinkUpdate:
            return "DisplayLinkUpdate"_s;

        case CommitLayerTreeStart:
        case CommitLayerTreeEnd:
            return "CommitLayerTree"_s;
        case ProcessLaunchStart:
        case ProcessLaunchEnd:
            return "ProcessLaunch"_s;
        case InitializeSandboxStart:
        case InitializeSandboxEnd:
            return "InitializeSandbox"_s;
        case WebXRCPFrameWaitStart:
        case WebXRCPFrameWaitEnd:
            return "WebXRCPFrameWait"_s;
        case WebXRCPFrameStartSubmissionStart:
        case WebXRCPFrameStartSubmissionEnd:
            return "WebXRCPFrameStartSubmission"_s;
        case WebXRCPFrameEndSubmissionStart:
        case WebXRCPFrameEndSubmissionEnd:
            return "WebXRCPFrameEndSubmission"_s;

        case WakeUpAndApplyDisplayListStart:
        case WakeUpAndApplyDisplayListEnd:
            return "WakeUpAndApplyDisplayList"_s;

#if PLATFORM(GTK) || PLATFORM(WPE)
        case FlushPendingLayerChangesStart:
        case FlushPendingLayerChangesEnd:
            return "FlushPendingLayerChanges"_s;
        case WaitForCompositionCompletionStart:
        case WaitForCompositionCompletionEnd:
            return "WaitForCompositionCompletion"_s;
        case RenderLayerTreeStart:
        case RenderLayerTreeEnd:
            return "RenderLayerTree"_s;
        case LayerFlushStart:
        case LayerFlushEnd:
            return "LayerFlush"_s;
        case UpdateLayerContentBuffersStart:
        case UpdateLayerContentBuffersEnd:
            return "UpdateLayerContentBuffers"_s;
#endif

        // Markers, not intended to be used in tracePoint calls.
        case WTFRange:
        case JavaScriptRange:
        case WebCoreRange:
        case WebKitRange:
        case WebKit2Range:
        case UIProcessRange:
        case GPUProcessRange:
#if PLATFORM(GTK) || PLATFORM(WPE)
        case GTKWPEPortRange:
#endif
            break;
        }
        WTFLogAlways("Invalid trace point code %d", code);
        return ""_s;
    }

    SystemTracingFTrace(const SystemTracingFTrace&) = delete;
    SystemTracingFTrace& operator=(const SystemTracingFTrace&) = delete;

private:
    int m_traceMarkerFd = -1;
    std::mutex m_mutex;
    unsigned m_pid = -1;
};

} // namespace WTF

#endif // USE(LINUX_FTRACE)
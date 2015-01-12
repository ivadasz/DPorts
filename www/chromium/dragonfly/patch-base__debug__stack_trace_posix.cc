--- base/debug/stack_trace_posix.cc.orig	2015-01-11 10:25:43 UTC
+++ base/debug/stack_trace_posix.cc
@@ -22,7 +22,9 @@
 #if defined(__GLIBCXX__)
 #include <cxxabi.h>
 #endif
-#if !defined(__UCLIBC__)
+#if defined(__DragonFly__)
+#include <unwind.h>
+#elif !defined(__UCLIBC__)
 #include <execinfo.h>
 #endif
 
@@ -50,6 +52,37 @@
 
 namespace {
 
+#if defined(__DragonFly__)
+struct StackCrawlState {
+  StackCrawlState(uintptr_t* frames, size_t max_depth)
+      : frames(frames),
+        frame_count(0),
+        max_depth(max_depth),
+        have_skipped_self(false) {}
+
+  uintptr_t* frames;
+  size_t frame_count;
+  size_t max_depth;
+  bool have_skipped_self;
+};
+
+_Unwind_Reason_Code TraceStackFrame(_Unwind_Context* context, void* arg) {
+  StackCrawlState* state = static_cast<StackCrawlState*>(arg);
+  uintptr_t ip = _Unwind_GetIP(context);
+
+  // The first stack frame is this function itself.  Skip it.
+  if (ip != 0 && !state->have_skipped_self) {
+    state->have_skipped_self = true;
+    return _URC_NO_REASON;
+  }
+
+  state->frames[state->frame_count++] = ip;
+  if (state->frame_count >= state->max_depth)
+    return _URC_END_OF_STACK;
+  return _URC_NO_REASON;
+}
+#endif
+
 volatile sig_atomic_t in_signal_handler = 0;
 
 #if !defined(USE_SYMBOLIZE) && defined(__GLIBCXX__)
@@ -745,7 +778,11 @@
   // NOTE: This code MUST be async-signal safe (it's used by in-process
   // stack dumping signal handler). NO malloc or stdio is allowed here.
 
-#if !defined(__UCLIBC__)
+#if defined(__DragonFly__)
+  StackCrawlState state(reinterpret_cast<uintptr_t*>(trace_), kMaxTraces);
+  _Unwind_Backtrace(&TraceStackFrame, &state);
+  count_ = state.frame_count;
+#elif !defined(__UCLIBC__)
   // Though the backtrace API man page does not list any possible negative
   // return values, we take no chance.
   count_ = base::saturated_cast<size_t>(backtrace(trace_, arraysize(trace_)));

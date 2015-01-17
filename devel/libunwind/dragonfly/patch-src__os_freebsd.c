--- src/os-freebsd.c.orig	2015-01-17 20:23:51.665515000 +0100
+++ src/os-freebsd.c	2015-01-17 20:24:54.225552000 +0100
@@ -79,8 +79,8 @@
   pid = -1;
   for (i = 0, kv = (struct kinfo_proc *)buf; i < len / sizeof(*kv);
    i++, kv++) {
-    if (kv->ki_tid == tid) {
-      pid = kv->ki_pid;
+    if (kv->kp_lwp.kl_tid == tid) {
+      pid = kv->kp_lwp.kl_pid;
       break;
     }
   }

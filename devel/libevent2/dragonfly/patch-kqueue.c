--- kqueue.c.orig	2015-01-05 17:17:40.000000000 +0200
+++ kqueue.c
@@ -49,7 +49,7 @@
 /* Some platforms apparently define the udata field of struct kevent as
  * intptr_t, whereas others define it as void*.  There doesn't seem to be an
  * easy way to tell them apart via autoconf, so we need to use OS macros. */
-#if defined(_EVENT_HAVE_INTTYPES_H) && !defined(__OpenBSD__) && !defined(__FreeBSD__) && !defined(__darwin__) && !defined(__APPLE__)
+#if defined(_EVENT_HAVE_INTTYPES_H) && !defined(__OpenBSD__) && !defined(__DragonFly__) && !defined(__FreeBSD__) && !defined(__darwin__) && !defined(__APPLE__)
 #define PTR_TO_UDATA(x)	((intptr_t)(x))
 #define INT_TO_UDATA(x) ((intptr_t)(x))
 #else

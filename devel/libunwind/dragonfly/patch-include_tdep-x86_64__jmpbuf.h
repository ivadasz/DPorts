--- include/tdep-x86_64/jmpbuf.h.orig	2015-01-13 21:00:36 UTC
+++ include/tdep-x86_64/jmpbuf.h
@@ -32,7 +32,7 @@
 #define JB_MASK_SAVED	8
 #define JB_MASK		9
 
-#elif defined __FreeBSD__
+#elif defined __FreeBSD__ || defined __DragonFly__
 
 #define JB_SP		2
 #define JB_RP		0

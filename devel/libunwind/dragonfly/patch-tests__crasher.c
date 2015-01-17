--- crasher.c.orig	2015-01-17 20:58:15.554507000 +0100
+++ crasher.c	2015-01-17 20:58:52.645097000 +0100
@@ -6,7 +6,7 @@
 #include <stdint.h>
 #include <stdlib.h>
 #include <unistd.h>
-#ifdef __FreeBSD__
+#if defined(__FreeBSD__) || defined(__DragonFly__)
 #include <sys/types.h>
 #include <sys/sysctl.h>
 #include <sys/user.h>
@@ -40,7 +40,7 @@
     fclose(out);
     fclose(maps);
 }
-#elif defined(__FreeBSD__)
+#elif defined(__FreeBSD__) || defined(__DragonFly__)
 void
 write_maps(char *fname)
 {

--- src/coredump/_UCD_create.c.orig	2015-01-13 17:15:45.170940000 +0100
+++ src/coredump/_UCD_create.c	2015-01-13 17:16:39.161792000 +0100
@@ -61,6 +61,8 @@
 #endif
 
 #include <elf.h>
+/* Add typedef that is missing in DragonFly */
+typedef Elf_Note	Elf32_Nhdr;
 #include <sys/procfs.h> /* struct elf_prstatus */
 
 #include "_UCD_lib.h"

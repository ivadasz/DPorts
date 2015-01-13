--- src/ptrace/_UPT_reg_offset.c.orig	2015-01-13 20:12:44 UTC
+++ src/ptrace/_UPT_reg_offset.c
@@ -249,7 +249,7 @@
 
     [UNW_HPPA_IP]	= 0x1a8		/* IAOQ[0] */
 #elif defined(UNW_TARGET_X86)
-#if defined __FreeBSD__
+#if defined __FreeBSD__  || defined __DragonFly__
 #define UNW_R_OFF(R, r) \
     [UNW_X86_##R]	= offsetof(gregset_t, r_##r),
     UNW_R_OFF(EAX, eax)
@@ -286,7 +286,7 @@
 #error Port me
 #endif
 #elif defined(UNW_TARGET_X86_64)
-#if defined __FreeBSD__
+#if defined __FreeBSD__  || defined __DragonFly__
 #define UNW_R_OFF(R, r) \
     [UNW_X86_64_##R]	= offsetof(gregset_t, r_##r),
     UNW_R_OFF(RAX, rax)

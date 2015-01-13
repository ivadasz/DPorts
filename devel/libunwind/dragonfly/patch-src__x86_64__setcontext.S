--- src/x86_64/setcontext.S.orig	2015-01-13 20:14:39 UTC
+++ src/x86_64/setcontext.S
@@ -29,7 +29,7 @@
 #include <asm/unistd.h>
 #define	SIG_SETMASK   2
 #define	SIGSET_BYTE_SIZE   (64/8)
-#elif defined __FreeBSD__
+#elif defined __FreeBSD__ || defined __DragonFly__
 #include <sys/syscall.h>
 #endif
 
@@ -60,7 +60,7 @@
 	mov    UC_MCONTEXT_FPREGS_PTR(%rdi),%r8
 	fldenv (%r8)
 	ldmxcsr FPREGS_OFFSET_MXCSR(%r8)
-#elif defined __FreeBSD__
+#elif defined __FreeBSD__ || defined __DragonFly__
 	/* restore signal mask */
 	pushq	%rdi
 	xorl	%edx,%edx

--- src/x86_64/getcontext.S.orig	2015-01-13 20:16:13 UTC
+++ src/x86_64/getcontext.S
@@ -64,7 +64,7 @@
 	movq %r8, UC_MCONTEXT_FPREGS_PTR(%rdi)
 	fnstenv (%r8)
 	stmxcsr FPREGS_OFFSET_MXCSR(%r8)
-#elif defined __FreeBSD__
+#elif defined __FreeBSD__ || defined __DragonFly__
 	fxsave UC_MCONTEXT_FPSTATE(%rdi)
 	movq $UC_MCONTEXT_FPOWNED_FPU,UC_MCONTEXT_OWNEDFP(%rdi)
 	movq $UC_MCONTEXT_FPFMT_XMM,UC_MCONTEXT_FPFORMAT(%rdi)

--- src/setjmp/siglongjmp.c.orig	2015-01-13 21:03:48 UTC
+++ src/setjmp/siglongjmp.c
@@ -103,7 +103,7 @@
 		|| (_NSIG > 8 * sizeof (unw_word_t)
 		    && unw_set_reg (&c, UNW_REG_EH + 3, wp[JB_MASK + 1]) < 0))
 	      abort ();
-#elif defined(__FreeBSD__)
+#elif defined(__FreeBSD__) || defined(__DragonFly__)
 	  if (unw_set_reg (&c, UNW_REG_EH + 2, &wp[JB_MASK]) < 0)
 	      abort();
 #else


$FreeBSD: head/games/uhexen/files/patch-src::h2_main.c 340725 2014-01-22 17:40:44Z mat $

--- src/h2_main.c.orig	Tue Dec  4 18:11:47 2001
+++ src/h2_main.c	Mon Feb 10 16:56:31 2003
@@ -127,7 +127,7 @@
 static char *wadfiles[MAXWADFILES] =
 {
 	"hexen.wad",
-	"/usr/local/share/games/uhexen/hexen.wad"
+	PREFIX "/share/doom/hexen.wad"
 };
 #else
 static char *wadfiles[MAXWADFILES] =

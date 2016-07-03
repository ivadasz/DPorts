# $FreeBSD$

PORTNAME=	xf86-input-libinput
PORTVERSION=	0.19.0
EXTRACT_SUFX=	.tar.gz
CATEGORIES=	x11-drivers

MAINTAINER=	x11@FreeBSD.org
COMMENT=	X.Org libinput input driver

CONFIGURE_ARGS=	LIBINPUT_CFLAGS=-I${PREFIX}/include LIBINPUT_LIBS="-L${PREFIX}/lib -linput"

XORG_CAT=	driver

.include <bsd.port.mk>

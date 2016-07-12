PORTNAME=	libinput
PORTVERSION=	0.1
DISTNAME=	dragonfly_libinput-${PORTVERSION}
CATEGORIES=	devel
MASTER_SITES=	http://dfly.vdsz.de/dragonfly_libinput/			\
		http://leaf.dragonflybsd.org/~ivadasz/dragonfly_libinput/

MAINTAINER=	imre@vdsz.com
COMMENT=	Input event library

LICENSE=	MIT

BUILD_DEPENDS=	${LOCALBASE}/include/linux/input.h:multimedia/v4l_compat

post-install:
	@${MKDIR} ${STAGEDIR}/libdata/pkgconfig
	${INSTALL_DATA} ${WRKSRC}/libinput.pc ${STAGEDIR}/${PREFIX}/libdata/pkgconfig

.include <bsd.port.mk>

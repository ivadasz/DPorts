# Created by: Matt Dawson <matt@mattsnetwork.co.uk>
# $FreeBSD$

PORTNAME=	gpredict
PORTVERSION=	1.3
CATEGORIES=	comms hamradio
MASTER_SITES=	SF/${PORTNAME}/Gpredict/${PORTVERSION}

MAINTAINER=	hamradio@FreeBSD.org
COMMENT=	Amateur Radio satellite tracking and prediction

LICENSE=	GPLv2 # (or later)

LIB_DEPENDS=	libhamlib.so:${PORTSDIR}/comms/hamlib \
		libcurl.so:${PORTSDIR}/ftp/curl \
		libgoocanvas.so:${PORTSDIR}/graphics/goocanvas

GNU_CONFIGURE=	yes
CPPFLAGS+=	-I${LOCALBASE}/include
LIBS+=		-L${LOCALBASE}/lib
USES=		gettext gmake pkgconfig
USE_GNOME=	gtk20

post-patch:
	@${REINPLACE_CMD} -e \
		'/^Icon/s|=.*|=${PREFIX}/share/pixmaps/gpredict-icon.png|' \
		${WRKSRC}/data/desktop/gpredict.desktop.in

.include <bsd.port.mk>

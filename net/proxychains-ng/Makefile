# Created by: Rusmir Dusko <nemysis@FreeBSD.org>
# $FreeBSD$

PORTNAME=	proxychains-ng
PORTVERSION=	4.10
CATEGORIES=	net security
MASTER_SITES=	SF/${PORTNAME}
DISTNAME=	${PORTNAME:S/-ng//}-${PORTVERSION}

MAINTAINER=	nemysis@FreeBSD.org
COMMENT=	Redirect connection through proxy servers

LICENSE=	GPLv2

USES=		gmake tar:bzip2
GNU_CONFIGURE=	yes
USE_LDCONFIG=	yes

CONFLICTS=	proxychains-[0-9]*

PORTDOCS=	AUTHORS README
OPTIONS_DEFINE=	DOCS

post-configure:
	@${ECHO_CMD} "PXCHAINS=proxychains" >> ${WRKSRC}/config.mak
	@${ECHO_CMD} "LDSO_PATHNAME=libproxychains-4.so" >> ${WRKSRC}/config.mak

post-patch:
	@${REINPLACE_CMD} \
		-e 's|libproxychains4.so|libproxychains-4.so|' \
		${WRKSRC}/src/proxyresolv

post-install:
	${INSTALL_SCRIPT} ${WRKSRC}/src/proxyresolv ${STAGEDIR}${PREFIX}/bin/
	${INSTALL_DATA} ${WRKSRC}/src/proxychains.conf \
		${STAGEDIR}${PREFIX}/etc/proxychains.conf.sample

	@${MKDIR} ${STAGEDIR}${DOCSDIR}
	${INSTALL_DATA} ${PORTDOCS:S|^|${WRKSRC}/|} ${STAGEDIR}${DOCSDIR}

	${STRIP_CMD} ${STAGEDIR}${PREFIX}/bin/proxychains
	${STRIP_CMD} ${STAGEDIR}${PREFIX}/lib/libproxychains-4.so

.include <bsd.port.mk>

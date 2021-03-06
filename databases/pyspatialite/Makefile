# Created by: lbartoletti <coder@tuxfamily.org>
# $FreeBSD$

PORTNAME=	pyspatialite
PORTVERSION=	3.0.1
PORTREVISION=	6
CATEGORIES=	databases python
MASTER_SITES=	CHEESESHOP
PKGNAMEPREFIX=	${PYTHON_PKGNAMEPREFIX}
DISTNAME=	${PORTNAME}-${PORTVERSION}-alpha-0

MAINTAINER=	coder@tuxfamily.org
COMMENT=	DB-API 2.0 interface for SQLite 3.x with Spatialite

LICENSE=	ZLIB
LICENSE_FILE=	${WRKSRC}/LICENSE

LIB_DEPENDS=	libspatialite.so:${PORTSDIR}/databases/spatialite \
		libgeos_c.so:${PORTSDIR}/graphics/geos \
		libproj.so:${PORTSDIR}/graphics/proj \
		libfreexl.so:${PORTSDIR}/textproc/freexl

OPTIONS_DEFINE=	DOCS

USES=		iconv python
USE_PYTHON=	autoplist concurrent distutils

.include <bsd.port.pre.mk>

.if empty(ICONV_LIB)
ICONV_APPEND=
.else
ICONV_APPEND=	,iconv
.endif

post-patch:
	@${REINPLACE_CMD} -e "s|pyspatialite-doc|${DOCSDIR}|g" ${WRKSRC}/setup.py
	@${REINPLACE_CMD} -e "s|%%LOCALBASE%%|${LOCALBASE}|g" \
		-e "s|%%ICONV%%|${ICONV_APPEND}|" ${WRKSRC}/setup.cfg

post-install:
	${STRIP_CMD} ${STAGEDIR}${PYTHONPREFIX_SITELIBDIR}/pyspatialite/_spatialite.so

.include <bsd.port.post.mk>

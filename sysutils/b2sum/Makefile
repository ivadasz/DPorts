# $FreeBSD$

PORTNAME=	b2sum
PORTVERSION=	0.0.d${B2VERSION}
PORTREVISION=	1
CATEGORIES=	sysutils security
MASTER_SITES=	http://blake2.net/
DISTNAME=	blake2_code_${B2VERSION}${EXTRACT_SUFFX}

MAINTAINER=	matthew@reztek.cz
COMMENT=	Fast secure hashing

LICENSE=	CC0-1.0
LICENSE_FILE=	${WRKSRC}/../COPYING

B2VERSION=	20150529

USES=		zip
USE_CSTD=	c99
WRKSRC_SUBDIR=	b2sum
MAKEFILE=	makefile
MAKE_ARGS=	CC="${CC}" CFLAGS="${CFLAGS}"
PLIST_FILES=	bin/b2sum

OPTIONS_DEFINE=	OPENMP OPTIMIZED_CFLAGS SSE
OPTIONS_DEFAULT_x86_64=SSE
SSE_DESC=	Use SSE2, SSSE3, SSE4.1, AVX or XOP instructions

OPENMP_USES+=	compiler:openmp
OPENMP_CFLAGS+=	-fopenmp

.include <bsd.port.options.mk>

.if ${PORT_OPTIONS:MOPTIMIZED_CFLAGS}
CFLAGS+=	-O3
.if !${CFLAGS:M-march=*}
CFLAGS+=	-march=native
.endif
.endif

.if (${PORT_OPTIONS:MSSE} && (${ARCH} == "i386" || ${ARCH} == "x86_64"))
CFLAGS+=	-I../sse
.else
CFLAGS+=	-I../ref
EXTRA_PATCHES=	${PATCHDIR}/use_ref.patch
.endif

do-install:
	${INSTALL_PROGRAM} ${WRKSRC}/${PORTNAME} ${STAGEDIR}${PREFIX}/bin

.include <bsd.port.mk>

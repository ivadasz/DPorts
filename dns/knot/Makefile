# Created by: Leo Vandewoestijne <freebsd@dns-lab.com>
# $FreeBSD$

PORTNAME=	knot
PORTVERSION=	1.6.3
PORTREVISION=	1
CATEGORIES=	dns ipv6
MASTER_SITES=	https://secure.nic.cz/files/knot-dns/ \
		http://dns-lab.com/downloads/knot-dns/

MAINTAINER=	freebsd@dns-lab.com
COMMENT=	High performance authoritative-only DNS server

LICENSE=	GPLv3

BUILD_DEPENDS=	flex>=2.5.35_1:${PORTSDIR}/textproc/flex
LIB_DEPENDS=	liburcu.so:${PORTSDIR}/sysutils/liburcu

CONFLICTS=	knot-2.*

USES=		alias bison libtool pkgconfig tar:xz
GNU_CONFIGURE=	yes
CONFIGURE_ARGS=	--with-storage=/var/db/knot \
		--with-rundir=/var/run/knot \
		--with-openssl=${OPENSSLBASE}
USE_LDCONFIG=	yes
USE_OPENSSL=	yes
WITH_OPENSSL_PORT=	yes

.if defined(BATCH) || defined(PACKAGE_BUILDING)
CONFIGURE_ARGS+=	--disable-silent-rules
.endif

INSTALL_TARGET=	install-strip

USE_RC_SUBR=	${PORTNAME}
SUB_FILES=	pkg-message

USERS=		knot
GROUPS=		knot
SUB_LIST+=	USERS="${USERS}" GROUPS="${GROUPS}"

PORTDOCS=	AUTHORS COPYING ChangeLog INSTALL NEWS README THANKS

OPTIONS_DEFINE=	DNSTAP FASTPARSER IDN LMDB

DNSTAP_DESC=		dnstap support (see dnstap.info)
FASTPARSER_DESC=	Fast zone parser (demanding compilation)
GOST_DESC=		Enable GOST ciphers (requires OpenSSL >= 1.0.0)
LMDB_DESC=		Enable LMDB backend

DNSTAP_CONFIGURE_ENABLE=	dnstap
DNSTAP_LIB_DEPENDS=	libfstrm.so:${PORTSDIR}/devel/fstrm \
			libprotobuf-c.so:${PORTSDIR}/devel/protobuf-c

FASTPARSER_CONFIGURE_ENABLE=	fastparser

IDN_CONFIGURE_WITH=	libidn
IDN_LIB_DEPENDS=	libidn.so:${PORTSDIR}/dns/libidn

LMDB_CONFIGURE_ENABLE=	lmdb
LMDB_LIB_DEPENDS=	liblmdb.so:${PORTSDIR}/databases/lmdb

.include <bsd.port.options.mk>

post-patch:
	@${REINPLACE_CMD} 's|$$(INSTALL) -d $$(DESTDIR)/\@run_dir\@|#$$(INSTALL) -d $$(DESTDIR)/\@run_dir\@|' \
		${WRKSRC}/src/Makefile.in
	@${REINPLACE_CMD} 's|$$(INSTALL) -d $$(DESTDIR)/\@storage_dir\@|#$$(INSTALL) -d $$(DESTDIR)/\@storage_dir\@|' \
		${WRKSRC}/src/Makefile.in
	@${RM} -rf ${WRKSRC}/src/zscanner/scanner.c

post-install:
	${MV} ${STAGEDIR}${ETCDIR}/knot.sample.conf \
		${STAGEDIR}${ETCDIR}/knot.conf.sample
	@${MKDIR} ${STAGEDIR}${DOCSDIR}
	${INSTALL_DATA} ${PORTDOCS:S,^,${WRKSRC}/,} ${STAGEDIR}${DOCSDIR}/

.include <bsd.port.mk>

# $FreeBSD$

PORTNAME=	phabricator
PORTVERSION=	20150602
CATEGORIES=	devel
PKGNAMEPREFIX=	php5-

MAINTAINER=	grembo@FreeBSD.org
COMMENT=	Open source, software engineering platform

LICENSE=	APACHE20
LICENSE_FILE=	${WRKSRC}/LICENSE

RUN_DEPENDS=	php5-libphutil>0:${PORTSDIR}/devel/libphutil \
		php5-arcanist>0:${PORTSDIR}/devel/arcanist \
		git:${PORTSDIR}/devel/git

OPTIONS_DEFINE=	GD OPCACHE
OPTIONS_DEFAULT=OPCACHE
OPCACHE_DESC?=	Use OPcache/APCu (APC on PHP < 5.5) to improve performance

USE_PHP=	ctype curl fileinfo hash iconv filter mbstring mysql openssl \
		pcntl posix

GD_USE=		PHP=gd

.include <bsd.port.pre.mk>

.if ${PORT_OPTIONS:MOPCACHE}
.	if ${PHP_EXT_DIR:C|[^0-9]||g}  >= 20121212
USE_PHP+=	opcache
RUN_DEPENDS+=	 ${PHPBASE}/lib/php/${PHP_EXT_DIR}/apcu.so:${PORTSDIR}/devel/pecl-APCu
.	else
USE_PHP+=	apc
.	endif
.endif

USE_GITHUB=	yes
NO_BUILD=	yes
USE_RC_SUBR=	phd
SUB_FILES=	pkg-message
GH_ACCOUNT=	phacility
GH_TAGNAME=	55c73b2

PHP_DESTDIR=	lib/php/phabricator

do-install:
	@${MKDIR} ${STAGEDIR}${PREFIX}/${PHP_DESTDIR}
	cd ${WRKSRC} ; ${PAX} -rw * ${STAGEDIR}${PREFIX}/${PHP_DESTDIR}

.include <bsd.port.post.mk>

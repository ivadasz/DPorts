# $FreeBSD$

PORTNAME=	postgis
PORTVERSION=	2.1.7
CATEGORIES=	databases geography
MASTER_SITES=	http://download.osgeo.org/postgis/source/

MAINTAINER=	harrison.grundy@astrodoggroup.com
COMMENT=	Geographic objects support for PostgreSQL databases

LICENSE=	GPLv2

LIB_DEPENDS=	libproj.so:${PORTSDIR}/graphics/proj \
		libgeos.so:${PORTSDIR}/graphics/geos \
		libjson-c.so:${PORTSDIR}/devel/json-c
RUN_DEPENDS=	postmaster:${PORTSDIR}/databases/postgresql${PGSQL_VER_NODOT}-server

CONFLICTS=	postgis-1.* postgis-2.0.*

GNU_CONFIGURE=	yes
CONFIGURE_ARGS=	${ICONV_CONFIGURE_BASE}
USES=		gmake iconv:wchar_t perl5 pgsql pkgconfig libtool:keepla
USE_GNOME=	libxml2
USE_LDCONFIG=	yes
USE_PERL5=	build

OPTIONS_DEFINE=		LOADERGUI RASTER TOPOLOGY SFCGAL
OPTIONS_DEFAULT=	TOPOLOGY
OPTIONS_SUB=	yes

PLIST_SUB+=	PORTVERSION=${PORTVERSION}

LOADERGUI_DESC=	Enable shp2pgsql-gui
RASTER_DESC=	Raster support
TOPOLOGY_DESC=	Topology support
SFCGAL_DESC=	Enable sfcgal
LOADERGUI_CONFIGURE_WITH=	gui
LOADERGUI_USE=	GNOME=gtk20
RASTER_USES=	alias
RASTER_CONFIGURE_WITH=	raster
RASTER_LIB_DEPENDS=		libgdal.so:${PORTSDIR}/graphics/gdal
RASTER_CONFIGURE_ON=	--with-gdalconfig=${LOCALBASE}/bin/gdal-config
TOPOLOGY_CONFIGURE_WITH=	topology
SFCGAL_LIB_DEPENDS=	libSFCGAL.so:${PORTSDIR}/databases/sfcgal
SFCGAL_CONFIGURE_ON=	--with-sfcgal=${LOCALBASE}/bin/sfcgal-config
SFCGAL_CONFIGURE_OFF=	--without-sfcgal

.include <bsd.port.options.mk>

.if ${PORT_OPTIONS:MRASTER}
.if ${OSVERSION} < 900033
BROKEN=			Raster suppport is broken on FreeBSD < 9.x
.endif
.endif

CFLAGS+=	-I${LOCALBASE}/include
LDFLAGS+=	-L${LOCALBASE}/lib

post-patch:
	@${GREP} -lR 'bin/bash' ${WRKSRC} | ${XARGS} ${REINPLACE_CMD} -e "s|/bin/bash|/bin/sh|"

pre-configure:
	@${SH} ${PKGINSTALL} ${PORTNAME} PRE-INSTALL

post-install: .SILENT
	${MKDIR} ${STAGEDIR}/${DATADIR}/tiger_2011
	(cd ${WRKSRC}/extras/tiger_geocoder/tiger_2011/ && ${COPYTREE_SHARE} \* ${STAGEDIR}/${DATADIR}/tiger_2011/ "! ( -name *\.orig -o -name *\.bak )" )
	${MKDIR} ${STAGEDIR}/${DATADIR}/utils
	(cd ${WRKSRC}/utils/ && ${COPYTREE_SHARE} \* ${STAGEDIR}/${DATADIR}/utils/ "! ( -name *\.orig -o -name *\.bak )" )
	${STRIP_CMD} ${STAGEDIR}/${PREFIX}/bin/shp2pgsql
	${STRIP_CMD} ${STAGEDIR}/${PREFIX}/bin/pgsql2shp
	${STRIP_CMD} ${STAGEDIR}/${PREFIX}/lib/liblwgeom-${PORTVERSION}.so
	${STRIP_CMD} ${STAGEDIR}/${PREFIX}/lib/postgresql/postgis-2.1.so

.include <bsd.port.mk>

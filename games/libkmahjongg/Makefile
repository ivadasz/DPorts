# $FreeBSD$

PORTNAME=	libkmahjongg
PORTVERSION=	${KDE4_VERSION}
CATEGORIES=	games kde
MASTER_SITES=	KDE/${KDE4_BRANCH}/${PORTVERSION}/src
DIST_SUBDIR=	KDE/${PORTVERSION}

MAINTAINER=	kde@FreeBSD.org
COMMENT=	Library for the Mahjongg Solitaire for KDE 4

USE_KDE4=	kdelibs kdeprefix libkdegames automoc4
USES=		cmake:outsource tar:xz
USE_QT4=	moc_build qmake_build rcc_build uic_build
USE_LDCONFIG=	yes

.include <bsd.port.mk>

# Created by: Stefan Pauly <stefan@fh-mainz.de>
# $FreeBSD$

PORTNAME=	RRDTool-OO
PORTVERSION=	0.36
CATEGORIES=	devel perl5
MASTER_SITES=	CPAN
PKGNAMEPREFIX=	p5-

MAINTAINER=	bofh@FreeBSD.org
COMMENT=	RRDTool::OO - Object-oriented interface to RRDTool

LICENSE=	ART10 GPLv1
LICENSE_COMB=	dual

BUILD_DEPENDS=	rrdtool>=0:${PORTSDIR}/databases/rrdtool \
		p5-Log-Log4perl>=0.40:${PORTSDIR}/devel/p5-Log-Log4perl
RUN_DEPENDS:=	${BUILD_DEPENDS}

USES=		perl5
USE_PERL5=	configure

.include <bsd.port.mk>

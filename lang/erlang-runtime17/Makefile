# Created by: ruslan@shevchenko.kiev.ua
# $FreeBSD$

PORTNAME=	erlang
PORTVERSION=	17.5.6
PORTREVISION=	1
CATEGORIES=	lang parallel java
MASTER_SITES=	http://www.erlang.org/download/:erlangorg		\
		http://erlang.stacken.kth.se/download/:erlangorg	\
		http://www.csd.uu.se/ftp/mirror/erlang/download/:erlangorg
PKGNAMESUFFIX=	-runtime17
DISTNAME=	otp_src_${ERL_RELEASE}
DISTFILES=	${DISTNAME}${EXTRACT_SUFX}:erlangorg	\
		${ERLANG_MAN}:erlangorg
DIST_SUBDIR=	erlang
EXTRACT_ONLY=	${DISTNAME}${EXTRACT_SUFX}

PATCH_SITES=	http://olgeni.olgeni.com/~olgeni/distfiles/ \
		LOCAL/olgeni
PATCHFILES=	patch-otp-17.5.1 patch-otp-17.5.2 patch-otp-17.5.3 \
		patch-otp-17.5.4 patch-otp-17.5.5 patch-otp-17.5.6

MAINTAINER=	olgeni@FreeBSD.org
COMMENT=	Functional programming language from Ericsson

WRKSRC=		${WRKDIR}/otp_src_${ERL_RELEASE}

ERLANG_LIB=	${PORTNAME}${PORTVERSION:C/\..*//}

SUB_FILES=	pkg-message
SUB_LIST=	ERLANG_LIB=${ERLANG_LIB}

MAKE_JOBS_UNSAFE=yes

OPTIONS_DEFINE=	DOCS DTRACE GCC GS HIPE JAVA KQUEUE ODBC OPENSSL SCTP SMP THREADS WX DIRTY

GCC_DESC=	Use current GCC
HIPE_DESC=	Build native HiPE compiler
KQUEUE_DESC=	Enable Kernel Poll (kqueue) support
SCTP_DESC=	Enable SCTP support
SMP_DESC=	Enable SMP support
WX_DESC=	Enable WX application
GS_DESC=	Enable GS application (deprecated)
DTRACE_DESC=	Enable DTrace support
DIRTY_DESC=	Enable Dirty schedulers (experimental)

OPTIONS_DEFAULT=KQUEUE OPENSSL SCTP SMP THREADS
OPTIONS_EXCLUDE_DragonFly=	SCTP

ERL_RELEASE=	17.5

USES=		autoreconf gmake perl5
USE_PERL5=	build
GNU_CONFIGURE=	yes
LDFLAGS+=	-L${LOCALBASE}/lib

.include <bsd.port.options.mk>

.if ${PORT_OPTIONS:MDOCS}
DISTFILES+=	${ERLANG_DOCS}:erlangorg
.endif

# See http://wiki.freebsd.org/DTrace to learn how to enable DTrace
# on your system. You will need to configure STRIP, WITH_CTF and
# CFLAGS in your make.conf before building.

# If you run Erlang and get a message resembling "WARNING: number of
# probes fixed does not match the number of defined probes (54 != 132,
# respectively)" you probably misconfigured DTrace in some way.

.if ${OSVERSION} < 1000000
.if ${ARCH} == "x86_64" && ${PORT_OPTIONS:MDTRACE} && ! ${PORT_OPTIONS:MGCC}
IGNORE=		DTRACE support on amd64 requires GCC option
.endif
.endif

.if ${PORT_OPTIONS:MGCC}
USE_GCC?=	yes
.endif

.if ${PORT_OPTIONS:MDTRACE}
CONFIGURE_ARGS+=--with-dynamic-trace=dtrace
.else
CONFIGURE_ARGS+=--without-dynamic-trace
.endif

.if ${PORT_OPTIONS:MOPENSSL}
USE_OPENSSL=	yes
CONFIGURE_ARGS+=--with-ssl=${OPENSSLBASE}
.else
CONFIGURE_ARGS+=--without-ssl
.endif

.if ${PORT_OPTIONS:MJAVA}
USE_JAVA=	yes
CONFIGURE_ARGS+=--with-javac
CONFIGURE_ENV+=	ac_cv_prog_JAVAC="${JAVAC}"
.else
CONFIGURE_ARGS+=--without-javac
.endif

.if ${PORT_OPTIONS:MHIPE}
CONFIGURE_ARGS+=--enable-hipe
.else
CONFIGURE_ARGS+=--disable-hipe
.endif

.if ${PORT_OPTIONS:MKQUEUE}
CONFIGURE_ARGS+=--enable-kernel-poll
.else
CONFIGURE_ARGS+=--disable-kernel-poll
.endif

.if ${PORT_OPTIONS:MGS}
USES+=		tk:run
.endif

.if ${PORT_OPTIONS:MWX}
USE_WX=		2.8+
WX_COMPS=	wx contrib
WX_UNICODE=	yes
CONFIGURE_ARGS+=--with-wx-config=${WX_CONFIG}
.endif

.if ${PORT_OPTIONS:MODBC}
CONFIGURE_ARGS+=--with-odbc
LIB_DEPENDS+=	libodbc.so:${PORTSDIR}/databases/unixODBC
.else
CONFIGURE_ARGS+=--without-odbc
.endif

ERLANG_MAN=	otp_doc_man_${ERL_RELEASE}.tar.gz
ERLANG_DOCS=	otp_doc_html_${ERL_RELEASE}.tar.gz

.if ${PORT_OPTIONS:MTHREADS}
CONFIGURE_ARGS+=--enable-threads
.else
CONFIGURE_ARGS+=--disable-threads
.endif

.if ${PORT_OPTIONS:MSCTP}
CONFIGURE_ARGS+=--enable-sctp
.else
CONFIGURE_ARGS+=--disable-sctp
.endif

# enabling --enable-smp-support crashes the OS when
# net_kernel:start([node_name, shortnames]) invoked repeatedly
# TODO verify if it still applies

.if ${PORT_OPTIONS:MSMP}
CONFIGURE_ARGS+=--enable-smp-support
.else
CONFIGURE_ARGS+=--disable-smp-support
.endif

.if ${PORT_OPTIONS:MDIRTY}
CONFIGURE_ARGS+=--enable-dirty-schedulers
.endif

.if ${ARCH} == i386
MAKE_ARGS+=	ARCH=x86
.endif

.if ${ARCH} == armv6
MAKE_ARGS+=	ARCH=arm
.endif

# The man-pages are put (in spite of FreeBSD's port convention) in a private
# subdir. This is to avoid cluttering up the man page name space. Also the
# Erlang man pages are more of internal documentation using the man format than
# actual system man pages. (erl.1 and epmd.1 perhaps being the exception).

# Install documentation. (HTML docs need to be in same dir as the
# rest, not in share/doc/erlang as it should, because of relative
# links in the documentation.

pre-configure:
.if ${PORT_OPTIONS:MDTRACE}
	@if ! /sbin/kldstat | ${GREP} dtraceall >/dev/null 2>&1; then \
		${ECHO_CMD}; \
		${ECHO_CMD} "DTRACE option requires kernel module \"dtraceall\" to be loaded."; \
		${ECHO_CMD}; \
		exit 1; \
	fi
.endif
.if ! ${PORT_OPTIONS:MWX}
	echo "disabled by port options" > ${WRKSRC}/lib/wx/SKIP
.endif

post-install:
	for SECTION in 1 3 4 6 7; do \
		${TAR} --unlink -xzpf ${DISTDIR}/${DIST_SUBDIR}/${ERLANG_MAN} \
			-C ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB} \
			"man/man$${SECTION}/*.$${SECTION}" || ${TRUE}; \
	done
	${RM} -rf ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/man/cat?

.if ${PORT_OPTIONS:MDOCS}
	${TAR} --unlink -xzpf ${DISTDIR}/${DIST_SUBDIR}/${ERLANG_DOCS} \
		-C ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}

	${MV} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/erts-6.4/* \
		${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/erts-6.4.1
	${RMDIR} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/erts-6.4

	${MV} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/inets-5.10.6/* \
		${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/inets-5.10.9
	${RMDIR} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/inets-5.10.6

	${MV} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/ssh-3.2/* \
		${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/ssh-3.2.4
	${RMDIR} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/ssh-3.2

	${MV} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/common_test-1.10/* \
		${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/common_test-1.10.1
	${RMDIR} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/common_test-1.10

	${MV} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/diameter-1.9/* \
		${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/diameter-1.9.2
	${RMDIR} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/diameter-1.9

	${MV} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/snmp-5.1.1/* \
		${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/snmp-5.1.2
	${RMDIR} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/snmp-5.1.1

	${MV} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/test_server-3.8/* \
		${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/test_server-3.8.1
	${RMDIR} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/test_server-3.8

	${MV} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/ssl-6.0/* \
		${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/ssl-6.0.1
	${RMDIR} ${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/ssl-6.0

	${INSTALL_DATA} ${WRKSRC}/lib/dialyzer/doc/*.txt \
		${STAGEDIR}${PREFIX}/lib/${ERLANG_LIB}/lib/dialyzer-*/doc/
.endif
.if ! ${PORT_OPTIONS:MGS}
	${RM} -r ${STAGEDIR}${PREFIX}/lib/erlang/lib/gs-*
.endif
.if ! ${PORT_OPTIONS:MJAVA}
	${RM} -r ${STAGEDIR}${PREFIX}/lib/erlang/lib/jinterface-*
.endif
.if ! ${PORT_OPTIONS:MODBC}
	${RM} -r ${STAGEDIR}${PREFIX}/lib/erlang/lib/odbc-*
.endif
.if ! ${PORT_OPTIONS:MOPENSSL}
	${RM} -r ${STAGEDIR}${PREFIX}/lib/erlang/lib/crypto-*
	${RM} -r ${STAGEDIR}${PREFIX}/lib/erlang/lib/ssh-*
	${RM} -r ${STAGEDIR}${PREFIX}/lib/erlang/lib/ssl-*
.endif
.if ! ${PORT_OPTIONS:MWX}
	${RM} -r ${STAGEDIR}${PREFIX}/lib/erlang/lib/wx-*
.endif

	@cd ${STAGEDIR}${PREFIX}; ${FIND} lib/${ERLANG_LIB}/* -type d -empty \
		| ${SORT} \
		| ${SED} -e 's#^#@dir #g' \
		> ${TMPPLIST}

	@cd ${STAGEDIR}${PREFIX}; ${FIND} lib/${ERLANG_LIB}/* -type f -o -type l \
		| ${SORT} \
		>> ${TMPPLIST}

.include <bsd.port.mk>

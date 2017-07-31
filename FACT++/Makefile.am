include aminclude.am

#-------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I .macro_dir

SVN_REVISION = -DREVISION=\"`svnversion -n .`\"

DIM_FLAGS    = -DMIPSEL -DPROTOCOL=1 -Dunix -Dlinux
BOOST_FLAGS  = -DBOOST_DISABLE_ASSERTS

AM_CFLAGS   = -pedantic
AM_CPPFLAGS = -Idim/dim -Ipal -Ierfa/src \
   $(DIM_FLAGS) $(BOOST_FLAGS) $(ROOTCPPFLAGS) \
   $(SVN_REVISION)

AM_CXXFLAGS = $(ROOTCXXFLAGS) \
   -Wall -Winit-self -Wpointer-arith -Wcast-align -Wextra -Wformat=2 \
   -Woverloaded-virtual -Wnon-virtual-dtor -Wshadow

# -no-undefined
AM_LDFLAGS = -module -avoid-version

#$(man3_MANS): doxygen-doc
#DX_CLEANFILES = everything to clean.
# Add to MOSTLYCLEANFILES.

#-------------------------------------------------------------------------

# Executables which are build by default ('all')
bin_PROGRAMS = \
	dns \
	log logtime time argv \
	ftmctrl \
	fadctrl \
	fscctrl \
	gcn \
	chatclient chatserv \
	biasctrl drivectrl agilentctrl \
	mcp feedback ratescan ratecontrol \
	tngweather lidctrl gpsctrl pfminictrl sqmctrl pwrctrl \
	magicweather magiclidar \
	smartfact evtserver getevent \
	temperature \
	timecheck \
	showlog \
	fitsdump fitscheck fitsselect \
	zfits

if HAS_FITS
bin_PROGRAMS += datalogger
endif

if HAS_V8
bin_PROGRAMS += dimctrl dimserver
endif

if HAS_GUI
bin_PROGRAMS += fact
endif

if HAS_DBUS
bin_PROGRAMS += skypeclient
endif

if HAS_VIEWER
bin_PROGRAMS += viewer
endif

if HAS_NOVA
bin_PROGRAMS += moon
endif

if HAS_SQL
if HAS_NOVA
bin_PROGRAMS += makedata makeschedule
if HAS_ROOT
bin_PROGRAMS += makeplots rootifysql
endif
endif
endif


if HAS_LIBXP
bin_PROGRAMS += did webDid
endif

# This is a trick to be able to build them but not having them in 'all'
if IS_FALSE
bin_PROGRAMS += \
	ftm fsc fad \
	test scheduler \
	triggerschedule \
	dclient5 dserver2 \
	fitsloader fitsgrep \
	sched astro readfits \
	cosyctrl
endif

lib_LTLIBRARIES = \
	libDim++.la libDim.la libDimExtension.la \
	libConfiguration.la libStateMachine.la libTime.la libTools.la \
	libPal.la

if HAS_HELP2MAN
dist_man1_MANS = \
	biasctrl.man \
	datalogger.man \
	dimctrl.man \
	dimserver.man \
	drivectrl.man \
	evtserver.man \
	fadctrl.man \
	feedback.man \
	fitsdump.man \
	fitscheck.man \
	fitsselect.man \
	fscctrl.man \
	ftmctrl.man \
	getevent.man \
	gpsctrl.man \
	lidctrl.man \
	magiclidar.man \
	magicweather.man \
	mcp.man \
	pfminictrl.man \
	pwrctrl.man \
	ratecontrol.man \
	ratescan.man \
	showlog.man \
	smartfact.man \
	sqmctrl.man \
	temperature.man \
	timecheck.man \
	tngweather.man \
	zfits.man
endif

#-------------------------------------------------------------------------

CLEANFILES =

dist_noinst_SCRIPTS = autogen.sh

EXTRA_DIST = \
	Doxyfile \
	dim/DIM_Performance.pdf \
	dim/LICENSE.GPL \
	dim/README.txt \
	dim/README_v9.txt \
	dim/README_v10.txt \
	dim/README_v11.txt \
	dim/README_v12.txt \
	dim/README_v13.txt \
	dim/README_v14.txt \
	dim/README_v17.txt \
	dim/README_v16.txt \
	dim/README_v17.txt \
	dim/README_v18.txt \
	dim/README_v19.txt

dns_LDADD    = libDim.la libDimExtension.la
dns_SOURCES  = src/dns.c
#dns_CPPFLAGS = $(AM_CPPFLAGS) $(DIM_FLAGS)

did_LDADD    = libDim.la 
did_CPPFLAGS = $(AM_CPPFLAGS) -Idim/src/did 
did_SOURCES  = \
	dim/src/did/did.c dim/src/did/did.h \
	dim/src/did/dui_util.c dim/src/did/dui_util.h \
	dim/src/did/dui_colors.h

webDid_LDADD    = libDim.la 
webDid_CPPFLAGS = $(AM_CPPFLAGS) -Idim/src/did 
webDid_SOURCES  = \
	dim/src/webDid/webDid.c \
	src/webServer.c \
	dim/src/webDid/webTcpip.c


#libDim_la_CPPFLAGS = $(AM_CPPFLAGS) $(DIM_FLAGS)
libDim_ladir = 
libDim_la_HEADERS = \
	dim/dim/dic.h \
	dim/dim/dim_common.h \
	dim/dim/dim.h \
	dim/dim/dim_tcpip.h \
	dim/dim/dis.h 

libDim_la_SOURCES = \
	dim/src/dic.c \
	dim/src/dis.c \
	dim/src/dna.c \
	dim/src/sll.c \
	dim/src/dll.c  \
	dim/src/hash.c \
	dim/src/swap.c \
	dim/src/copy_swap.c \
	dim/src/open_dns.c \
	dim/src/conn_handler.c \
	dim/src/tcpip.c \
	dim/src/dtq.c \
	dim/src/dim_thr.c \
	dim/src/utilities.c

libDimExtension_la_SOURCES = src/DimSetup.cc src/DimSetup.h

#libDimCpp_la_CXXFLAGS = $(AM_CXXFLAGS) $(DIM_FLAGS)
libDim___ladir =
libDim___la_HEADERS = \
	dim/dim/dic.hxx \
	dim/dim/dis.hxx \
	dim/dim/dim_core.hxx \
	dim/dim/dim.hxx \
	dim/dim/dim_tcpip.h \
	dim/dim/dllist.hxx \
	dim/dim/sllist.hxx \
	dim/dim/tokenstring.hxx \
	src/Dim.h
libDim___la_SOURCES = \
	dim/src/diccpp.cxx \
	dim/src/dimcpp.cxx \
	dim/src/discpp.cxx \
	dim/src/tokenstring.cxx 

# Divide into Readline / StateMachine / StateMachineDim / Tools?
libStateMachine_la_SOURCES = \
	src/WindowLog.h       src/WindowLog.cc \
	src/Readline.h        src/Readline.cc \
	src/ReadlineColor.h   src/ReadlineColor.cc \
	src/ReadlineWindow.h  src/ReadlineWindow.cc \
	src/Console.h         src/Console.cc \
	src/Shell.h           src/Shell.cc \
	\
	src/EventImp.h        src/EventImp.cc \
	src/Event.h           src/Event.cc \
	src/State.h           src/State.cc \
	src/Description.h     src/Description.cc \
	src/MessageImp.h      src/MessageImp.cc \
	src/Converter.h       src/Converter.cc \
	src/StateMachineImp.h src/StateMachineImp.cc \
	src/StateMachine.h    src/StateMachine.cc \
	\
	src/EventDim.h \
	src/MessageDim.h         src/MessageDim.cc \
	src/StateMachineDim.h    src/StateMachineDim.cc \
	src/DimServerList.h      src/DimServerList.cc \
	src/DimServiceInfoList.h src/DimServiceInfoList.cc \
	src/DimNetworkList.h     src/DimNetwork.cc \
	src/ServiceList.h        src/ServiceList.cc \
	src/DimErrorRedirecter.h  \
	src/DimErrorRedirecter.cc \
	src/DimDescriptionService.h \
	src/DimDescriptionService.cc \
	\
	src/Connection.h      src/Connection.cc \
	src/ConnectionUSB.h   src/ConnectionUSB.cc \
	\
	FACT.h ByteOrder.h \
	\
	src/DimWriteStatistics.h src/DimWriteStatistics.cc

libConfiguration_la_SOURCES = \
	src/Configuration.h src/Configuration.cc \
	src/FACT.h src/FACT.cc

libTime_la_SOURCES  = src/Time.h src/Time.cc
#libAstro_la_SOURCES = src/Astro.h src/Astro.cc
libTools_la_SOURCES = src/tools.h src/tools.cc

libPal_la_SOURCES = pal/pal.h \
	pal/palDtt.c 		pal/palDat.c 		pal/palMappa.c		\
	pal/palPrenut.c		pal/palEvp.c 		pal/palAoppa.c		\
	pal/palAoppat.c		pal/palRefco.c 		pal/palRefro.c		\
	pal/pal1Atmt.c 		pal/palDrange.c		pal/palOne2One.c        \
	pal/pal1Atms.c 		pal/palMapqkz.c 	pal/palAopqk.c		\
	pal/palRefz.c 		pal/palAmpqk.c 		pal/palRdplan.c		\
	pal/palDt.c 		pal/palPvobs.c 		pal/palNut.c		\
	pal/palDmoon.c 		pal/palPlanet.c 	pal/palNutc.c           \
	pal/palDeuler.c \
	\
	erfa/src/gd2gc.c	erfa/src/p06e.c		erfa/src/c2s.c 		\
	erfa/src/eform.c	erfa/src/s2c.c		erfa/src/pas.c		\
	erfa/src/pmat06.c	erfa/src/epv00.c	erfa/src/plan94.c	\
	erfa/src/anpm.c		erfa/src/obl06.c	erfa/src/dat.c		\
        erfa/src/af2a.c		erfa/src/rxr.c		erfa/src/gmst06.c	\
	erfa/src/sepp.c		erfa/src/rz.c		erfa/src/zp.c		\
	erfa/src/rxpv.c		erfa/src/pn.c		erfa/src/cr.c		\
	erfa/src/seps.c		erfa/src/ry.c		erfa/src/pdp.c		\
	erfa/src/pnm06a.c	erfa/src/hfk5z.c	erfa/src/epj2jd.c	\
	erfa/src/pv2s.c		erfa/src/tf2a.c		erfa/src/pm.c		\
	erfa/src/sxp.c		erfa/src/a2af.c		erfa/src/rxp.c		\
	erfa/src/pxp.c		erfa/src/fk5hip.c	erfa/src/fw2m.c		\
	erfa/src/rx.c		erfa/src/tf2d.c		erfa/src/cal2jd.c	\
	erfa/src/cp.c		erfa/src/nut06a.c	erfa/src/rm2v.c		\
	erfa/src/nut00a.c	erfa/src/ee06a.c	erfa/src/fk5hz.c	\
	erfa/src/epb2jd.c	erfa/src/refco.c	erfa/src/a2tf.c		\
	erfa/src/fapa03.c	erfa/src/gst06a.c	erfa/src/faf03.c	\
	erfa/src/faur03.c	erfa/src/faju03.c	erfa/src/fal03.c	\
	erfa/src/fasa03.c	erfa/src/fame03.c 	erfa/src/fave03.c  	\
	erfa/src/fama03.c 	erfa/src/faom03.c 	erfa/src/gst06.c	\
	erfa/src/jd2cal.c	erfa/src/gd2gce.c	erfa/src/anp.c		\
	erfa/src/fae03.c	erfa/src/ir.c		erfa/src/pfw06.c	\
	erfa/src/bpn2xy.c	erfa/src/eors.c		erfa/src/s06.c		\
	erfa/src/trxp.c		erfa/src/era00.c	erfa/src/epj.c		\
	erfa/src/d2tf.c		erfa/src/epb.c		erfa/src/rv2m.c		\
	erfa/src/pap.c		erfa/src/fad03.c	erfa/src/pmp.c		\
	erfa/src/tr.c		erfa/src/falp03.c	

dserver2_SOURCES = src/dserver2.cc 
dserver2_LDADD   = libTime.la libTools.la

ftm_SOURCES = src/ftm.cc src/HeadersFTM.cc
ftm_LDADD   = libTime.la libTools.la libDim++.la libDim.la libConfiguration.la libDimExtension.la

fad_SOURCES = src/fad.cc src/HeadersFAD.cc
fad_LDADD   = libTime.la libTools.la libTools.la libDim++.la libDim.la libConfiguration.la libDimExtension.la

fsc_SOURCES = src/fsc.cc
fsc_LDADD   = libTime.la libTools.la


log_SOURCES = src/log.cc
log_LDADD   = libDim++.la libDim.la libStateMachine.la libTime.la libTools.la


logtime_SOURCES = src/logtime.cc
logtime_LDADD = libStateMachine.la libTools.la libTime.la libDim++.la libDim.la

moon_SOURCES = src/moon.cc
moon_LDADD = libTime.la libConfiguration.la

rootifysql_SOURCES = src/rootifysql.cc
rootifysql_LDADD = $(ROOTLDFLAGS) $(ROOTLIBS) libTime.la libConfiguration.la

makeplots_SOURCES = src/makeplots.cc
makeplots_LDADD = $(ROOTLDFLAGS) $(ROOTLIBS) libTime.la libConfiguration.la

makedata_SOURCES = src/makedata.cc
makedata_LDADD = libTime.la libConfiguration.la

makeschedule_SOURCES = src/makeschedule.cc
makeschedule_LDADD = libTime.la libConfiguration.la libTools.la


chatserv_SOURCES = src/chatserv.cc src/LocalControl.h
chatserv_LDADD = libStateMachine.la libTools.la libConfiguration.la libTime.la \
	libDim++.la libDim.la libDimExtension.la


dclient5_SOURCES = src/dclient5.cc src/LocalControl.h
dclient5_LDADD   = libDim++.la libDim.la libStateMachine.la libTime.la libTools.la \
    libConfiguration.la

ftmctrl_SOURCES = src/ftmctrl.cc src/LocalControl.h src/HeadersFTM.cc
ftmctrl_LDADD   = libDim++.la libDim.la libDimExtension.la \
	libStateMachine.la libTime.la libTools.la \
	libConfiguration.la

cosyctrl_SOURCES = src/cosyctrl.cc src/LocalControl.h src/HeadersFTM.cc
cosyctrl_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

drivectrl_SOURCES = src/drivectrl.cc src/LocalControl.h
drivectrl_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la libPal.la

agilentctrl_SOURCES = src/agilentctrl.cc src/LocalControl.h src/HeadersAgilent.h
agilentctrl_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

magicweather_SOURCES = src/magicweather.cc src/LocalControl.h
magicweather_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

magiclidar_SOURCES = src/magiclidar.cc src/LocalControl.h
magiclidar_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

tngweather_SOURCES = src/tngweather.cc src/LocalControl.h
tngweather_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la -lQtXml

lidctrl_SOURCES = src/lidctrl.cc src/LocalControl.h
lidctrl_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la -lQtXml

gpsctrl_SOURCES = src/gpsctrl.cc src/LocalControl.h
gpsctrl_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

pfminictrl_SOURCES = src/pfminictrl.cc src/LocalControl.h
pfminictrl_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

sqmctrl_SOURCES = src/sqmctrl.cc src/LocalControl.h
sqmctrl_LDADD   = libDim++.la libDim.la libDimExtension.la \
       libStateMachine.la libTools.la libTime.la \
       libConfiguration.la

temperature_SOURCES = src/temperature.cc src/LocalControl.h
temperature_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

pwrctrl_SOURCES = src/pwrctrl.cc src/LocalControl.h \
	src/HeadersPower.h src/HeadersPower.cc
pwrctrl_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la -lQtXml

timecheck_SOURCES = src/timecheck.cc src/LocalControl.h
timecheck_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

smartfact_SOURCES = src/smartfact.cc src/LocalControl.h src/PixelMap.cc
smartfact_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

evtserver_SOURCES = src/evtserver.cc src/LocalControl.h
evtserver_LDADD   = libDim++.la libDim.la libDimExtension.la \
 	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

getevent_SOURCES = src/getevent.cc
getevent_LDADD   = libTools.la libTime.la libConfiguration.la

fadctrl_SOURCES = src/fadctrl.cc src/LocalControl.h src/HeadersFAD.cc \
	src/EventBuilder.cc     src/EventBuilder.h \
	src/DataProcessorImp.cc src/DataProcessorImp.h \
	src/DataCalib.cc        src/DataCalib.h \
	src/DataWriteRaw.cc     src/DataWriteRaw.h \
	src/DrsCalib.h
if HAS_FITS
fadctrl_SOURCES += src/FitsFile.h src/FitsFile.cc \
	src/DataWriteFits.cc    src/DataWriteFits.h \
	src/DataWriteFits2.cc   src/DataWriteFits2.h
endif
fadctrl_LDADD   = libDim++.la libDim.la  libDimExtension.la \
	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

fscctrl_SOURCES = src/fscctrl.cc src/LocalControl.h
fscctrl_LDADD   = libDim++.la libDim.la  libDimExtension.la \
       libStateMachine.la libTools.la libTime.la \
       libConfiguration.la

gcn_SOURCES = src/gcn.cc src/LocalControl.h
gcn_LDADD   = libDim++.la libDim.la  libDimExtension.la \
       libStateMachine.la libTools.la libTime.la \
       libConfiguration.la -lQtXml

biasctrl_SOURCES = src/biasctrl.cc src/LocalControl.h src/PixelMap.cc
biasctrl_LDADD   = libDim++.la libDim.la  libDimExtension.la \
	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

mcp_SOURCES = src/mcp.cc src/LocalControl.h
mcp_LDADD   = libDim++.la libDim.la  libDimExtension.la \
	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

feedback_SOURCES = src/feedback.cc src/LocalControl.h src/PixelMap.cc
feedback_LDADD   = libDim++.la libDim.la  libDimExtension.la \
	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

ratescan_SOURCES = src/ratescan.cc src/LocalControl.h src/PixelMap.cc
ratescan_LDADD   = libDim++.la libDim.la  libDimExtension.la \
	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

ratecontrol_SOURCES = src/ratecontrol.cc src/LocalControl.h src/PixelMap.cc
ratecontrol_LDADD   = libDim++.la libDim.la  libDimExtension.la \
	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la


argv_SOURCES = src/argv.cc
argv_LDADD = libConfiguration.la


dimctrl_SOURCES = src/dimctrl.cc \
	src/StateMachineDimControl.cc src/StateMachineDimControl.h \
	src/RemoteControl.cc src/RemoteControl.h \
	src/InterpreterV8.cc src/InterpreterV8.h \
	src/DimState.cc src/DimState.h
dimctrl_LDADD = libStateMachine.la libTools.la libTime.la \
	libDim++.la libDim.la libDimExtension.la libConfiguration.la

dimserver_SOURCES = $(dimctrl_SOURCES)
dimserver_LDADD = $(dimctrl_LDADD)


chatclient_SOURCES = src/chatclient.cc src/ChatClient.h
chatclient_LDADD = libStateMachine.la libTools.la libTime.la \
	libDim++.la libDim.la libDimExtension.la libConfiguration.la

skypeclient_SOURCES = src/skypeclient.cc src/ChatClient.h
skypeclient_LDADD = libStateMachine.la libTools.la libTime.la \
	libDim++.la libDim.la libDimExtension.la libConfiguration.la


time_SOURCES = src/time.cc
time_LDADD = libTime.la 

#astro_SOURCES = src/astro.cc
#astro_LDADD = libAstro.la libTime.la 

test_SOURCES = src/test.cc
test_LDADD = libStateMachine.la libTools.la libTime.la libDim++.la libDim.la libConfiguration.la

datalogger_SOURCES = src/datalogger.cc src/LocalControl.h src/DimState.cc src/DimState.h
if HAS_FITS
datalogger_SOURCES += src/FitsFile.h src/FitsFile.cc src/Fits.h src/Fits.cc
endif
datalogger_LDADD  = libDim++.la libDim.la libDimExtension.la \
	libStateMachine.la libTools.la libTime.la \
    	libConfiguration.la

scheduler_SOURCES = src/scheduler.cc src/LocalControl.h 
scheduler_LDADD   = libStateMachine.la libTools.la libTime.la \
	libDim++.la libDim.la libDimExtension.la libConfiguration.la

fitsloader_SOURCES = src/fitsloader.cc src/LocalControl.h 
fitsloader_LDADD   = libStateMachine.la libTools.la libTime.la libDim++.la libDim.la libConfiguration.la

fitsdump_SOURCES = src/fitsdump.cc 
fitsdump_LDADD   = $(ROOTLDFLAGS) $(ROOTLIBS) libTools.la libConfiguration.la libTime.la

fitscheck_SOURCES = src/fitscheck.cc 
fitscheck_LDADD   = libConfiguration.la

fitsselect_SOURCES = src/fitsselect.cc 
fitsselect_LDADD   = libConfiguration.la


readfits_SOURCES = src/readfits.cc src/ReadFits.h
readfits_LDADD   = libStateMachine.la libTools.la libTime.la libDim++.la libDim.la libConfiguration.la

zfits_SOURCES = src/zfits.cc externals/huffmans.h
zfits_LDADD   = libTime.la libConfiguration.la

showlog_SOURCES = src/showlog.cc
showlog_LDADD   = libTime.la libTools.la libConfiguration.la -lncurses src/WindowLog.lo 

triggerschedule_SOURCES = src/triggerschedule.cc 
triggerschedule_LDADD   = libDim++.la libDim.la libDimExtension.la libConfiguration.la

#fitsCompressor_SOURCES = src/fitsCompressor.cc 
#fitsCompressor_LDADD   = libConfiguration.la

#fitsgrep_SOURCES = src/fitsgrep.cc
#fitsgrep_LDADD   = libConfiguration.la

# -----

if HAS_GUI

fact_DIALOGS = \
	gui/design.ui

fact_RESOURCES = \
	gui/design.qrc

fact_QT_SOURCES = \
	gui/DockWindow.cc \
	gui/MainWindow.cc \
	gui/QCameraWidget.cc \
	gui/BasicGlCamera.cc

fact_SRCADD = $(fact_DIALOGS:.ui=.h) $(fact_RESOURCES:.qrc=.cc) $(fact_QT_SOURCES:.cc=.moc.cc)

endif

# -----

if HAS_VIEWER

viewer_DIALOGS = \
	gui/RawEventsViewer/viewer.ui

viewer_QT_SOURCES = \
	gui/RawEventsViewer/RawEventsViewer.cc \
	gui/BasicGlCamera.cc \
	gui/QCameraWidget.cc \
	gui/Q3DCameraWidget.cc

viewer_SRCADD = $(viewer_DIALOGS:.ui=.h) $(viewer_RESOURCES:.qrc=.cc) $(viewer_QT_SOURCES:.cc=.moc.cc)

endif



BUILT_SOURCES = $(fact_SRCADD) $(viewer_SRCADD)
	
CLEANFILES += $(BUILT_SOURCES)

EXTRA_DIST += \
	$(fact_DIALOGS) $(fact_RESOURCES) \
	$(viewer_DIALOGS) $(viewer_RESOURCES) 

fact_LDADD = $(ROOTLDFLAGS) $(ROOTGLIBS) -lGQt $(QT4_LIB) $(QT4_LDFLAGS) -L. \
	libDim++.la libDim.la libDimExtension.la \
	libTools.la libStateMachine.la libTime.la libTools.la \
	libConfiguration.la

fact_SOURCES = $(fact_SRCADD) $(fact_QT_SOURCES) \
	gui/CheckBoxDelegate.cc gui/HtmlDelegate.cc \
	gui/fact.cc gui/FactGui.cc src/HeadersFTM.cc \
	src/PixelMap.cc

# Switch off most qwt warnings
viewer_CXXFLAGS = $(AM_CXXFLAGS) -Wno-shadow

viewer_LDADD = $(QT4_LIB) $(QT4_LDFLAGS) -L. -lQtOpenGL -lGLU \
	libDimExtension.la \
	libConfiguration.la libStateMachine.la libTools.la \
	libTime.la libDim++.la libDim.la 

viewer_SOURCES = $(viewer_SRCADD) $(viewer_QT_SOURCES) \
	src/DataProcessorImp.cc src/DataProcessorImp.h \
	src/FitsFile.cc  src/FitsFile.h \
	src/Fits.cc      src/Fits.h \
	src/PixelMap.cc



#-------------------------------------------------------------------------

SUFFIXES = .moc.cc

.ui.h: $<
	$(AM_V_GEN)$(UIC4) $< -o $@

.h.moc.cc: $<
	$(AM_V_GEN)$(MOC4) $(EXTRA_CPPFLAGS) $< -o $@

.qrc.cc: $<
	$(AM_V_GEN)$(RCC4) -name `echo "$<" | sed 's|^.*/\(.*\)\.qrc$$|\1|'` $< -o $@


#-------------------------------------------------------------------------

MAN_TARGETS = $(dist_man1_MANS)

if HAS_GROFF

MAN_TARGETS += $(dist_man1_MANS:.man=.html)
EXTRA_DIST  += $(dist_man1_MANS:.man=.html) 
CLEANFILES  += $(dist_man1_MANS:.man=.html) 

if HAS_PS2PDF

MAN_TARGETS += $(dist_man1_MANS:.man=.pdf)
EXTRA_DIST  += $(dist_man1_MANS:.man=.pdf)
CLEANFILES  += $(dist_man1_MANS:.man=.pdf)
endif

endif

if HAS_JSDOC
JAVA_SCRIPT_DOC=jsdoc
endif

$(dist_man1_MANS): $(dist_man1_MANS:.man=)
	@mkdir -p man
	$(AM_V_GEN)help2man -N -o $@ -m $(@:.man=) ./$(@:.man=)

.man.html: $<
	$(AM_V_GEN)groff -mandoc `man -w -l $<` -T html > $@

.man.pdf: $<
	$(AM_V_GEN)groff -mandoc `man -w -l $<` | ps2pdf - $@

jsdoc:
	@rm -rf www/dimctrl
	$(AM_V_GEN)jsdoc -r=2 -d=www/dimctrl scripts | grep -v ^java


doc: $(MAN_TARGETS) $(JAVA_SCRIPT_DOC) doxygen-run
	@ln -sfv doxygen-doc/html/index.html doxygen-doc/html/main.html
	@mkdir -vp doxygen-doc/html/pdf
	@mkdir -vp doxygen-doc/html/man
	@ln -sfv `pwd`/*.pdf doxygen-doc/html/pdf/
	@ln -sfv `pwd`/*.html doxygen-doc/html/man/

diff:
	@svn diff | $(COLORDIFF)

rdiff:
	@svn diff -r BASE:HEAD . externals | $(COLORDIFF)

status:
	@svn status -u | grep -v ^\?

#-------------------------------------------------------------------------

# Overwrite rules for silent or other verbosity levels
#AM_V_MAN = $(AM_MAN_$(V))
#AM_MAN_ = $(AM_V_GEN)
#AM_MAN_0 = @echo  "  MAN    "$@;

#$(MyAnalysisDS): $(MyAnalysisH) $(MyAnalysisL)
#	$(ROOTCINT) -f $@ -c -I$(top_builddir)/config $(INCLUDES) $^
#	rootcint_files=`echo $@ | sed -ne 's/\(.*\)\..*/\1.cxx \1.h/p'` && \
#        $(top_srcdir)/config/runsed $(top_srcdir)/config/rootcint.sed $$rootcint_files && \
#        for i in $$rootcint_files; do \
#          if test ! `diff $$i $(srcdir)/$$i >/dev/null 2>&1`; then \
#            cp $$i $(srcdir)/; \
#          fi; \
#        done

#CLEANFILES = *~ *.rej *.orig
#MAINTAINERCLEANFILES = aclocal.m4 config.h.in configure Makefile.in \
#        stamp-h.in stamp-h[0-9].in
#DISTCLEANFILES = config.cache config.log

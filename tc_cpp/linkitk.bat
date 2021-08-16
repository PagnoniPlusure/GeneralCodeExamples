@rem = '
@echo off
setlocal
if "%MSDEV_HOME%"=="" goto nomsdev
set VCVARS_FILE="%MSDEV_HOME%\Auxiliary\Build\vcvarsall.bat"
if not exist %VCVARS_FILE% goto nomsdev

REM Set compiler bit
set COMPILER=x64

REM call %VCVARS_FILE% %COMPILER%
%TC_ROOT%\perl\bin\perl %0.bat %*
endlocal
goto :EOF
:nomsdev
echo MSDEV_HOME is either not defined or not defined to the correct location.
echo It must be defined to point to the root of the Visual Studio compiler
echo installation.  For example:
echo set MSDEV_HOME=c:\Program Files\Microsoft Visual Studio 15.0\VC
goto :EOF
rem ';

################################################################################
# This is a sample link script for linking ITK programs for Windows
# The syntax is:  
#   linkitk -o <executable> <object1> [object2] ...
################################################################################
# REQUIRES the following be set:
#
# The root of the Microsoft Visual C++ directory must be set in the MSDEV_HOME
# environment variable.
#
# Example : set MSDEV_HOME=c:\Program Files\Microsoft Visual Studio 15.0\VC
#
################################################################################

sub Tell_Help
{
  print <<EOF

  Usage : linkitk -o executable object-1 [object-2 ...]

  Please notice:
  * The executable file name should not carry the 'exe' extension.
  * The following environment variables should be set properly :
     TC_LIBRARY   library dir of TC.
EOF
} 

$errEnvVar = 0;

if (! $ENV{TC_LIBRARY}) {
    print "Error: TC_LIBRARY variable must be set to run linkitk.\n";
    $errEnvVar = 1;
}

$ARGC=@ARGV;

################################
# Must have -o followed by at least two arguments.
################################
if ( $ARGC < 3 || $ARGV[0] ne "-o" || $errEnvVar == 1 ) {
  &Tell_Help;
  exit 1;
}

shift @ARGV;
$EXECUTABLE  = shift @ARGV;
@OBJECT_FILES = @ARGV;

print "\n\t      Executable file will be : $EXECUTABLE\n";
print "\t      Linking against files   : @OBJECT_FILES\n";

$DLLFLAGS = " -debug" .
            " -nologo" .
            " -subsystem:console" .
            " -MACHINE:X64";

$LIBPATH = "$ENV{LIB_PATH}";

###############################################################################
# Platform Independent
###############################################################################
$LINK = "link";

$SYSLIBS = "wsock32.lib " .
           "advapi32.lib " .
           "msvcrt.lib " .
           "msvcprt.lib ".
           "oldnames.lib " .
           "kernel32.lib " .
           "winmm.lib ";

$XMLLIBS = "$ENV{TC_LIBRARY}\\xerces321.lib";
###############################################################################
# link up
###############################################################################

$linkcmd =  "$LINK $DLLFLAGS $LIBPATH @OBJECT_FILES ".
            "$SYSLIBS /ignore:4199 delayimp.lib ".
            "$ENV{USER_LIBS} ".
            "$ENV{TC_LIBRARY}\\itk_main.obj ".
            "$ENV{TC_LIBRARY}\\libsyss.lib ".
            "$ENV{TC_LIBRARY}\\libpom.lib ".
            "$ENV{TC_LIBRARY}\\libtcsso.lib /delayload:libtcsso.dll ".
            "$ENV{TC_LIBRARY}\\libae.lib /delayload:libae.dll".
            "$ENV{TC_LIBRARY}\\libappr.lib /delayload:libappr.dll ".
            "$ENV{TC_LIBRARY}\\libarchive.lib /delayload:libarchive.dll ".
            "$ENV{TC_LIBRARY}\\libbackup.lib /delayload:libbackup.dll ".
            "$ENV{TC_LIBRARY}\\libbom.lib /delayload:libbom.dll ".
            "$ENV{TC_LIBRARY}\\libcfm.lib /delayload:libcfm.dll ".
            "$ENV{TC_LIBRARY}\\libconstants.lib /delayload:libconstants.dll ".
            "$ENV{TC_LIBRARY}\\libcxpom.lib ".
            "$ENV{TC_LIBRARY}\\libdmi.lib /delayload:libdmi.dll ".
            "$ENV{TC_LIBRARY}\\libecm.lib /delayload:libecm.dll ".
            "$ENV{TC_LIBRARY}\\libeint.lib /delayload:libeint.dll ".
            "$ENV{TC_LIBRARY}\\libepm.lib /delayload:libepm.dll ".
            "$ENV{TC_LIBRARY}\\libfclasses.lib ".
            "$ENV{TC_LIBRARY}\\libform.lib /delayload:libform.dll ".
            "$ENV{TC_LIBRARY}\\libgms.lib /delayload:libgms.dll ".
            "$ENV{TC_LIBRARY}\\libgrm.lib /delayload:libgrm.dll ".
            "$ENV{TC_LIBRARY}\\libtcinit.lib ".
            "$ENV{TC_LIBRARY}\\libtc.lib ".
            "$ENV{TC_LIBRARY}\\libtccore.lib /delayload:libtccore.dll ".
            "$ENV{TC_LIBRARY}\\libFnd0FastenerMgmt.lib /delayload:libFnd0FastenerMgmt.dll ".
            "$ENV{TC_LIBRARY}\\libFnd0FastenerMgmtBvr.lib /delayload:libFnd0FastenerMgmtBvr.dll ".
            "$ENV{TC_LIBRARY}\\libtctrushape.lib /delayload:libtctrushape.dll ".
            "$ENV{TC_LIBRARY}\\libinternal_exits.lib /delayload:libinternal_exits.dll ".
            "$ENV{TC_LIBRARY}\\libvalidation.lib /delayload:libvalidation.dll ".
            "$ENV{TC_LIBRARY}\\libitk.lib /delayload:libitk.dll ".
            "$ENV{TC_LIBRARY}\\liblov.lib /delayload:liblov.dll ".
            "$ENV{TC_LIBRARY}\\libme.lib /delayload:libme.dll ".
            "$ENV{TC_LIBRARY}\\libmgc.lib /delayload:libmgc.dll ".
            "$ENV{TC_LIBRARY}\\libobjio.lib /delayload:libobjio.dll ".
            "$ENV{TC_LIBRARY}\\libpie.lib /delayload:libpie.dll ".
            "$ENV{TC_LIBRARY}\\libai.lib /delayload:libai.dll ".
            "$ENV{TC_LIBRARY}\\libproperty.lib ".
            "$ENV{TC_LIBRARY}\\libwproxy.lib /delayload:libwproxy.dll ".
            "$ENV{TC_LIBRARY}\\libps.lib /delayload:libps.dll ".
            "$ENV{TC_LIBRARY}\\libpublication.lib /delayload:libpublication.dll ".
            "$ENV{TC_LIBRARY}\\libqry.lib /delayload:libqry.dll ".
            "$ENV{TC_LIBRARY}\\libclips.lib /delayload:libclips.dll ".
            "$ENV{TC_LIBRARY}\\libclipsutil.lib /delayload:libclipsutil.dll ".
            "$ENV{TC_LIBRARY}\\librbf.lib /delayload:librbf.dll ".
            "$ENV{TC_LIBRARY}\\libce.lib /delayload:libce.dll ".
            "$ENV{TC_LIBRARY}\\libres.lib /delayload:libres.dll ".
            "$ENV{TC_LIBRARY}\\libsa.lib ".
            "$ENV{TC_LIBRARY}\\libss.lib ".
            "$ENV{TC_LIBRARY}\\libstep.lib /delayload:libstep.dll ".
            "$ENV{TC_LIBRARY}\\libsub_mgr.lib /delayload:libsub_mgr.dll ".
            "$ENV{TC_LIBRARY}\\libtextsrv.lib ".
			      "$ENV{TC_LIBRARY}\\libibom.lib /delayload:libibom.dll ".
            "$ENV{TC_LIBRARY}\\libtrv.lib /delayload:libtrv.dll ".
            "$ENV{TC_LIBRARY}\\libtie.lib /delayload:libtie.dll ".
            "$ENV{TC_LIBRARY}\\libnxmgr_im.lib /delayload:libnxmgr_im.dll ".
            "$ENV{TC_LIBRARY}\\libaie.lib /delayload:libaie.dll ".
            "$ENV{TC_LIBRARY}\\libics.lib /delayload:libics.dll ".
            "$ENV{TC_LIBRARY}\\libeintstub.lib /delayload:libeintstub.dll ".
            "$ENV{TC_LIBRARY}\\libsml.lib /delayload:libsml.dll ".
            "$ENV{TC_LIBRARY}\\libuser_exits.lib /delayload:libuser_exits.dll ".
            "$ENV{TC_LIBRARY}\\librdv.lib /delayload:librdv.dll ".
            "$ENV{TC_LIBRARY}\\libcollabctx.lib /delayload:libcollabctx.dll ".
            "$ENV{TC_LIBRARY}\\libmechatronics.lib /delayload:libmechatronics.dll ".
            "$ENV{TC_LIBRARY}\\libtccoreext.lib /delayload:libtccoreext.dll ".
            "$ENV{TC_LIBRARY}\\libusagewc.lib /delayload:libusagewc.dll ".
            "$ENV{TC_LIBRARY}\\libusage.lib /delayload:libusage.dll ".
            "$ENV{TC_LIBRARY}\\libcontmgmt.lib /delayload:libcontmgmt.dll ".
            "$ENV{TC_LIBRARY}\\libtc_utils.lib /delayload:libtc_utils.dll ".
            "$ENV{TC_LIBRARY}\\libschmgt.lib /delayload:libschmgt.dll ".
            "$ENV{TC_LIBRARY}\\libschmgt_bridge.lib /delayload:libschmgt_bridge.dll ".
            "$ENV{TC_LIBRARY}\\libsmengine.lib /delayload:libsmengine.dll ".
            "$ENV{TC_LIBRARY}\\libsmanalytics.lib /delayload:libsmanalytics.dll ".
            "$ENV{TC_LIBRARY}\\libdocmgt.lib /delayload:libdocmgt.dll ".
            "$ENV{TC_LIBRARY}\\libdocmgtapp.lib /delayload:libdocmgtapp.dll ".
            "$ENV{TC_LIBRARY}\\libvm.lib /delayload:libvm.dll ".
            "$ENV{TC_LIBRARY}\\libadsfoundationdispatch.lib /delayload:libadsfoundationdispatch.dll ".
            "$ENV{TC_LIBRARY}\\libcm.lib /delayload:libcm.dll ".
            "$ENV{TC_LIBRARY}\\libcmdispatch.lib /delayload:libcmdispatch.dll ".
            "$ENV{TC_LIBRARY}\\libconstants.lib /delayload:libconstants.dll ".
            "$ENV{TC_LIBRARY}\\libbase_utils.lib /delayload:libbase_utils.dll ".
            "$ENV{TC_LIBRARY}\\libfoundationbase.lib /delayload:libfoundationbase.dll ".
            "$ENV{TC_LIBRARY}\\libmld.lib /delayload:libmld.dll ".
            "$ENV{TC_LIBRARY}\\libfunctionality.lib /delayload:libfunctionality.dll ".
            "$ENV{TC_LIBRARY}\\libmfg.lib ".
            "$ENV{TC_LIBRARY}\\libmfgbvr.lib ".
            "$ENV{TC_LIBRARY}\\libnls.lib ".
            "$ENV{TC_LIBRARY}\\libcondvalidation.lib ".
            "$ENV{TC_LIBRARY}\\libbooleanmath.lib ".
            "$ENV{TC_LIBRARY}\\libconfigurator.lib ".
            "$ENV{TC_LIBRARY}\\libfnd0booleansolve.lib ".
            "$ENV{TC_LIBRARY}\\libsecore.lib ".
            "$ENV{TC_LIBRARY}\\libdispatcher.lib ".
            "$ENV{TC_LIBRARY}\\libFnd0formulamgmt.lib /delayload:libFnd0formulamgmt.dll ".
            "$ENV{TC_LIBRARY}\\libFnd0audit_mgr.lib /delayload:libFnd0audit_mgr.dll ".
			      "$ENV{TC_LIBRARY}\\libxformer.lib /delayload:libxformer.dll ".
            "-out:$EXECUTABLE.exe";

print "\n$linkcmd\n";
$link_status=system($linkcmd);
exit $link_status;

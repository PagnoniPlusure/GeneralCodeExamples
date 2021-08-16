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

  Usage: compile [-h] [-debug] -DIPLIB=(${IPLIB_KEYS}) filenames

     -h     : This usage.
     -debug : To generate debugging information into the object file. 
     -DIPLIB=xxx : to specify the target library, or 'none' for ITK programs.

  Please notice:
  * Set USER_INCLUDE to augment the path for additional include files.
  * The following environment variables should be set properly:
     TC_INCLUDE   IMAN's include files.
  * Setup environment as follows:
    set TC_ROOT, TC_DATA evnvironment variables and run tc_profilevars from TC_DATA.
EOF
}

# Legal values for IPLIB, with comments

%IPLIB=(
    none            => "ITK program object file",
    libuser_exits   => "user_exits library object file",
    libserver_exits => "server_exits library object file",
);

$IPLIB_KEYS=join("|",keys %IPLIB);
$IPLIB_SUMMARY=
    join("\n",
        map {
                sprintf("%35s : %s", $IPLIB{$_}, "-DIPLIB=$_")
            } 
            keys %IPLIB
        );

################################
# Checking at least two argument (-DIPLIB and source file)
################################

if ( @ARGV < 2 || $ARGV[0] =~ m~^[-/]h~ ) {
  &Tell_Help;
  exit 1;
}

$debug_mode=0;
$bit64_mode=0;
$libtarget='';
foreach( @ARGV ) {
  if ( /^-debug/) {
    die "-debug already specified\n" if $debug_mode;
    $debug_mode=1;
  }
  elsif ( /^-DIPLIB=(${IPLIB_KEYS})$/ ) {
    die "-DIPLIB=$libtarget already specified\n" if $libtarget;
    $libtarget=$1;
  }
  elsif ( ! -f $_ ) {
      warn "\tError: $_ does not exist\n";
      $COMPILED_FAILED=1;
  }
  else {
      push @FILES, $_;
  }
}

unless ($libtarget) {
    die <<EOF
$0: No legal -DIPLIB= value. Must use one of
$IPLIB_SUMMARY
EOF
}

###############################################
# Check stated requirements, set basic includes
###############################################

if (! -d $ENV{TC_INCLUDE}) {
  print "\nThe TC_INCLUDE environment variable is either\n";
  print "not set or does not specify a valid NT directory.\n";
  exit 1;
}

# If USER_INCLUDE is undefined, default to the current directory.
if (! $ENV{USER_INCLUDE}) {
  $ENV{USER_INCLUDE} = ".";
}

# Set all include directories.
$INCLUDES = "$ENV{ALL_INCLUDES}";

$DEFINES = join(" ",qw{      
       -DSTEP
       -DAP203
       -DAP214
       -DWIN32_LEAN_AND_MEAN
       -DWNT
       -D_CRT_SECURE_NO_DEPRECATE
       -D_CRT_NONSTDC_NO_DEPRECATE
       -D_SECURE_SCL=0
       -D_HAS_TR1=0
       -DWNT
       -D_WIN32
       -DWIN32
       -D_WIN64
       -D_INTEL=1
       -DCRTAPI1=_cdecl
       -DCRTAPI2=cdecl
       -DIMAN_TAO
       -DBYPASS_FLEX
       -DPOMDLL
       -DUNICODE
       -D_UNICODE
  }, "-DIPLIB=${libtarget}");

if ($debug_mode == 1) {
  $MISCFLAGS="-c -nologo -EHsc -W1 -Zi -Od -MD";
}
else
{
  $MISCFLAGS="-c -nologo -EHsc -W1 -Zi -Ox -Oy- -MD";
}

$CFLAGS="$DEFINES $INCLUDES $MISCFLAGS";

$COMPILER="cl";

foreach $file (@FILES) {
  $compilecmd="$COMPILER $CFLAGS $file";
  print "$compilecmd\n";
  $compiler_status=system($compilecmd);
  print "Compiler Status: $compiler_status\n";
  $COMPILED_FAILED||=$compiler_status;
} 

exit $COMPILED_FAILED;

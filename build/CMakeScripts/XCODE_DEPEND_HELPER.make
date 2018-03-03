# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.common_lib.Debug:
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Debug/libcommon_lib.a:
	/bin/rm -f /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Debug/libcommon_lib.a


PostBuild.final.Debug:
PostBuild.common_lib.Debug: /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Debug/final
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Debug/final:\
	/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Debug/libcommon_lib.a
	/bin/rm -f /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Debug/final


PostBuild.common_lib.Release:
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Release/libcommon_lib.a:
	/bin/rm -f /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Release/libcommon_lib.a


PostBuild.final.Release:
PostBuild.common_lib.Release: /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Release/final
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Release/final:\
	/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Release/libcommon_lib.a
	/bin/rm -f /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Release/final


PostBuild.common_lib.MinSizeRel:
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/MinSizeRel/libcommon_lib.a:
	/bin/rm -f /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/MinSizeRel/libcommon_lib.a


PostBuild.final.MinSizeRel:
PostBuild.common_lib.MinSizeRel: /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/MinSizeRel/final
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/MinSizeRel/final:\
	/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/MinSizeRel/libcommon_lib.a
	/bin/rm -f /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/MinSizeRel/final


PostBuild.common_lib.RelWithDebInfo:
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/RelWithDebInfo/libcommon_lib.a:
	/bin/rm -f /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/RelWithDebInfo/libcommon_lib.a


PostBuild.final.RelWithDebInfo:
PostBuild.common_lib.RelWithDebInfo: /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/RelWithDebInfo/final
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/RelWithDebInfo/final:\
	/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/RelWithDebInfo/libcommon_lib.a
	/bin/rm -f /Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/RelWithDebInfo/final




# For each target create a dummy ruleso the target does not have to exist
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Debug/libcommon_lib.a:
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/MinSizeRel/libcommon_lib.a:
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/RelWithDebInfo/libcommon_lib.a:
/Users/xinbeifu/Documents/grad\ courses/computational\ photography/gradient\ editing/build/Release/libcommon_lib.a:

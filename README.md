Tomahawk
========

Compiling and running Tomahawk
------------------------------

Compile:

    $ mkdir build && cd build
    $ cmake ..
    $ make

Start the application on Linux:

    $ ./tomahawk

Start the application on OS X:

    $ open tomahawk.app
    
    
To compile for debugging:

	$ cmake -DCMAKE_BUILD_TYPE=Debug ..
	
	
To compile without GUI:

	$ cmake -DBUILD_GUI=OFF ..

    
Cross-Compile:

	$ mkdir build-armhf && cd build-armhf
	$ cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-armhf.cmake ..
    $ make



Install dependencies for running on Debian 8.0
-----------------------------------------

	$ sudo apt-get install phonon phonon-backend-gstreamer gstreamer1.0-fluendo-mp3 libqjson0 libtag1c2a libqca2 libquazip1 libqt4-dbus libqt4-network libqt4-script libqt4-sql libqt4-sql-sqlite libqt4-svg libqt4-test libqt4-xml libqt4-xmlpatterns libqtcore4 libqtgui4 libqtwebkit4 libechonest2.1 libattica0.4 liblastfm1 libclucene-core1


Install dependencies for building & running on Debian 8.0
-----------------------------------------

	$ sudo apt-get install build-essential cmake pkg-config libphonon-dev phonon-backend-gstreamer gstreamer1.0-fluendo-mp3 libqjson-dev libtag1-dev libboost-dev libqca2-dev libechonest-dev libattica-dev liblastfm-dev libquazip-dev libclucene-dev


Install dependencies for cross compiling on Debian 8.0
-----------------------------------------

	The package g++-4.7-arm-linux-gnueabihf and all needed dependencies could be found on http://www.emdebian.org/debian.
	
	$ sudo apt-get install xapt g++-4.7-arm-linux-gnueabihf
	$ sudo xapt -a armhf -m libxext-dev libqt4-dev libqtwebkit-dev libphonon-dev libqjson-dev libtag1-dev libboost-dev libqca2-dev libechonest-dev libattica-dev liblastfm-dev libquazip-dev libclucene-dev
	
	$ sudo cp /usr/lib/x86_64-linux-gnu/CLucene/CLuceneConfig.cmake /usr/arm-linux-gnueabihf/lib/CLucene/
	$ sudo cp libpulse0_4.0-6+b1_armhf.deb:/usr/lib/arm-linux-gnueabihf/pulseaudio/libpulsecommon-4.0.so /usr/arm-linux-gnueabihf/lib


Install dependencies for running on Raspbian 7.0
-----------------------------------------

	$ sudo apt-get install phonon phonon-backend-gstreamer gstreamer0.10-fluendo-mp3 libqjson0 libtag1c2a libqca2
	$ sudo apt-get install libqt4-dbus libqt4-network libqt4-script libqt4-sql libqt4-sql-sqlite libqt4-svg libqt4-test libqt4-xml libqt4-xmlpatterns libqtcore4 libqtgui4 libqtwebkit4


The following packages have to be installed manually, because the versions of the package managment are too old:

* libattica0.4
* liblastfm1
* libquazip0
* libclucene-contribs1
* libclucene-core1
* libechonest2.1



Install dependencies for building & running on Raspbian 7.0
-----------------------------------------

	$ sudo apt-get install build-essential cmake libphonon-dev phonon-backend-gstreamer gstreamer0.10-fluendo-mp3
	$ sudo apt-get install libqjson-dev libtag1-dev libboost-dev libqca2-dev zlib1g-dev
	$ libqca2-plugin-ossl ???

The following packages have to be installed manually, because the versions of the package managment are too old:

* libattica0.4
* liblastfm1
* libquazip0
* libclucene-contribs1
* libclucene-core1
* libechonest2.1

* libattica-dev
* libfftw3-single3
* liblastfm-dev
* liblastfm-fingerprint1
* libquazip0-dev
* libclucene-dev
* libechonest-dev



Detailed building instructions for Ubuntu
-----------------------------------------
See: http://wiki.tomahawk-player.org/index.php/Building_Binary_on_Ubuntu_Precise_(12.04)

Detailed building instructions for OS X
---------------------------------------
See: http://wiki.tomahawk-player.org/index.php/Building_OS_X_Application_Bundle

Doxygen Documentation
---------------------
See: http://dev.tomahawk-player.org/api/classes.html

Dependencies
------------

Required dependencies:

* CMake 2.8.6 - http://www.cmake.org/
* Qt 4.7.0 - http://qt-project.org/
* Phonon 4.6.0 - http://phonon.kde.org/
* QJson 0.7.1 - http://qjson.sourceforge.net/
* SQLite 3.6.22 - http://www.sqlite.org/
* TagLib 1.6.2 - http://developer.kde.org/~wheeler/taglib.html
* Boost 1.3 - http://www.boost.org/
* CLucene 0.9.23 (0.9.21 will fail) - http://clucene.sourceforge.net/download.shtml
* libechonest 2.0.3 - http://projects.kde.org/projects/playground/libs/libechonest/
* Attica 0.4.0 - ftp://ftp.kde.org/pub/kde/stable/attica/
* QuaZip 0.4.3 - http://quazip.sourceforge.net/
* liblastfm 1.0.1 - https://github.com/lastfm/liblastfm/

The following dependencies are optional, but recommended:

* Jreen 1.0.5 (1.1.0 will fail, 1.1.1 is fine) - http://qutim.org/jreen/
* QTweetLib 0.5.0 - https://github.com/minimoog/QTweetLib/

Third party libraries that we ship with our source:

* MiniUPnP 1.6 - http://miniupnp.free.fr/
* Breakpad - http://code.google.com/p/google-breakpad/
* Qocoa - https://github.com/mikemcquaid/Qocoa/
* libqnetwm - http://code.google.com/p/libqnetwm/
* libqxt (QxtWeb module) - http://libqxt.org/
* SPMediaKeyTap - https://github.com/nevyn/SPMediaKeyTap/
* kdSingleApplicationGuard - http://www.kdab.com/

Enjoy!




Troubleshooting
---------------

Sync geht nur mit internet
aber in privat modus


Armel Tomahawk does not run on armhf system
Error message like:
	-bash: ./tomahawk: No such file or directory


Fix it with

	$ sudo ln -s /lib/ld-linux-armhf.so.3 /lib/ld-linux.so.3
	




GStreamer audio output is scattering:
Try tu run
	
	$ speaker-test
	
and

	$ gst-launch playbin uri=file:///.../test.mp3 audio-sink="osssink device=/dev/snd/dsp"
	
If there is no problem, use the oss playback device from alsa with gstreamer.

	$ export PHONON_GST_AUDIOSINK="osssink"
	$ export AUDIODEV="/dev/snd/dsp"

This device could be set to default by the following command.
But this parameters will not used by playbin and phonon.
	
	$ gconftool-2 --set /system/gstreamer/0.10/default/musicaudiosink --type string "osssink device=/dev/snd/dsp"
	$ gconftool-2 --set /system/gstreamer/0.10/default/audiosink --type string "osssink device=/dev/snd/dsp"
	




Tomahawk crashes, because of an video device which will be used by GStreamer:

The shown warning could be something like this
"libv4lconvert: warning more framesizes then I can handle!".

The video device is not needed by tomahawk,
so it can be removed or the rights for accessing it can be removed.
	
	$ sudo chmod o-rwx /dev/video0
	

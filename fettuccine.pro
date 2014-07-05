# Add files and directories to ship with the application 
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
DEPLOYMENTFOLDERS = # file1 dir1

symbian:TARGET.UID3 = 0xE3988A06

# Smart Installer package's UID
# This UID is from the protected range 
# and therefore the package will fail to install if self-signed
# By default qmake uses the unprotected range value if unprotected UID is defined for the application
# and 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=

QT += network maemo5

SOURCES += main.cpp mainwindow.cpp \
    fetcamwidgetitem.cpp \
    fetcamselectordialog.cpp \
    fetcaminfodialog.cpp \
    fetmessenger.cpp \
    fetcamimage.cpp \
    fettagdialog.cpp \
    fetloadfiledialog.cpp \
    fetwidgetsettingsdialog.cpp \
    qmaemo5homescreenadaptor.cpp \
    fetaboutdialog.cpp
HEADERS += mainwindow.h \
    fetcamwidgetitem.h \
    fetcamselectordialog.h \
    fetcaminfodialog.h \
    fetmessenger.h \
    fettargetarchitecture.h \
    fetcamimage.h \
    fettagdialog.h \
    fetloadfiledialog.h \
    fetwidgetsettingsdialog.h \
    qmaemo5homescreenadaptor.h \
    fetaboutdialog.h
FORMS += mainwindow.ui \
    fetcamselectordialog.ui \
    fetcaminfodialog.ui \
    fettagdialog.ui \
    fetloadfiledialog.ui \
    fetwidgetsettingsdialog.ui \
    fetaboutdialog.ui

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qtc_packaging/debian_fremantle/rules \
    qtc_packaging/debian_fremantle/README \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog \
    README.md \
    COPYING \
    LICENSE.md

RESOURCES += \
    fettuccine.qrc

# Extra files needed to create a homescreen widget:
desktop.path = /usr/share/applications/hildon-home
desktop.files = fettuccinewidget.desktop

INSTALLS += desktop

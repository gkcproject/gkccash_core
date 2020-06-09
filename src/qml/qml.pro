QT += quick quickcontrols2 widgets network
CONFIG += c++11 static


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += ENABLE_WALLET  GKC_BUILD HAVE_BUILD_INFO ETH_FATDB=1 HAVE_CONFIG_H DPOS


TARGET = gkc-qml



#--------------ENV  start------------------------------
win32{
#    depends_prefix=$$PWD/../../depends/x86_64-w64-mingw32
}
unix:!macx{
    depends_prefix=$$PWD/../../depends/x86_64-pc-linux-gnu
}

SYSROOT_PREFIX = $$depends_prefix
QT_INCLUDEPATH_PREFIX = $$SYSROOT_PREFIX/qml/include
QT_LIBPATH_PREFIX = $$SYSROOT_PREFIX/qml/lib


TOOLCHAIN_INCLUDE_PATH = $$SYSROOT_PREFIX/include
TOOLCHAIN_LIB_PATH = $$SYSROOT_PREFIX/lib



LIBS += -L$$QT_LIBPATH_PREFIX

INCLUDEPATH += $$TOOLCHAIN_INCLUDE_PATH
INCLUDEPATH += $$QT_INCLUDEPATH_PREFIX
INCLUDEPATH += $$QT_INCLUDEPATH_PREFIX/QtANGLE
INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../leveldb/include
INCLUDEPATH += $$PWD/../univalue/include

INCLUDEPATH += $$PWD/../contract_api
INCLUDEPATH += $$PWD/../contract
INCLUDEPATH += $$PWD/../contract/utils

#--------------ENV  Stop------------------------------



# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = $$PWD/images/icons/app.ico

SOURCES += main.cpp \
    lightwallet.cpp \
    bitcoinapplication.cpp \
    bitcoincore.cpp \
    qt_native/addresstablemodel.cpp \
    qt_native/bitcoinaddressvalidator.cpp \
    qt_native/bitcoinamountfield.cpp \
    qt_native/bitcoinunits.cpp \
    qt_native/clientmodel.cpp \
    qt_native/csvmodelwriter.cpp \
    qt_native/gkcstrings.cpp \
    qt_native/guiutil.cpp \
    qt_native/networkstyle.cpp \
	qt_native/notificationlistproxy.cpp\
	qt_native/notificationlistmodel.cpp\
    qt_native/entrustnodelistmodel.cpp\
    qt_native/entrustnodelistproxy.cpp\	
    qt_native/entrustrecordlistmodel.cpp\
    qt_native/entrustrecordlistproxy.cpp\
    qt_native/mainnodelistmodel.cpp\    
    qt_native/mainnodelistproxy.cpp\
    qt_native/notificator.cpp \
    qt_native/optionsmodel.cpp \
    qt_native/paymentrequest.pb.cc \
    qt_native/paymentrequestplus.cpp \
    qt_native/paymentserver.cpp \
    qt_native/peertablemodel.cpp \
    qt_native/platformstyle.cpp \
    qt_native/qvalidatedlineedit.cpp \
    qt_native/qvaluecombobox.cpp \
    qt_native/recentrequeststablemodel.cpp \
    qt_native/splashscreen.cpp \
    qt_native/trafficgraphwidget.cpp \
    qt_native/transactiondesc.cpp \
    qt_native/transactionfilterproxy.cpp \
    qt_native/transactionrecord.cpp \
    qt_native/transactiontablemodel.cpp \
    qt_native/walletmodel.cpp \
    qt_native/walletmodeltransaction.cpp \
    qt_native/winshutdownmonitor.cpp \
	qt_native/mnemonicmanager.cpp\
    bitcoingui.cpp \
    walletmanager.cpp \
    qt_native/rpcconsole.cpp \
    singleitemmodel.cpp \
    qt_native/utilitydialog.cpp \
    qt_native/blockexplorer.cpp \
    qt_native/addressfilterproxy.cpp \
    qt_native/recentrequestsfilterproxy.cpp \
    qt_native/imageprovider.cpp \
    qt_native/contracttablemodel.cpp \
    qt_native/contractfilterproxy.cpp \
    qt_native/abifunctionfield.cpp \
    qt_native/contractabi.cpp \
    qt_native/contractresult.cpp \
    qt_native/createcontractpage.cpp \
    qt_native/execrpccommand.cpp \
    qt_native/abiparam.cpp \
    qt_native/abiparamitem.cpp \
    qt_native/abiparamsfield.cpp \
    qt_native/tokenfilterproxy.cpp \
    qt_native/tokentransactionrecord.cpp \
    qt_native/tokentransactiontablemodel.cpp \
    qt_native/tokentransactiondesc.cpp \
    qt_native/tokenitemmodel.cpp \
    qt_native/token.cpp \
    qt_native/eventlog.cpp \
    qt_native/notificator.cpp \
    qt_native/intro.cpp \
    qt_native/peertableproxy.cpp \
    qt_native/coincontrolmodel.cpp \
    qt_native/coincontrolproxy.cpp \
    qt_native/masternodetablemodel.cpp \
    qt_native/masternodetableproxy.cpp \
    qt_native/bip39.c \
    bip39words.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


HEADERS += \
    nativeeventfilter.h \
    cursorposprovider.h \
    lightwallet.h \
    bitcoinapplication.h \
    bitcoincore.h \
    bitcoinheaders.h \
    qt_native/addresstablemodel.h \
    qt_native/bitcoinaddressvalidator.h \
    qt_native/bitcoinamountfield.h \
    qt_native/bitcoinunits.h \
    qt_native/clientmodel.h \
    qt_native/csvmodelwriter.h \
    qt_native/guiconstants.h \
    qt_native/guiutil.h \
    qt_native/networkstyle.h \	
	qt_native/notificationlistproxy.h\
	qt_native/notificationlistmodel.h\
    qt_native/entrustnodelistmodel.h\
    qt_native/entrustnodelistproxy.h\
    qt_native/entrustrecordlistmodel.h\
    qt_native/entrustrecordlistproxy.h\
    qt_native/mainnodelistmodel.h\
    qt_native/mainnodelistproxy.h\
    qt_native/notificator.h \
    qt_native/optionsmodel.h \
    qt_native/paymentrequest.pb.h \
    qt_native/paymentrequestplus.h \
    qt_native/paymentserver.h \
    qt_native/peertablemodel.h \
    qt_native/platformstyle.h \
    qt_native/qvalidatedlineedit.h \
    qt_native/qvaluecombobox.h \
    qt_native/recentrequeststablemodel.h \
    qt_native/splashscreen.h \
    qt_native/trafficgraphwidget.h \
    qt_native/transactiondesc.h \
    qt_native/transactionfilterproxy.h \
    qt_native/transactionrecord.h \
    qt_native/transactiontablemodel.h \
    qt_native/walletmodel.h \
    qt_native/walletmodeltransaction.h \
    qt_native/winshutdownmonitor.h \
	qt_native/mnemonicmanager.h\
    bitcoingui.h \
    walletmanager.h \
    qt_native/rpcconsole.h \
    singleitemmodel.h \
    qt_native/utilitydialog.h \
    qt_native/blockexplorer.h \
    qt_native/addressfilterproxy.h \
    qt_native/recentrequestsfilterproxy.h \
    qt_native/imageprovider.h \
    qt_native/contracttablemodel.h \
    qt_native/contractfilterproxy.h \
    qt_native/abifunctionfield.h \
    qt_native/contractabi.h \
    qt_native/contractresult.h \
    qt_native/createcontractpage.h \
    qt_native/execrpccommand.h \
    qt_native/abiparam.h \
    qt_native/abiparamitem.h \
    qt_native/abiparamsfield.h \
    qt_native/tokenfilterproxy.h \
    qt_native/tokentransactionrecord.h \
    qt_native/tokentransactiontablemodel.h \
    qt_native/tokentransactiondesc.h \
    qt_native/token.h \
    qt_native/tokenitemmodel.h \
    qt_native/eventlog.h \
    qt_native/notificator.h \
    qt_native/intro.h \
    qt_native/ui_intro.h \
    qt_native/peertableproxy.h \
    qt_native/coincontrolmodel.h \
    qt_native/coincontrolproxy.h \
    qt_native/masternodetablemodel.h \
    qt_native/masternodetableproxy.h \
    qt_native/bip39.h \
    bip39words.h


win32{
    QMAKE_LFLAGS += -std=c++11 -pipe -O2 -Wstack-protector -fstack-protector-all -Wl,--exclude-libs -Wl,ALL -pthread -mwindows -static -Wl,--dynamicbase -Wl,--nxcompat
}

unix:!macx{
    LIBS+= -Wl,-Bdynamic -lGL
}

unix:!macx{
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -std=c++11 -pipe -O2 -Wstack-protector -fstack-protector-all -Wl,--exclude-libs -Wl,ALL -pthread
}





LIBS += -L$$PWD/.. \
        -lbitcoin_server \
        -lbitcoin_wallet \
        -lbitcoin_cli \
        -lbitcoin_zmq \
        -lbitcoin_common \
        -lbitcoin_util

LIBS += -L$$PWD/../crypto \
        -lbitcoin_crypto

LIBS += -L$$PWD/../univalue/.libs \
        -lunivalue

LIBS += -L$$PWD/../libzerocoin \
        -lbitcoin_zerocoin

LIBS += -L$$PWD/../leveldb \
        -lleveldb \
        -lmemenv



win32{
LIBS += -L$$TOOLCHAIN_LIB_PATH \
        -lboost_system-mt-s \
        -lboost_filesystem-mt-s \
        -lboost_program_options-mt-s \
        -lboost_thread_win32-mt-s \
        -lboost_chrono-mt-s \
        -lboost_log-mt-s \
        -lboost_random-mt-s \
        -lprotobuf \
        -lqrencode \
        -ldb_cxx-4.8 \
        -lssl \
        -lcrypto \
        -lminiupnpc \
        -lssp
}

unix:!macx{
LIBS += -L$$TOOLCHAIN_LIB_PATH \
        -lfontconfig \
        -lfreetype \
        -lboost_system-mt \
        -lboost_filesystem-mt \
        -lboost_program_options-mt \
        -lboost_thread-mt \
        -lboost_chrono-mt \
        -lboost_log-mt \
        -lboost_random-mt \
        -lprotobuf \
        -lqrencode \
        -ldb_cxx-4.8 \
        -lssl \
        -lcrypto \
        -lminiupnpc \
        -levent_pthreads \
        -lpthread
}


LIBS += -L$$PWD/../lz4/lib \
        -llz4

LIBS += -L$$PWD/../secp256k1/.libs \
        -lsecp256k1

LIBS += -L$$TOOLCHAIN_LIB_PATH \
        -levent

LIBS += -L$$PWD/../cryptopp \
        -lcryptopp

LIBS += -L$$TOOLCHAIN_LIB_PATH \
        -lzmq \
        -lethash \
        -levmc-loader \
        -levmc-instructions \
        -ljsoncpp \
        -lscrypt \
        -lcryptopp \
        -lrocksdb \
        -lff \
        -lgmp

win32{
    LIBS += -lshlwapi -lcomctl32 -lole32 -lws2_32 -lmswsock  -lgdi32 -liphlpapi -luuid -lmingwthrd -ldwmapi
}

unix:!macx{
    LIBS +=  -lanl
}


lupdate_only{
SOURCES += *.qml \
          qml/*.qml \
          qml/app_dialogs/*.qml \
          qml/app_items/*.qml \
          qml/app_pages/*.qml \
          qml/base_items/*.qml
}

TRANSLATIONS +=  locale/gkc_en.ts \
                 locale/gkc_zh_CN.ts
#                locale/gkc_bg.ts \
#                locale/gkc_de.ts \
#                locale/gkc_es.ts \
#                locale/gkc_fi.ts \
#                locale/gkc_fr.ts \
#                locale/gkc_it.ts \
#                locale/gkc_ja.ts \
#                locale/gkc_pl.ts \
#                locale/gkc_pt.ts \
#                locale/gkc_ru.ts \
#                locale/gkc_sk.ts \
#                locale/gkc_sv.ts \
#                locale/gkc_vi.ts \
#                locale/gkc_zh_TW.ts

DISTFILES += \
    qt_native/BIP39/secp256k1/src/stamp-h1


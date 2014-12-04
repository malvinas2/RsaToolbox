
# Requires Qt 5
QT += core gui widgets network printsupport

INCLUDEPATH += $$PWD \
               $$PWD/General \
               $$PWD/Log \
               $$PWD/Bus \
#              $$PWD/Bus/Tcp \ # Bug in Qt 5 prevents this from working
#               $$PWD/Bus/Rsib \ # Firmware bug
               $$PWD/Bus/No \
               $$PWD/Bus/Visa \
               $$PWD/Instruments \
               $$PWD/Instruments/Vna \
               $$PWD/Instruments/Vna/Dialogs/CalDialog \
               $$PWD/Instruments/Vna/Dialogs/getChannelDialog \
               $$PWD/Instruments/Vna/Dialogs/getPortsDialog \
               $$PWD/Instruments/Vna/Models/VnaModel \
               $$PWD/Instruments/Vna/Models/ChannelsModel \
               $$PWD/Instruments/Vna/Models/CalGroupsModel \
               $$PWD/Instruments/Vna/Views/VnaForm \
               $$PWD/Instruments/Vna/Views/CalGroupPanel \
               $$PWD/Gui/LastPath \
               $$PWD/Gui/ConnectButton \
               $$PWD/Gui/ConnectionTypeComboBox \
               $$PWD/Gui/ErrorLabel \
               $$PWD/Gui/GetSnpFile \
               $$PWD/Gui/ListPanel \
               $$PWD/Gui/PortsLineEdit \
               $$PWD/Gui/Wizard \
               $$PWD/QCustomPlot

# RsVisa:
LIBS += -L$$PWD/Bus/Visa

CONFIG(debug, release|debug) {
  LIBS += -l$$PWD/RsaToolboxd
  LIBS += -l$$PWD/QCustomPlot/qcustomplotd
}
else {
  LIBS += -l$$PWD/RsaToolbox
  LIBS += -l$$PWD/QCustomPlot/qcustomplot
}
LIBS += -L$$PWD
LIBS += -L$$PWD/QCustomPlot/

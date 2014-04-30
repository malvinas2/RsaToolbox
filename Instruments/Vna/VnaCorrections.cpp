

// RsaToolbox includes
#include "General.h"
#include "VnaCorrections.h"
#include "VnaChannel.h"
#include "Vna.h"
using namespace RsaToolbox;

// Qt includes
// #include <Qt>

/*!
 * \class RsaToolbox::VnaCorrections
 * \ingroup VnaGroup
 * \brief The \c %VnaCorrections class
 * controls and queries the measurement
 * corrections applied to a channel.
 */

VnaCorrections::VnaCorrections(QObject *parent) :
    QObject(parent)
{
    placeholder.reset(new Vna());
    _vna = placeholder.data();
    _channel.reset(new VnaChannel());
    _channelIndex = 0;
}

VnaCorrections::VnaCorrections(VnaCorrections &other)
{
    if (other.isFullyInitialized()) {
        _vna = other._vna;
        _channel.reset(new VnaChannel(*other._channel.data()));
        _channelIndex = other._channelIndex;
    }
    else {
        placeholder.reset(new Vna());
        _vna = placeholder.data();
        _channel.reset(new VnaChannel());
        _channelIndex = 0;
    }
}
VnaCorrections::VnaCorrections(Vna *vna, VnaChannel *channel, QObject *parent) :
    QObject(parent)
{
    _vna = vna;
    _channel.reset(new VnaChannel(*channel));
    _channelIndex = channel->index();
}

VnaCorrections::VnaCorrections(Vna *vna, uint channelIndex, QObject *parent) :
    QObject(parent)
{
    _vna = vna;
    _channel.reset(new VnaChannel(vna, channelIndex, this));
    _channelIndex = channelIndex;
}

// Use corrections
bool VnaCorrections::isOn() {
    CorrectionState state = correctionState();
    if (state == APPLIED_CORRECTIONS)
        return(true);
    if (state == INTERPOLATED_CORRECTIONS)
        return(true);
    // else
    return(false);
}
bool VnaCorrections::isOff() {
    return(!isOn());
}
bool VnaCorrections::isPresent() {
    return(correctionState() != NO_CORRECTIONS);
}
bool VnaCorrections::isNotPresent() {
    return(!isPresent());
}
bool VnaCorrections::isInterpolated() {
    return(correctionState() == INTERPOLATED_CORRECTIONS);
}
bool VnaCorrections::isNotInterpolated() {
    return(!isInterpolated());
}
void VnaCorrections::on(bool isOn) {
    QString scpi = ":SENS%1:CORR %2\n";
    scpi = scpi.arg(_channelIndex);
    if (isOn)
        scpi = scpi.arg(1);
    else
        scpi = scpi.arg(0);
    _vna->write(scpi);
}
void VnaCorrections::off(bool isOff) {
    on(!isOff);
}
QString VnaCorrections::calGroup() {
    QString scpi = ":MMEM:LOAD:CORR? %1\n";
    scpi = scpi.arg(_channelIndex);
    return(_vna->query(scpi).remove("\'").trimmed());
}
void VnaCorrections::setCalGroup(QString calGroup) {
    if (calGroup.contains(".cal", Qt::CaseInsensitive) == false)
        calGroup = calGroup + ".cal";
    QString scpi = ":MMEM:LOAD:CORR %1,\'%2\'\n";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg(calGroup);
    _vna->write(scpi);
}
void VnaCorrections::saveToCalGroup(QString calGroup) {
    if (calGroup.contains(".cal", Qt::CaseInsensitive) == false)
        calGroup = calGroup + ".cal";
    QString scpi = ":MMEM:STOR:CORR %1,\'%2\'\n";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg(calGroup);
    _vna->write(scpi);
}
void VnaCorrections::dissolveCalGroupLink() {
    QString scpi = ":MMEM:LOAD:CORR:RES %1\n";
    scpi = scpi.arg(_channelIndex);
    _vna->write(scpi);
}
void VnaCorrections::clear() {
//    _vna->fileSystem().uploadFile();
//    _vna->importCalGroup("DELETE");
//    setCalGroup("DELETE");
//    dissolveCalGroupLink();
//    _vna->deleteCalGroup("DELETE");
} //?


// Correction properties
VnaCalType VnaCorrections::calibrationType() {
    QString scpi = ":SENS%1:CORR:DATA:PAR%2? %3\n";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg("");
    scpi = scpi.arg("TYPE");
    QString result = _vna->query(scpi);
    return(toVnaCalType(result));

}
VnaSweepType VnaCorrections::sweepType() {
    QString scpi = ":SENS%1:CORR:DATA:PAR%2? %3\n";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg("");
    scpi = scpi.arg("STYP");
    QString result = _vna->query(scpi);
    return(toVnaSweepType(result));
}
QVector<uint> VnaCorrections::ports() {
    QString scpi = ":SENS%1:CORR:DATA:PAR%2? %3\n";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg("");
    scpi = scpi.arg("PORT");
    QString result = _vna->query(scpi);
    return(parseUints(result, ","));
}
uint VnaCorrections::points() {
    QString scpi = ":SENS%1:CORR:DATA:PAR%2? %3\n";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg("");
    scpi = scpi.arg("POIN");
    return(_vna->query(scpi).toUInt());
}
double VnaCorrections::startFrequency_Hz() {
    QString scpi = ":SENS%1:CORR:DATA:PAR%2? %3\n";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg("");
    scpi = scpi.arg("STAR");
    return(_vna->query(scpi).toDouble());
}
double VnaCorrections::stopFrequency_Hz() {
    QString scpi = ":SENS%1:CORR:DATA:PAR%2? %3\n";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg("");
    scpi = scpi.arg("STOP");
    return(_vna->query(scpi).toDouble());
}
double VnaCorrections::power_dBm() {
    QString scpi = ":SENS%1:CORR:DATA:PAR%2? %3\n";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg("");
    scpi = scpi.arg("SPOW");
    return(_vna->query(scpi).toDouble());
}


ComplexRowVector VnaCorrections::directivity(uint outputPort, uint inputPort) {
    return(errorValues("DIRECTIVITY", outputPort, inputPort));
}
void VnaCorrections::setDirectivity(ComplexRowVector corrections, uint outputPort, uint inputPort) {
    setErrorValues("DIRECTIVITY", corrections, outputPort, inputPort);
}
ComplexRowVector VnaCorrections::sourceMatch(uint outputPort, uint inputPort) {
    return(errorValues("SRCMATCH", outputPort, inputPort));
}
void VnaCorrections::setSourceMatch(ComplexRowVector corrections, uint outputPort, uint inputPort) {
    setErrorValues("SRCMATCH", corrections, outputPort, inputPort);
}
ComplexRowVector VnaCorrections::reflectionTracking(uint outputPort, uint inputPort) {
    return(errorValues("REFLTRACK", outputPort, inputPort));
}
void VnaCorrections::setReflectionTracking(ComplexRowVector corrections, uint outputPort, uint inputPort) {
    setErrorValues("REFLTRACK", corrections, outputPort, inputPort);
}
ComplexRowVector VnaCorrections::loadMatch(uint outputPort, uint inputPort) {
    return(errorValues("LOADMATCH", outputPort, inputPort));
}
void VnaCorrections::setLoadMatch(ComplexRowVector corrections, uint outputPort, uint inputPort) {
    setErrorValues("LOADMATCH", corrections, outputPort, inputPort);
}
ComplexRowVector VnaCorrections::transmissionTracking(uint outputPort, uint inputPort) {
    return(errorValues("TRANSTRACK", outputPort, inputPort));
}
void VnaCorrections::setTransmissionTracking(ComplexRowVector corrections, uint outputPort, uint inputPort) {
    setErrorValues("TRANSTRACK", corrections, outputPort, inputPort);
}


void VnaCorrections::operator=(VnaCorrections const &other) {
    if (other.isFullyInitialized()) {
        _vna = other._vna;
        _channel.reset(new VnaChannel(*other._channel.data()));
        _channelIndex = other._channelIndex;
    }
    else {
        placeholder.reset(new Vna());
        _vna = placeholder.data();
        _channel.reset(new VnaChannel());
        _channelIndex = 0;
    }
}

// Private
bool VnaCorrections::isFullyInitialized() const {
    if (_vna == NULL)
        return(false);
    if (_vna == placeholder.data())
        return(false);

    //else
    return(true);
}
VnaCorrections::CorrectionState VnaCorrections::correctionState() {
    QString scpi = ":SENS%1:CORR:SST?\n";
    scpi = scpi.arg(_channelIndex);

    QString result = _vna->query(scpi).remove("\'").trimmed();
    return(toCorrectionState(result));
}
VnaCorrections::CorrectionState VnaCorrections::toCorrectionState(QString scpi) {
    if (scpi == "CAL")
        return(APPLIED_CORRECTIONS);
    if (scpi == "CAI")
        return(INTERPOLATED_CORRECTIONS);
    if (scpi == "CAL OFF")
        return(DISABLED_CORRECTIONS);
    if (scpi.isEmpty())
        return(NO_CORRECTIONS);
    // Default
    return(NO_CORRECTIONS);
}
ComplexRowVector VnaCorrections::errorValues(QString term, uint outputPort, uint inputPort) {
    QString scpi = ":SENS%1:CORR:CDAT? \'%2\',%3,%4\n";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg(term);
    scpi = scpi.arg(inputPort);
    scpi = scpi.arg(outputPort);
    return(_vna->queryComplexVector(scpi, complexBufferSize()));
}
void VnaCorrections::setErrorValues(QString term, ComplexRowVector corrections, uint outputPort, uint inputPort) {
    QString scpi = ":SENS%1:CORR:CDAT \'%2\',%3,%4";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg(term);
    scpi = scpi.arg(inputPort);
    scpi = scpi.arg(outputPort);

    bool isReadAscii = _vna->settings().isReadAsciiFormat();
    bool isRead32Bit = _vna->settings().isRead32BitBinaryFormat();

    _vna->settings().setRead64BitBinaryFormat();
    _vna->binaryWrite(scpi.toUtf8() + toBlockDataFormat(corrections));

    if (isReadAscii)
        _vna->settings().setReadAsciiFormat();
    if (isRead32Bit)
        _vna->settings().setRead32BitBinaryFormat();
}

uint VnaCorrections::bufferSize(uint sfk) {
    Q_UNUSED(sfk);
    const uint SIZE_PER_POINT = 8;
    return(SIZE_PER_POINT * points() + 11);
}
uint VnaCorrections::complexBufferSize(uint sfk) {
    return(2 * bufferSize(sfk) - 11);
}
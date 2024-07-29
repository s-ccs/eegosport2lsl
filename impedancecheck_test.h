#define EEGO_SDK_BIND_STATIC// How to bind

#pragma once

#include <QtWidgets/QWidget>
#include <QSlider>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QStatusBar>
#include <QChar>
#include <QLabel>
#include <QString>

#include <QApplication>
#include "ui_impedancecheck_test.h"

#include <eemagine/sdk/factory.h>
#include <lsl/lsl_cpp.h>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>


using namespace std;

const float MIN_VALUE = 0;
const float MAX_VALUE = 100;

class ImpedanceCheck_test : public QWidget
{
	Q_OBJECT

private:
    Ui::ImpedanceCheck_testClass ui;



	eemagine::sdk::amplifier* amp;
	eemagine::sdk::stream* amplifierStream;

    std::vector<std::string> channelLabels;
	unsigned long long ref_bitmask = 0xffffffffffffffff;
	unsigned long long bip_bitmask = 0x0000000000000000;
	int ampCounter = 0;
	const int kilo = 1000;
	QString kiloOhm = QString("k")+QString((QChar)937);
public:
    ImpedanceCheck_test(QWidget *parent = Q_NULLPTR);
	~ImpedanceCheck_test();
	bool doShowImpedances = false;
	bool doStreamEEG = false;
	bool closeALL = false;
	bool saveImpedancesFlag = false;

	bool initiate_impedance_stream = true;
	bool initiate_eeg_stream = false;
	bool LSLisUninitialized = true;

	int Newframesize;
	int NewSpacerValue;
	int NewFontSize;
	int samplingRate = 500;
	int greenbound;
	int yellowbound;
	int orangebound;
	int redbound;

    int nChannels;

	//bitmasks for channel setting
	const unsigned long long full_bitmask = 0xffffffffffffffff;
	const unsigned long long empty_bitmask = 0x0000000000000000;

	void streamEEGToLSL();
	void showImpedancesInGUI();

	void updateChannelsInGUI();

    QVector<QLabel*> labels;
    QWidget* main = new QWidget;
	QRect mainScreenSize;


	QVector<QVector<QString>> info; // Channelnumber (Name, Xpos, Ypos)

	QSlider* m_slider = new QSlider(Qt::Horizontal, this);
	QLabel *labelFramesize = new QLabel(this);

	QSlider* spacer_slider = new QSlider(Qt::Horizontal, this);
	QLabel *labelSpacer = new QLabel(this);

	QSlider *fontsize_slider = new QSlider(Qt::Horizontal, this);
	QLabel *labelfontsize = new QLabel(this);

	QSpinBox *greenbound_spinbox = new QSpinBox(this);
	QLabel *greenbound_label = new QLabel(this);

	QSpinBox *yellowbound_spinbox = new QSpinBox(this);
	QLabel *yellowbound_label = new QLabel(this);

	QSpinBox *orangebound_spinbox = new QSpinBox(this);
	QLabel *orangebound_label = new QLabel(this);

	QComboBox *eegSamplingRate = new QComboBox(this);

	QCheckBox *streamBipolarChannels = new QCheckBox(this);

	//QLabel *channelCountInfo = new QLabel();
	QStatusBar *statusBar = new QStatusBar();
	QLabel *statusIndicator = new QLabel();
private slots:
	void setButton();
	void resetGUIButton();
	void updateFramesize(int newframesize);
	void updateSpacerValue(int newspacervalue);
	void updateFontSize(int newfontsize);
	void setBipChannels();
	void setSamplingRate(QString);

	/*void updateGreenbound(int newvalue);

	void updateYellowbound(int newvalue);

	void updateOrangebound(int orangebound);
	*/

	//void streamImpedance(int NumberOfChannels);
	//void setSamplingRate();
	void enableEEG();
	void stopstreamEEG();
	void saveImpedances();
	void closeEvent(QCloseEvent *event);

};

#define EEGO_SDK_BIND_STATIC // How to bind

#include "impedancecheck_test.h"
#include <eemagine/sdk/amplifier.h>
#include <eemagine/sdk/channel.h>
#include <eemagine/sdk/factory.h>
#include <eemagine/sdk/stream.h>
#include <eemagine/sdk/wrapper.cc>

#include <QDesktopWidget>
#include <QApplication>
#include <QCoreApplication>
#include <QComboBox>
#include <QDebug>
#include <QLabel>
#include <QDateTime>
#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QString>
#include <QShortcut>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>  
#include <sstream>
#include <chrono>



using namespace lsl;
using namespace eemagine::sdk;

void ImpedanceCheck_test::setButton()
{
	closeALL = true;
	doShowImpedances = false;
	doStreamEEG = false;

}


void ImpedanceCheck_test::enableEEG()
{
	doShowImpedances = false;
	initiate_eeg_stream = true;

	statusIndicator->setStyleSheet("QLabel { background-color : yellowgreen; color : black; }");
	statusIndicator->setText("  Status: STREAMING EEG");
	//lock sampling rate settings
    eegSamplingRate->setEnabled(false);

}

void ImpedanceCheck_test::stopstreamEEG()
{
	doStreamEEG = false;
	initiate_impedance_stream = true;

	statusIndicator->setStyleSheet("QLabel { background-color : tomato; color : black; }");
	statusIndicator->setText("  Status: IMPEDANCE CHECK");

	QMessageBox errorBox;
	errorBox.setWindowTitle("eego LSL App");
	errorBox.setText("EEG stream stopped, impedance stream up again");
	errorBox.exec();
}


void ImpedanceCheck_test::saveImpedances()
{
	saveImpedancesFlag = true;
}


void ImpedanceCheck_test::updateFramesize(int newframesize)
{
	Newframesize = m_slider->value();

	updateChannelsInGUI();

}


void ImpedanceCheck_test::closeEvent(QCloseEvent *event)
{
	closeEvent(event);
}

//resetGUIButton(): fits ectrode labels to window size.
void ImpedanceCheck_test::resetGUIButton()
{
	NewSpacerValue = 4;
	Newframesize = 75;
	NewFontSize = 11;
	spacer_slider->setValue(NewSpacerValue);
	m_slider->setValue(Newframesize);
	fontsize_slider->setValue(NewFontSize);

	//updateChannelsInGUI();
}
void ImpedanceCheck_test::updateSpacerValue(int newspacervalue)
{
	NewSpacerValue = spacer_slider->value();

	updateChannelsInGUI();
}

void ImpedanceCheck_test::updateFontSize(int newfontsize)
{
	NewFontSize = fontsize_slider->value();

	updateChannelsInGUI();
}

void ImpedanceCheck_test::setBipChannels()
{
	//if (streamBipolarChannels->isChecked())
		this->bip_bitmask = full_bitmask;
	//else
	//	this->bip_bitmask = empty_bitmask;
	qDebug() << "toggled bip channels to 0x" << QString::number(this->bip_bitmask,16) << endl;

}


void ImpedanceCheck_test::setSamplingRate(QString srate) {

	int samplerate = srate.toInt();
    qDebug() << "setting samplingrate to int:" << samplerate << endl;

	this->samplingRate = samplerate;

}

//void ImpedanceCheck_test::updateGreenbound(int)
//{
//	 greenbound = (greenbound_spinbox->value());
//}
//
//void ImpedanceCheck_test::updateYellowbound()
//{
//	yellowbound = yellowbound_spinbox->value();
//}
//
//void ImpedanceCheck_test::updateOrangebound()
//{
//	orangebound = orangebound_spinbox->value();
//}

ImpedanceCheck_test::ImpedanceCheck_test(QWidget *parent)
	: QWidget(parent)
{

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//get positions and labels from the txt file
	/*
	QMessageBox msgBox1;
	msgBox1.setWindowTitle("Notice");
	msgBox1.setText("Please make sure, that the 2D position in the .txt-file should have the following form:\n \n Fp1  82.7	32.28 \n Fpz  90  -0.1 \n Fp2       82.7 -32.38 \n \n there should be only blanks inbetween.");
	msgBox1.exec();
	*/
	QString	fileName = QFileDialog::getOpenFileName(this, "Select cap-postion file (.txt or .elc)");

	string filestring = fileName.toStdString();
	string line;
	vector<string> text;
	QVector<QString> subinfo;

	ifstream myfile(filestring);


	if (!myfile) //Always test the file open.
	{
		cout << "Error opening electrode position file" << endl;
		system("pause");

	}

	while (std::getline(myfile, line))
	{
		text.push_back(line);
	}

	for (int i = 0; i < text.size(); i++)
	{
		std::istringstream iss(text.at(i));

		do
		{
			std::string sub;
			iss >> sub;
			QString qstr = QString::fromStdString(sub);
			subinfo.push_back(qstr);
		} while (iss);

		info.push_back(subinfo);
		subinfo.clear();

	}

	// last 2 channels are REF and GND
	nChannels = info.size() - 2;
	qDebug() << nChannels << endl;

	//------------------------------------------------------------------------------------------------------------------------------------------------
	//Generate GUI

	main->setWindowTitle("eego LSL App");
	QDesktopWidget desktop;
	mainScreenSize = desktop.availableGeometry(desktop.primaryScreen());
	//mainScreenSize.setWidth(mainScreenSize.width()*0.8);
	//mainScreenSize.setHeight(mainScreenSize.height()*0.8);
	//z->setVisible(true);

	main->setStyleSheet("background-color:gray;");

	m_slider->setMinimum(30);
	m_slider->setMaximum(100);
	m_slider->setParent(main);
	m_slider->setValue(75);
	m_slider->setGeometry(mainScreenSize.width() - 220, 50, 200, 22);
	connect(m_slider, SIGNAL(valueChanged(int)),
		this, SLOT(updateFramesize(int)));

	QFont f("Arial", 8);
	labelFramesize->setText(QString("change Framesize:"));
	QFontMetrics fm(f);
	labelFramesize->setFont(f);
	labelFramesize->setParent(main);
	labelFramesize->setGeometry(mainScreenSize.width() - 220, 30, 200, 22);
	labelFramesize->setVisible(true);

	spacer_slider->setMinimum(3);
	spacer_slider->setMaximum(9);
	spacer_slider->setParent(main);
	spacer_slider->setValue(8);
	spacer_slider->setGeometry(mainScreenSize.width() - 220, 100, 200, 22);
	connect(spacer_slider, SIGNAL(valueChanged(int)),
		this, SLOT(updateSpacerValue(int)));


	labelSpacer->setText(QString("change spacervalue:"));
	labelSpacer->setFont(f);
	labelSpacer->setParent(main);
	labelSpacer->setGeometry(mainScreenSize.width() - 220, 80, 200, 22);
	labelSpacer->setVisible(true);

	labelfontsize->setText(QString("change fontsize:"));
	labelfontsize->setFont(f);
	labelfontsize->setParent(main);
	labelfontsize->setGeometry(mainScreenSize.width() - 220, 130, 200, 22);
	labelfontsize->setVisible(true);

	fontsize_slider->setMinimum(4);
	fontsize_slider->setMaximum(15);
	fontsize_slider->setParent(main);
	fontsize_slider->setValue(12);
	fontsize_slider->setGeometry(mainScreenSize.width() - 220, 150, 200, 22);
	connect(fontsize_slider, SIGNAL(valueChanged(int)),
		this, SLOT(updateFontSize(int)));

	QPushButton *doneButton = new QPushButton(this);
	doneButton->setText("Done");
	doneButton->setParent(main);
	doneButton->setGeometry(mainScreenSize.width() - 200, mainScreenSize.height() - 200, 80, 40);
	doneButton->setStyleSheet("QPushButton { background-color : yellowgreen; color : black; }");
	connect(doneButton, SIGNAL(clicked()), this, SLOT(setButton()));

	QPushButton *resetGUIButton = new QPushButton(this);
	resetGUIButton->setText("Reset");
	resetGUIButton->setParent(main);
	resetGUIButton->setGeometry(mainScreenSize.width() - 200, 180, 80, 40);
	resetGUIButton->setStyleSheet("QPushButton { background-color : white; color : black; }");
	connect(resetGUIButton, SIGNAL(clicked()), this, SLOT(resetGUIButton()));

	/*
	QPushButton *streamImpedance = new QPushButton(this);
	streamImpedance->setText("stream Impedance to LSL");
	streamImpedance->setParent(z);
	streamImpedance->setGeometry(20, 230, 180, 40);
	streamImpedance->setStyleSheet("QButton { background-color : green; color : black; }");
	connect(streamImpedance, SIGNAL(clicked()), this, SLOT(streamImpedance()));
	*/
	QLabel *eegSamplingRateLabel = new QLabel(this);
	eegSamplingRateLabel->setText("Sampling Rate (Hz):");
	eegSamplingRateLabel->setParent(main);
	eegSamplingRateLabel->setGeometry(20, 60, 180, 22);
	eegSamplingRateLabel->setStyleSheet("QLabel { background-color : white; color : black; }");
	eegSamplingRateLabel->setVisible(true);

	QComboBox *eegSamplingRate = new QComboBox(this);
	eegSamplingRate->setParent(main);
	eegSamplingRate->setGeometry(20, 80, 180, 22);
	eegSamplingRate->setStyleSheet("QComboBox { background-color : white; color : black; }");
	eegSamplingRate->setVisible(true);


	// commented out because the streamed channels are now only determined by the channel definition file that is being loaded!
	/*
	streamBipolarChannels->setText("Stream additionally Bipolar Channels");
	streamBipolarChannels->setParent(main);
	streamBipolarChannels->setGeometry(20, 160, 250, 22);
	streamBipolarChannels->setStyleSheet("QCheckBox { background-color : white; color : black; }");
	streamBipolarChannels->setChecked(false);
	connect(streamBipolarChannels, SIGNAL(clicked()), this, SLOT(setBipChannels()));
	*/

	QPushButton *enableEEG = new QPushButton(this);
	enableEEG->setText("Stream EEG to LSL");
	enableEEG->setParent(main);
	enableEEG->setGeometry(20, 300, 180, 40);
	enableEEG->setStyleSheet("QPushButton { background-color : white; color : black; }");
	connect(enableEEG, SIGNAL(clicked()), this, SLOT(enableEEG()));
	QShortcut *eegKey = new QShortcut(QKeySequence("E"), main);
	connect(eegKey, SIGNAL(activated()), this, SLOT(enableEEG()));

	QPushButton *stopstreamEEG = new QPushButton(this);
	stopstreamEEG->setText("Stop EEG stream");
	stopstreamEEG->setParent(main);
	stopstreamEEG->setGeometry(20, 350, 180, 40);
	stopstreamEEG->setStyleSheet("QPushButton { background-color : white; color : black; }");
	connect(stopstreamEEG, SIGNAL(clicked()), this, SLOT(stopstreamEEG()));

	QPushButton *saveImpedancesButton = new QPushButton(this);
	saveImpedancesButton->setText("Save Impedances (I)");
	saveImpedancesButton->setParent(main);
	saveImpedancesButton->setGeometry(20, 400, 180, 40);
	saveImpedancesButton->setStyleSheet("QPushButton { background-color : white; color : black; }");
	connect(saveImpedancesButton, SIGNAL(clicked()), this, SLOT(saveImpedances()));
	QShortcut *impKey = new QShortcut(QKeySequence("I"), main);
	connect(impKey, SIGNAL(activated()), this, SLOT(saveImpedances()));

	//impedances configuration
	greenbound_label->setText(QString("Impedances threshold green (") + kiloOhm + ")");
	greenbound_label->setParent(main);
	greenbound_label->setGeometry(20, 480, 220, 22);
	greenbound_label->setVisible(true);


	greenbound_spinbox->setStyleSheet("QSpinBox { background-color : yellowgreen; color : black; }");
	greenbound_spinbox->setMinimum(0);
	greenbound_spinbox->setMaximum(99999999999999999);
	greenbound_spinbox->setParent(main);
	greenbound_spinbox->setValue(10);
	greenbound_spinbox->setGeometry(20, 500, 220, 22);
	//connect(greenbound_spinbox, SIGNAL(valueChanged()),
	//	this, SLOT(updateGreenbound()));


	yellowbound_label->setText(QString("Impedances threshold yellow (") + kiloOhm + ")");
	yellowbound_label->setParent(main);
	yellowbound_label->setGeometry(20, 520, 220, 22);
	yellowbound_label->setVisible(true);

	yellowbound_spinbox->setStyleSheet("QSpinBox { background-color : gold; color : black; }");
	yellowbound_spinbox->setMinimum(0);
	yellowbound_spinbox->setMaximum(99999999999999999);
	yellowbound_spinbox->setParent(main);
	yellowbound_spinbox->setValue(20);
	yellowbound_spinbox->setGeometry(20, 540, 220, 22);
	//connect(yellowbound_spinbox, SIGNAL(valueChanged()),
	//	this, SLOT(updateYellowbound()));


	orangebound_label->setText(QString("Impedances threshold orange (") + kiloOhm + ")");
	orangebound_label->setParent(main);
	orangebound_label->setGeometry(20, 560, 220, 22);
	orangebound_label->setVisible(true);

	orangebound_spinbox->setStyleSheet("QSpinBox { background-color : orange; color : black; }");
	orangebound_spinbox->setMinimum(0);
	orangebound_spinbox->setMaximum(99999999999999999);
	orangebound_spinbox->setParent(main);
	orangebound_spinbox->setValue(50);
	orangebound_spinbox->setGeometry(20, 580, 220, 22);
	//connect(orangebound_spinbox, SIGNAL(valueChanged()),
	//	this, SLOT(updateOrangebound()));


	QLabel *ampInfo = new QLabel();
	statusIndicator->setStyleSheet("QLabel { background-color : tomato; color : black; }");
	statusIndicator->setParent(main);
	statusIndicator->setText("  Status: IMPEDANCE CHECK");

	statusIndicator->setGeometry(20, 200, 250, 20);
	statusBar->setParent(main);
	statusBar->setGeometry(0, 0, mainScreenSize.width(), 30);
	//statusBar->addPermanentWidget(statusIndicator);
	statusBar->addWidget(ampInfo);

	

	//---------------------------------------------------------------------------------------------------------------
	/*
	QMessageBox msgBox;
	msgBox.setWindowTitle("Please notice!");
	msgBox.setText("Please press the Done button before closing the app! \n (otherwise the app may continue running in the background)");
	msgBox.exec();

	ui.setupUi(this);
	*/
	

	//no amp pluged in testing
	/*
	while (done == false)
	{

		qApp->processEvents(QEventLoop::AllEvents);


		for (int i = 0; i <= info.size() - 1; i++)
		{

			
			tmp = info.at(i).at(0);
			value = " inf";
			QFont f("Arial", NewFontSize);
			QFontMetrics fm(f);
			labels[i]->setFont(f);
			labels[i]->setAlignment(Qt::AlignCenter);
			labels[i]->setText(tmp + "<br>" + value );
			labels[i]->setParent(z);
			newspacer = (double)NewSpacerValue;
			newspacer = -newspacer;
			Xpos = info.at(i).at(1).toDouble(false);
			Ypos = info.at(i).at(2).toDouble(false);
			labels[i]->setGeometry(offsetX + Ypos*newspacer, offsetY + Xpos*newspacer, Newframesize, Newframesize);
			labels[i]->setStyleSheet("QLabel { background-color : white; color : black; }");

		}

	}
	
	*/

	//------------------------------------------------------------------------------------------------------------------------------------------------
	// eego Amp setup

	
	using namespace eemagine::sdk;

	try {
		
		factory fact;
		std::vector<eemagine::sdk::amplifier *> all_conected_amplifiers = fact.getAmplifiers();


		if (all_conected_amplifiers.empty())
		{
			QMessageBox errorBox;
			errorBox.setWindowTitle("Error: Amp not found");
			errorBox.setText("Please connect the amplifier and make sure that the amp is turned on, then restart the app. Forgot to press 'Done'? Go to Task manager (strg+shift+esc) and close ANT_recordData.exe");
			errorBox.exec();
			main->close();

		}
		else
		{
			ampCounter = all_conected_amplifiers.size();
			qDebug() << "ampcounter " << ampCounter << endl;
			ampInfo->setText("Detected " + QString::number(ampCounter) + " Amplifier(s):");
            for (auto amp : all_conected_amplifiers) {
				auto ampTypeInfo = QString::fromStdString("[" + amp->getType() + "]");
				auto ampSerialInfo = QString::fromStdString("Serial: " + amp->getSerialNumber());
				qDebug() << ampSerialInfo << endl;
				ampInfo->setText(ampInfo->text() + ampTypeInfo + " " + ampSerialInfo + ";");
			}
		}

		if (all_conected_amplifiers.size() == 1) {
			amp = all_conected_amplifiers.at(0);
		}
		else if (all_conected_amplifiers.size() == 2) {

			// Sort numerically on serial numbers
			std::sort(std::begin(all_conected_amplifiers), std::end(all_conected_amplifiers),
				[](const eemagine::sdk::amplifier *amp1,
					const eemagine::sdk::amplifier *amp2) {
				return amp1->getSerialNumber() < amp2->getSerialNumber();
			});

            for (auto amp : all_conected_amplifiers) {
				auto ampTypeInfo = QString::fromStdString("[" + amp->getType() + "]");
                auto ampSerialInfo = QString::fromStdString("Serial after sort: " + amp->getSerialNumber());
				qDebug() << ampSerialInfo << endl;
			}



            amp = fact.createCascadedAmplifier(all_conected_amplifiers);
            qDebug() << "cascaded Amplifier" << this->amp << endl;

        }
		else if (all_conected_amplifiers.size() > 2) {
			QMessageBox errorBox;
			errorBox.setWindowTitle("eego LSL App");
			errorBox.setText("There are more than two amplifiers connected. This is currently not supported!");
			errorBox.exec();
			closeALL = true;
		}

		//Get information about available sampling rate modes
		std::vector<int> availSamplingRates = amp->getSamplingRatesAvailable();
        for  (int sr : availSamplingRates)
		{
			eegSamplingRate->addItem(QString::number(sr));
		}

		//qDebug() << eegSamplingRate->currentText().toInt() << endl;
		eegSamplingRate->setEnabled(true);
		connect(eegSamplingRate, SIGNAL(currentIndexChanged(QString)), this, SLOT(setSamplingRate(QString)));

		//set the index to a default mode (here 1000 Hz ) to ensure pasing right value 
        //int index = eegSamplingRate->findData("1000");
        //if (index != -1){ // -1 for not found
        //    eegSamplingRate->setCurrentIndex(0);
        // else{
        //    eegSamplingRate->setCurrentIndex(index)
        //}
        //qDebug() << "sampling rate index" << QString::number(index) << endl;

        //2024-07-11 we couldnt get the findData method to work and thus manually set the sampling rate to the third item (500,512,_1000_...). Seems to work
        eegSamplingRate -> setCurrentIndex(2);
		this->resetGUIButton();

		main->show();

		while (closeALL == false)
		{

            sleep(1);

			qApp->processEvents(QEventLoop::AllEvents);
			
			if (initiate_impedance_stream) {
				initiate_impedance_stream = false;
                qDebug() << "opening" << endl;

				amplifierStream = amp->OpenImpedanceStream(ref_bitmask);
                qDebug() << "openedImpedanceStream" << endl;
                usleep(1000000); // wait 1s

				doShowImpedances = true;

				showImpedancesInGUI();
			}

			if (initiate_eeg_stream) {
				
				initiate_eeg_stream = false;

				/*
				std::vector<double> ranges = amp->getBipolarRangesAvailable();

				qDebug() << "ranges size" << ranges.size() << endl;

				qDebug() << "ranges 1" << ranges[0] << endl;
				qDebug() << "ranges 2" << ranges[1] << endl;
				qDebug() << "ranges 3" << ranges[2] << endl;
				qDebug() << "ranges 4" << ranges[3] << endl;
				*/


                //amplifierStream = amp->OpenEegStream(this->samplingRate, 0.75, 4.0, full_bitmask, full_bitmask);
                amplifierStream = amp->OpenEegStream(samplingRate);//, 0.75, 4.0, full_bitmask, full_bitmask);
                //amplifierStream = amp->OpenEegStream(this->samplingRate, 0.75, 4.0, empty_bitmask, full_bitmask); //# XXX BIP only VERSION

                usleep(1000000); // wait for 1s
                qDebug() << "OpenEEGStream successful"<< endl;

                amplifierStream->getData();
                buffer buffer1 = amplifierStream->getData();

				int channelCount = buffer1.getChannelCount();

				int sampleCount = buffer1.size() / channelCount;

				if (sampleCount == 0) {
					QMessageBox errorBox;
					errorBox.setWindowTitle("eego LSL app");
					errorBox.setText("No samples in the data. Is the trigger plugged in for a cascaded amplifier?");
					errorBox.exec();

					delete amplifierStream;
					initiate_impedance_stream = true;
				}
				else {

					doStreamEEG = true;
					streamEEGToLSL();
				}


			}

			

		}

		// Make sure to delete the amplifier objects to release resources
		delete amp;
		fact.~factory();
	}
	catch (exceptions::notConnected)
	{
		QMessageBox errorBox;
		errorBox.setWindowTitle("Error");
		errorBox.setText("Amp not found. Please connect the amplifier and make sure that the amp is turned on.");
		errorBox.exec();

		qApp->quit();
	}

	// this only gets reached when all is false, which only happens when pressing the "done" button

	QMessageBox errorBox;
	errorBox.setWindowTitle("eego LSL app");
	errorBox.setText("Streams finished! You can close the window now.");
	errorBox.exec();

	qApp->quit();
	
	//------------------------------------------------------------------------------------------------------------------------------------------------


}



//destructor
ImpedanceCheck_test::~ImpedanceCheck_test()
{

}

void ImpedanceCheck_test::streamEEGToLSL()
{
    buffer buf;

	qDebug() << "passed srate" << this->samplingRate << endl;
	qDebug() << "bip_bimask " << QString::number(this->bip_bitmask, 16) << endl;
	qDebug() << "bip_bimask " << QString::number(this->full_bitmask, 16) << endl;

	std::vector<channel> eegChannelList = amplifierStream->getChannelList();

	//qDebug() << eegChannelList << endl;

	qDebug() << "channelList (size): " << eegChannelList.size() << endl;

    //for (int k = 0; k < eegChannelList.size(); k++)
    //{
    //qDebug() << "channel type: " << eegChannelList[k].getType() << endl;
	/*
	if (eegChannelList[k].getType() == 2)
	{
		QMessageBox errorBox;
		errorBox.setWindowTitle("eego LSL app");
		errorBox.setText("Seems you have not assigned the bipolar channels. They will be counted with channel index");
		errorBox.exec();
		break;
	}
	*/
    //}
	//channelCountInfo->setText("channel count: " + QString::number(eegChannelList.size()));
	// create data streaminfo and append some meta-data
	lsl::stream_info data_info("EEGstream " + amp->getType(), "EEG", nChannels + 1, this->samplingRate, lsl::cf_float32, "eegoSports_" + amp->getType());

	lsl::xml_element channels = data_info.desc().append_child("channels");
	QString label;

	// only stream the amount of channels that are in the info file
	for (int k = 0; k < nChannels; k++) {

        //qDebug() << "channel " << eegChannelList[k].getIndex() << "type  " << eegChannelList[k].getType();// << endl;

		// be sure that only normal referenced or bipolar channels are taken
		if (eegChannelList[k].getType() == 1) {
			label = info.at(k).at(0); //remove colon
			label.chop(1);
            //qDebug() << "taken " << endl;
		}
		else if (eegChannelList[k].getType() == 2)// && this->streamBipolarChannels->isChecked())
		{
			label = QString("Bipolar") + QString::number(k);
            qDebug() << "found and detected BIP channel " << endl;
        }
		else {
			qDebug() << "skipped channel type: " << eegChannelList[k].getType() << endl;
			QMessageBox errorBox;
			errorBox.setWindowTitle("eego LSL App");
			errorBox.setText("Channeltype of channel " + QString::number(k) + " not recognized");
			errorBox.exec();
			continue;
		}

		channels.append_child("channel")
			.append_child_value("label", label.toUtf8().constData())
			.append_child_value("type", "EEG")
			.append_child_value("unit", "microvolts");
	}
	channels.append_child("channel")
		.append_child_value("label", "sampleNumber")
		.append_child_value("type", "MISC")
		.append_child_value("unit", "");

	data_info.desc().append_child("acquisition")
		.append_child_value("manufacturer", "antneuro")
		.append_child_value("serial_number", boost::lexical_cast<std::string>(amp->getSerialNumber()).c_str());
	// make a data outlet

	lsl::stream_outlet data_outlet(data_info);

	// create marker streaminfo and outlet
	lsl::stream_info marker_info("eegoSports-" + amp->getType() + "_markers" + "Markers", "Markers", 1, 0, lsl::cf_string, "eegoSports_" + boost::lexical_cast<std::string>(amp->getType()) + "_markers");
	lsl::stream_outlet marker_outlet(marker_info);


	int microvoltUnit = 1e6;

	QMessageBox errorBox;
	errorBox.setWindowTitle("eego LSL App");
	errorBox.setText("Amp is now streaming EEG data of " + QString::number(nChannels) + " channels to LSL with " + QString::number(this->samplingRate) + "Hz");
	errorBox.exec();

    buf = amplifierStream->getData(); // get

    int channelCount = buf.getChannelCount();
	//qDebug() << "chancount = " << channelCount << endl;

    int sampleCount = buf.size() / channelCount;

    double firstSample_samplerCounter = buf.getSample(channelCount-1,0);

	std::vector<std::vector<float>> send_buffer(sampleCount, std::vector<float>(nChannels + 1));
	int prevError = 0;
	while (doStreamEEG == true) {
        usleep(1000); // faster polling is not better
		qApp->processEvents(QEventLoop::AllEvents);

		try {
            buf = amplifierStream->getData();
            if (buf.size()==0){
                buf = amplifierStream->getData();
            }
		}
		catch (const eemagine::sdk::exceptions::incorrectValue& e) {
			qDebug() << "incorrectValue Error last sample: " << send_buffer[sampleCount][nChannels] << "timestamp " << lsl::local_clock() << endl;

			prevError = 1;
            //std::string exception_string = boost::lexical_cast<std::string>("-1");
            //exception_string = exception_string + "@" + boost::lexical_cast<std::string>(send_buffer[sampleCount][nChannels]);
			// very complex way to write -1@-1, we tried "-1@-1" before and it didnt work, C++ noobies here
            //marker_outlet.push_sample(&exception_string, lsl::local_clock());
            buf = amplifierStream->getData(); // get the buffer again, sometimes it is empty ...?

			continue;
		}
		
		double now = lsl::local_clock();

        channelCount = buf.getChannelCount();

        sampleCount = buf.size() / channelCount;


        //qDebug() << "samplecount: " << sampleCount << "chancount: " <<channelCount << endl;

		std::vector<std::vector<float>> send_buffer(sampleCount, std::vector<float>(nChannels+1));
		for (int s = 0; s < sampleCount; s++) {
			for (int c = 0; c < nChannels; c++) {
                send_buffer[s][c] = microvoltUnit * buf.getSample(c, s);
			}

				// add sample counter
            send_buffer[s][nChannels] = buf.getSample(channelCount-1, s) - firstSample_samplerCounter; //last channel is the sample number
			
		}
		data_outlet.push_chunk(send_buffer, now);

		int last_mrk = 0;
		for (int s = 0; s < sampleCount; s++) {
			//if (int mrk = src_buffer[channelCount + s*(channelCount + 1)]) {
            int mrk = buf.getSample(channelCount-2, s);
			//qDebug() << "Mrk" << mrk << " "<< last_mrk << endl;

			if (mrk != last_mrk) { // send trigger only if we have one sample difference
				if (mrk != 0) { // no need to send off markers
					//qDebug() << "MrkDiff" << mrk << endl;

					std::string mrk_string = boost::lexical_cast<std::string>(mrk);
					mrk_string = mrk_string + "@" + boost::lexical_cast<std::string>(send_buffer[s][nChannels]);
					marker_outlet.push_sample(&mrk_string, now + (s + 1 - sampleCount) / samplingRate);
					last_mrk = mrk;
				}
			}
		}
		if (prevError == 1) {
            qDebug() << "continue after error new sample: " <<endl;//<< send_buffer[sampleCount][nChannels] << "timestamp " << lsl::local_clock() << endl;
			prevError = 0;
		}
	}

	delete amplifierStream;

}

void ImpedanceCheck_test::showImpedancesInGUI()
{
	QString tmp;
	QString value;
	QString chType;
	double* impedanceData;
	buffer buf;
	int kOhm;
	QString chanNumber;


    std::vector<channel> eegChannelList = amplifierStream->getChannelList();

    // initialize LSL stream for Impedances
    lsl::stream_info data_info("ImpedanceStream " + amp->getType(), "EEG", nChannels + 1, 5, lsl::cf_float32, "eegoSportsImpedance_" + amp->getType());

    lsl::xml_element channels = data_info.desc().append_child("channels");
    QString label;

    // only stream the amount of channels that are in the info file
    for (int k = 0; k < nChannels; k++) {

        //qDebug() << "channel " << eegChannelList[k].getIndex() << "type  " << eegChannelList[k].getType();// << endl;

        // be sure that only normal referenced or bipolar channels are taken
        if (eegChannelList[k].getType() == 1) {
            label = info.at(k).at(0); //remove colon
            label.chop(1);
            //qDebug() << "taken " << endl;
        }
        else if (eegChannelList[k].getType() == 2)// && this->streamBipolarChannels->isChecked())
        {
            label = QString("Bipolar") + QString::number(k);
            qDebug() << "found and detected BIP channel " << endl;
        }
        else if (eegChannelList[k].getType() == 5){
            label = QString("impedance_reference") + QString::number(k);
            qDebug() << "impedance_reference detected" << endl;

        }
        else if (eegChannelList[k].getType() == 6){
            label = QString("impedance_ground") + QString::number(k);
            qDebug() << "impedance_ground detected" << endl;

        }


        else {
            qDebug() << "skipped channel type: " << eegChannelList[k].getType() << endl;
            QMessageBox errorBox;
            errorBox.setWindowTitle("eego LSL App");
            errorBox.setText("Channeltype of channel " + QString::number(k) + " not recognized");
            errorBox.exec();
            continue;
        }

        channels.append_child("channel")
            .append_child_value("label", label.toUtf8().constData())
            .append_child_value("type", "EEG")
            .append_child_value("unit", "microvolts");
    }
    channels.append_child("channel")
        .append_child_value("label", "sampleNumber")
        .append_child_value("type", "MISC")
        .append_child_value("unit", "");

    data_info.desc().append_child("acquisition")
        .append_child_value("manufacturer", "antneuro")
        .append_child_value("serial_number", boost::lexical_cast<std::string>(amp->getSerialNumber()).c_str());
    // make a data outlet
    lsl::stream_outlet data_outlet(data_info);
    qDebug() << "Accessing impedance, can take 2-3s to show up" << endl;

    while (doShowImpedances == true)
	{

        usleep(100000); // 100ms wait
		qApp->processEvents(QEventLoop::AllEvents);


//qDebug() << "loop: getData" << endl;


        try{
        buf = amplifierStream->getData();
}	catch (eemagine::sdk::exceptions::internalError)
{
qDebug() << "failed to getData() - caught an internalError (2024-07: used to return what() 'no data'" << endl;
            continue;
        }
        // this showed that the last two channels are type 5 and 6 which is ref and gnd

        std::vector<channel> ampChanlist = amplifierStream->getChannelList();


        impedanceData = buf.data();

        int channelCount = buf.getChannelCount();
        int sampleCount = buf.size() / channelCount;

        double now = lsl::local_clock();

        //qDebug() << "imp-loop: samplecount: " << sampleCount << "chancount: " <<channelCount << endl;
        if (sampleCount == 0) {
            qDebug() << "no new sample, continue with loop" << endl;

            continue;
        }

        std::vector<std::vector<float>> send_buf(sampleCount, std::vector<float>(nChannels+1));
        for (int s = 0; s < sampleCount; s++) {
            for (int c = 0; c < nChannels; c++) {
                send_buf[s][c] = buf.getSample(c, s);
            }
            send_buf[s][nChannels] = buf.getSample(channelCount-1, s); //last channel is the sample number
        }
        data_outlet.push_chunk(send_buf, now);

        //statusBar->addWidget(channelCountInfo);
        int chanOfInfo = -1;
        for (int chanOfList = 0; chanOfList < ampChanlist.size(); chanOfList++)
        //for (int chanOfList = 0; chanOfList < nChannels; chanOfList++) // BIP ONLY

        {
            //qDebug() << "imp-loop: chanOfList"<<chanOfList << endl;

            // ignore chantypes that are not 1 (regular ref channel), 2 (bipolar channels), 5 (ref) or 6(gnd)
            int chanTypeInt = (int)ampChanlist[chanOfList].getType();
            // BUG DUE TO BUGGED SDK: PUT CHANTYPE 2 IN!!!
            //if (!(chanTypeInt == 1 || chanTypeInt == 2 || chanTypeInt == 5 || chanTypeInt == 6)) {
            if (!(chanTypeInt == 1 || chanTypeInt == 5 || chanTypeInt == 6)) {
                //qDebug() << "cont" << endl;
                continue;
            }
            chanOfInfo++;
            //qDebug() << "impedances " << chanTypeInt << endl;


            kOhm = (int)(impedanceData[chanOfList] / 1000.00f);

            chanNumber = QString("%1").arg(chanOfInfo+1);
            tmp = info.at(chanOfInfo).at(0);
            value = QString("%1").arg(kOhm);
            chType = QString("%1").arg((int)ampChanlist[chanOfList].getType());
            //labels[chanOfInfo]->setText(chanNumber + ": " + tmp + "<br>" + value + kiloOhm + "<br>" + chType);
            labels[chanOfInfo]->setText(chanNumber + ":<br>" + tmp + "<br>" + value + kiloOhm);


            greenbound = (greenbound_spinbox->value() * kilo);
            yellowbound = (yellowbound_spinbox->value()* kilo);
            orangebound = (orangebound_spinbox->value() * kilo);

            //coloring labels according to Impedance value in Ohm
            if (impedanceData[chanOfList] > orangebound)//too high, red
            {
                labels[chanOfInfo]->setStyleSheet("QLabel { background-color : tomato; color : black; }");
            }
            else if (impedanceData[chanOfList] <= orangebound && impedanceData[chanOfList] > yellowbound)//high, orange
            {
                labels[chanOfInfo]->setStyleSheet("QLabel { background-color : orange; color : black; }");
            }
            else if (impedanceData[chanOfList] <= yellowbound && impedanceData[chanOfList] > greenbound)//okay, yellow
            {
                labels[chanOfInfo]->setStyleSheet("QLabel { background-color : gold; color : black; }");
            }
            else if (impedanceData[chanOfList] <= (greenbound))// good, green
            {
                labels[chanOfInfo]->setStyleSheet("QLabel { background-color : yellowgreen; color : black; }");
            }
        }

        if (saveImpedancesFlag == true)
        {

            QString impedancesFileName = QFileDialog::getSaveFileName(this, "save impedances");
            if (impedancesFileName.isEmpty()) {//backup
                impedancesFileName = QDate::currentDate().toString("'impedances_'MM_dd_yyyy'.txt'");
            }
            QFile impFile(impedancesFileName);

            impFile.open(QIODevice::WriteOnly | QIODevice::Text);
            /*if (!impFile.open(QFile::WriteOnly | QFile::Text))
            {
            QMessageBox::warning(this, "LSL ANT APP Save Impedances", QString("cannot write file %1:\n%2").arg(QDir::toNativeSeparators(impedancesFileName), impFile.errorString()));
            }
            else {*/

            QTextStream impedances(&impFile);
            QString label;
            for (int i = 0; i < info.size(); i++)
            {
                label = info.at(i).at(0);
                label.chop(1);
                //label // impedance
                impedances << label << "\t" << impedanceData[i] << endl;

            }
            impedances.flush();
            impFile.close();
            QMessageBox::information(this, "LSL ANT APP", QString("File %1 saved in path").arg(QDir::toNativeSeparators(impedancesFileName)));//.arg(QDir::absoluteFilePath));
                                                                                                                                              //}


            saveImpedancesFlag = false;

        }


		

	}

	delete amplifierStream;
}

void ImpedanceCheck_test::updateChannelsInGUI()
{
	//-----------------------------------------------------------------------------------------------------------------------------
	//Place lables according to txt-file data

	double offsetX = (mainScreenSize.width() / 2) + 50;
	double offsetY = (mainScreenSize.height() / 2 - 100);
	double spacer = -3.5;
	//NewSpacerValue = 5;
	//Newframesize = 50;
	//NewFontSize = 6;

	double newspacer;
	double Xpos;
	double Ypos;

	int kOhm = -1;
	int framesize = 1;


	main->adjustSize();

	for (int i = 0; i <= info.size() - 1; i++)
	{
		labels.append(new QLabel);

		//QString tmp = info.at(i).at(0);
		//QString value = " inf";

		//labels[i]->setText(QString("<font size=3>") + tmp +
			//"</font><br><font size=" + Newframesize / 10 + ">" + value + "</font>");
		//labels[i]->setParent(main);

		//double Xpos = info.at(i).at(1).toDouble(false);
		//double Ypos = info.at(i).at(2).toDouble(false);
		//labels[i]->setGeometry(offsetX + Ypos * spacer, offsetY + Xpos * spacer, framesize, framesize);
		//labels[i]->setStyleSheet("QLabel { background-color : white; color : black; }");

		//QString chanNumber = QString("%1").arg(i);
		//value = QString("%1").arg(kOhm);
		QFont f("Arial", NewFontSize);
		QFontMetrics fm(f);
		labels[i]->setFont(f);
		labels[i]->setAlignment(Qt::AlignCenter);
		//labels[i]->setText(chanNumber + ": " + tmp + "<br>" + value + kiloOhm);
		labels[i]->setParent(main);
		newspacer = (double)NewSpacerValue;
		newspacer = -newspacer;
        Xpos = info.at(i).at(1).toDouble();
        Ypos = info.at(i).at(2).toDouble();
		labels[i]->setGeometry(offsetX + Ypos * newspacer, offsetY + Xpos * newspacer, Newframesize, Newframesize);
		//labels[i]->setStyleSheet("QLabel { background-color : white; color : black; }");
	}
}




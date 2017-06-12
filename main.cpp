/*
 * Cautomate.cpp
 *
 *  Created on: 27 juil. 2011
 *      Author: philippe
 */

#include <stdio.h>

#include <iostream> 
#include <stdlib.h>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "Cautomate.h"
#include "CautomateGetSensorValuesAndStop.h"
#include "Csyslog.h"
#include "CsoapServer.h"

#include "version.h"

using namespace std;
namespace po = boost::program_options;

const int defaultPh4Value = 329;
const int defaultPh7Value = 495;
const int defaultCaptureDuration = 10;
const std::string defaultConfigFileName("phiaquarium.conf");
const std::string defaultNeonActivation("NONE");
const int defaultMesureCacheSize = 1000;
const int defaultCaptureFrequencyInMs = 2;

int main(int argc, char* argv[])
{
	int ph4Value, ph7Value;
	int captureDuration = defaultCaptureDuration, mesureCacheSize,
			captureFrequencyInMs;

	std::string configFileName, strWitchNeonToActivate, outputFileName;
	AquaAutomate::CautomateGetSensorValuesAndStop::EneonActivationKind
			neonActivation =
					AquaAutomate::CautomateGetSensorValuesAndStop::ACTIVATE_NO_NEON;
	po::options_description generic("Generic options");
	generic.add_options()(
			"version,v",
			"print version message, like phiaquarium version " PHIAQUARIUM_VERSION)(
			"display_sensor_and_quit,q",
			po::value<int>(),
			"display sensor values and quit after one minute (used to calibrate witch different pH test water)")(
			"force_neon,l",
			po::value<std::string>(&strWitchNeonToActivate)->default_value(
					defaultNeonActivation),
			"force neon activation (only compatible with option display_sensor_anq_quit)\n"
				"and specify combination : ALL, FRONT, REAR, NONE (default)")(
			"help,h", "produce help message")("config_file,f",
			po::value<std::string>(&configFileName)->default_value(
					defaultConfigFileName),
			"config file to be used to set parameters")("output_file,o",
			po::value<std::string>(),
			"output filename used to store result generate by an automate");

	po::options_description config("Configuration");
	config.add_options()("ph4Value", po::value<int>(&ph4Value)->default_value(
			defaultPh4Value), "calibration value for a pH = 4")("ph7Value",
			po::value<int>(&ph7Value)->default_value(defaultPh7Value),
			"calibration value for a pH = 7")("capture_frequency", po::value<
			int>(&captureFrequencyInMs)->default_value(
			defaultCaptureFrequencyInMs), "capture frequency in milli-seconds")(
			"mesure_cache_size,m",
			po::value<int>(&mesureCacheSize)->default_value(
					defaultMesureCacheSize),
			"size of the cache used to smooth variation");

	po::options_description cmdlineOptions;
	cmdlineOptions.add(generic).add(config);

	po::options_description configFileOptions;
	configFileOptions.add(config);

	po::variables_map vm;
	try
	{
		po::store(po::parse_command_line(argc, argv, cmdlineOptions), vm);
	} catch (po::error &exception)
	{
		std::cerr << "Command line error : " << exception.what() << std::endl;
		std::cerr << "Syntax :" << std::endl;
		std::cerr << cmdlineOptions << std::endl;
		return 3;
	}

	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << cmdlineOptions << std::endl;
		return 1;
	}

	if (vm.count("version"))
	{
		std::cout << "phiaquarium version " PHIAQUARIUM_VERSION << std::endl;
		return 2;
	}

	boost::shared_ptr<Syslog::Csyslog> syslog(new Syslog::Csyslog());
	syslog->initialize("automate");

	boost::filesystem::path configFilePath(configFileName);

	if (boost::filesystem::exists(configFilePath))
	{
		std::cout << "Using config file " << configFileName << std::endl;
		syslog->log_info("Using config file %s", configFileName.c_str());
		try
		{
			po::store(po::parse_config_file<char>(configFileName.c_str(),
					configFileOptions), vm);
		} catch (po::error &exception)
		{
			std::cerr << "Config file error : " << exception.what()
					<< std::endl;
			std::cerr << "Syntax :" << std::endl;
			std::cerr << configFileOptions << std::endl;
			return 4;
		}
		po::notify(vm);
	}
	else
	{
		std::cerr << "Missing config file " << configFileName
				<< ", continue and ignoring this file" << std::endl;
	}

	if (vm.count("ph4Value"))
	{
		ph4Value = vm["ph4Value"].as<int> ();
	}

	if (vm.count("ph7Value"))
	{
		ph7Value = vm["ph7Value"].as<int> ();
	}

	if (vm.count("force_neon"))
	{
		strWitchNeonToActivate = vm["force_neon"].as<std::string> ();

		if (strWitchNeonToActivate == "ALL")
		{
			neonActivation
					= AquaAutomate::CautomateGetSensorValuesAndStop::ACTIVATE_ALL_NEON;
		}
		else if (strWitchNeonToActivate == "REAR")
		{
			neonActivation
					= AquaAutomate::CautomateGetSensorValuesAndStop::ACTIVATE_REAR_NEON;

		}
		else if (strWitchNeonToActivate == "FRONT")
		{
			neonActivation
					= AquaAutomate::CautomateGetSensorValuesAndStop::ACTIVATE_FRONT_NEON;
		}
		else if (strWitchNeonToActivate == "NONE")
		{
			neonActivation
					= AquaAutomate::CautomateGetSensorValuesAndStop::ACTIVATE_NO_NEON;
		}
	}

	if (vm.count("mesure_cache_size"))
	{
		mesureCacheSize = vm["mesure_cache_size"].as<int> ();
	}

	if (vm.count("capture_frequency"))
	{
		captureFrequencyInMs = vm["capture_frequency"].as<int> ();
	}

	if (vm.count("output_file"))
	{
		outputFileName = vm["output_file"].as<std::string> ();
	}

	/* Choose the good automate */
	boost::shared_ptr<ItfRunnable::Irunnable> automate;
	boost::shared_ptr<AquaAutomate::CsoapServer> soapServer;

	if (vm.count("display_sensor_and_quit"))
	{
		captureDuration = vm["display_sensor_and_quit"].as<int> ();
		automate = boost::shared_ptr<ItfRunnable::Irunnable>(
				new AquaAutomate::CautomateGetSensorValuesAndStop(syslog,
						neonActivation, captureDuration, ph4Value, ph7Value,
						mesureCacheSize, captureFrequencyInMs, outputFileName));
	}
	else
	{
		boost::shared_ptr<AquaAutomate::Cautomate> automateComplet = boost::shared_ptr<AquaAutomate::Cautomate>(
				new AquaAutomate::Cautomate(syslog, ph4Value, ph7Value,
						mesureCacheSize, captureFrequencyInMs));
		automate = automateComplet;

		soapServer = boost::shared_ptr<AquaAutomate::CsoapServer>(new AquaAutomate::CsoapServer(automateComplet));

		soapServer->start();
	}

	automate->run();

	return 0;
}


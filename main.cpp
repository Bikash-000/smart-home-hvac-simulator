#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <thread>
#include <chrono>

using namespace std;

int main() {

    int choice;

    cout << "\n========================================\n";
    cout << "   SMART HOME OCCUPANCY & HVAC SYSTEM\n";
    cout << "========================================\n";
    cout << "1. Run Smart Home Simulation\n";
    cout << "2. Exit\n";
    cout << "========================================\n";
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice == 2) {
        cout << "\nExiting program...\n";
        return 0;
    }

    if (choice != 1) {
        cout << "\nInvalid choice!\n";
        return 0;
    }

    cout << "\nStarting Smart Home System...\n";

    // ========================================
    // CSV LOG FILE
    // ========================================

    ofstream logFile("smart_home_log.csv", ios::app);

    if (!logFile) {
        cout << "ERROR: Cannot open log file." << endl;
        return 1;
    }

    if (logFile.tellp() == 0) {
        logFile << "Date,Time,Occupants,Temperature,Humidity,AQI,Energy\n";
    }

    // ========================================
    // INITIAL SENSOR READING
    // ========================================

    cout << "\n--- Smart Sensor Driver ---\n";

    ifstream sensor("/dev/smart_sensor");

    if (!sensor.is_open()) {
        cout << "ERROR: Cannot open /dev/smart_sensor" << endl;
        cout << "Make sure the kernel driver is loaded." << endl;
        return 1;
    }

    string line;

    int occupants = 0;
    double temperature = 0;
    double humidity = 0;
    int airQuality = 0;

    while (getline(sensor, line)) {

        cout << line << endl;

        if (line.find("Occupancy:") != string::npos) {

            if (line.find("INACTIVE") != string::npos) {
                occupants = 0;
            }
            else if (line.find("ACTIVE") != string::npos) {
                occupants = 1;
            }
        }

        else if (line.find("Temperature:") != string::npos) {

            sscanf(line.c_str(),
                   "Temperature: %lf C",
                   &temperature);
        }

        else if (line.find("Humidity:") != string::npos) {

            sscanf(line.c_str(),
                   "Humidity: %lf percent",
                   &humidity);
        }

        else if (line.find("AQI:") != string::npos) {

            sscanf(line.c_str(),
                   "AQI: %d",
                   &airQuality);
        }
    }

    sensor.close();

    // ========================================
    // SMART HOME STATUS
    // ========================================

    cout << "\n========================================\n";
    cout << " Smart Home Occupancy & HVAC Simulator\n";
    cout << "========================================\n";

    if (occupants == 0) {
        cout << "Occupancy Status: EMPTY" << endl;
        cout << "Energy Saving Mode: ON" << endl;
    }
    else {
        cout << "Occupancy Status: OCCUPIED" << endl;
        cout << "Energy Saving Mode: OFF" << endl;
    }

    // ========================================
    // HUMIDITY STATUS
    // ========================================

    if (humidity > 70) {
        cout << "Humidity Status: HIGH" << endl;
    }
    else if (humidity < 30) {
        cout << "Humidity Status: LOW" << endl;
    }
    else {
        cout << "Humidity Status: NORMAL" << endl;
    }

    // ========================================
    // AIR QUALITY
    // ========================================

    if (airQuality <= 50) {
        cout << "Air Quality: GOOD" << endl;
    }
    else if (airQuality <= 100) {
        cout << "Air Quality: MODERATE" << endl;
    }
    else {
        cout << "Air Quality: POOR" << endl;
    }

    // ========================================
    // HVAC DECISION
    // ========================================

    cout << "\n--- HVAC Decision ---\n";

    if (occupants == 0) {

        cout << "Home is empty.\n";
        cout << "HVAC: OFF\n";
    }

    else if (temperature > 26) {

        cout << "Occupants detected: "
             << occupants << "\n";

        cout << "Temperature is high.\n";
        cout << "HVAC: COOLING ON\n";

        if (temperature >= 30) {
            cout << "Fan Speed: HIGH\n";
        }
        else {
            cout << "Fan Speed: LOW\n";
        }
    }

    else if (temperature < 18) {

        cout << "Occupants detected: "
             << occupants << "\n";

        cout << "Temperature is low.\n";
        cout << "HVAC: HEATING ON\n";
    }

    else {

        cout << "Occupants detected: "
             << occupants << "\n";

        cout << "Temperature is comfortable.\n";
        cout << "HVAC: OFF\n";
    }

    // ========================================
    // ENERGY CONSUMPTION
    // ========================================

    double energy;

    if (occupants == 0) {
        energy = 0;
    }
    else if (temperature > 26) {
        energy = 2.5;
    }
    else if (temperature < 18) {
        energy = 2.0;
    }
    else {
        energy = 0.5;
    }

    cout << "\n--- Energy Consumption ---\n";
    cout << "Estimated Energy Usage: "
         << energy << " kWh" << endl;

    // ========================================
    // INITIAL CSV LOGGING
    // ========================================

    time_t now = time(0);
    tm *localTime = localtime(&now);

    logFile << put_time(localTime, "%Y-%m-%d") << ","
            << put_time(localTime, "%H:%M:%S") << ","
            << occupants << ","
            << temperature << ","
            << humidity << ","
            << airQuality << ","
            << energy << endl;

    // ========================================
    // ENERGY SAVING RECOMMENDATION
    // ========================================

    cout << "\n--- Energy Saving Recommendation ---\n";

    if (occupants == 0) {

        cout << "Recommendation: "
             << "Turn OFF HVAC to save energy." << endl;
    }

    else if (temperature >= 26 &&
             temperature <= 28) {

        cout << "Recommendation: "
             << "Use moderate cooling to save energy."
             << endl;
    }

    else if (temperature > 28) {

        cout << "Recommendation: "
             << "Cooling required. Use high fan speed."
             << endl;
    }

    else if (temperature < 18) {

        cout << "Recommendation: "
             << "Heating required. Maintain minimum heating."
             << endl;
    }

    else {

        cout << "Recommendation: "
             << "Temperature is comfortable. Keep HVAC OFF."
             << endl;
    }

    cout << "\n--- Data Logging ---\n";
    cout << "Sensor data saved successfully to "
         << "smart_home_log.csv" << endl;

    // ========================================
    // CONTINUOUS SENSOR MONITORING
    // ========================================

    cout << "\n--- Continuous Sensor Monitoring ---\n";

    for (int cycle = 1; cycle <= 5; cycle++) {

        cout << "\nReading "
             << cycle
             << " of 5\n";

        ifstream monitor("/dev/smart_sensor");

        if (!monitor.is_open()) {

            cout << "ERROR: Cannot open "
                 << "/dev/smart_sensor" << endl;

            break;
        }

        string monitorLine;

        int monitorOccupants = 0;
        double monitorTemperature = 0;
        double monitorHumidity = 0;
        int monitorAQI = 0;

        while (getline(monitor, monitorLine)) {

            cout << monitorLine << endl;

            // Occupancy
            if (monitorLine.find("Occupancy:")
                != string::npos) {

                if (monitorLine.find("INACTIVE")
                    != string::npos) {

                    monitorOccupants = 0;
                }
                else if (monitorLine.find("ACTIVE")
                         != string::npos) {

                    monitorOccupants = 1;
                }
            }

            // Temperature
            else if (monitorLine.find("Temperature:")
                     != string::npos) {

                sscanf(monitorLine.c_str(),
                       "Temperature: %lf C",
                       &monitorTemperature);
            }

            // Humidity
            else if (monitorLine.find("Humidity:")
                     != string::npos) {

                sscanf(monitorLine.c_str(),
                       "Humidity: %lf percent",
                       &monitorHumidity);
            }

            // AQI
            else if (monitorLine.find("AQI:")
                     != string::npos) {

                sscanf(monitorLine.c_str(),
                       "AQI: %d",
                       &monitorAQI);
            }
        }

        monitor.close();

        // ====================================
        // MONITORING HVAC DECISION
        // ====================================

        cout << "\n--- HVAC Decision ---\n";

        if (monitorOccupants == 0) {

            cout << "Home is empty.\n";
            cout << "HVAC: OFF\n";
            cout << "Energy Saving Mode: ON\n";
        }

        else if (monitorTemperature > 26) {

            cout << "Occupants detected: "
                 << monitorOccupants << "\n";

            cout << "Temperature is high.\n";
            cout << "HVAC: COOLING ON\n";

            if (monitorTemperature >= 30) {
                cout << "Fan Speed: HIGH\n";
            }
            else {
                cout << "Fan Speed: LOW\n";
            }
        }

        else if (monitorTemperature < 18) {

            cout << "Occupants detected: "
                 << monitorOccupants << "\n";

            cout << "Temperature is low.\n";
            cout << "HVAC: HEATING ON\n";
        }

        else {

            cout << "Occupants detected: "
                 << monitorOccupants << "\n";

            cout << "Temperature is comfortable.\n";
            cout << "HVAC: OFF\n";
        }

        // ====================================
        // MONITORING AQI
        // ====================================

        if (monitorAQI > 100) {

            cout << "Air Quality: POOR - "
                 << "Ventilation recommended.\n";
        }

        else if (monitorAQI > 50) {

            cout << "Air Quality: MODERATE.\n";
        }

        else {

            cout << "Air Quality: GOOD.\n";
        }

        // ====================================
        // MONITORING HUMIDITY
        // ====================================

        cout << "Humidity Status: ";

        if (monitorHumidity < 30) {

            cout << "LOW - Air is dry.\n";
        }

        else if (monitorHumidity <= 60) {

            cout << "COMFORTABLE.\n";
        }

        else {

            cout << "HIGH - "
                 << "Dehumidification recommended.\n";
        }

        // ====================================
        // MONITORING ENERGY
        // ====================================

        cout << "\n--- Energy Consumption ---\n";

        double monitorEnergy;

        if (monitorOccupants == 0) {

            monitorEnergy = 0.0;
        }

        else if (monitorTemperature > 26) {

            monitorEnergy = 2.5;
        }

        else if (monitorTemperature < 18) {

            monitorEnergy = 2.0;
        }

        else {

            monitorEnergy = 0.5;
        }

        cout << "Estimated Energy Usage: "
             << monitorEnergy
             << " kWh\n";

        // ====================================
        // MONITORING CSV LOGGING
        // ====================================

        time_t monitorNow = time(0);
        tm *monitorTime = localtime(&monitorNow);

        logFile << put_time(monitorTime, "%Y-%m-%d")
                << ","
                << put_time(monitorTime, "%H:%M:%S")
                << ","
                << monitorOccupants
                << ","
                << monitorTemperature
                << ","
                << monitorHumidity
                << ","
                << monitorAQI
                << ","
                << monitorEnergy
                << endl;

        // ====================================
        // WAIT FOR NEXT READING
        // ====================================

        if (cycle < 5) {

            cout << "Next reading in 5 seconds...\n";

            this_thread::sleep_for(
                chrono::seconds(5)
            );
        }
    }

    // ========================================
    // CLOSE CSV FILE
    // ========================================

    logFile.close();

    // ========================================
    // ENERGY STATISTICS
    // ========================================

    ifstream dataFile("smart_home_log.csv");

    string dataLine;
    int recordCount = 0;
    double totalEnergy = 0.0;

    if (dataFile.is_open()) {

        getline(dataFile, dataLine);

        while (getline(dataFile, dataLine)) {

            recordCount++;

            size_t lastComma =
                dataLine.rfind(',');

            if (lastComma != string::npos) {

                double loggedEnergy =
                    stod(dataLine.substr(
                        lastComma + 1));

                totalEnergy += loggedEnergy;
            }
        }

        dataFile.close();
    }

    cout << "\n--- Energy Statistics ---\n";

    cout << "Total Records: "
         << recordCount << endl;

    cout << "Total Energy Consumption: "
         << totalEnergy
         << " kWh" << endl;

    if (recordCount > 0) {

        cout << "Average Energy Consumption: "
             << totalEnergy / recordCount
             << " kWh" << endl;
    }

    cout << "\n========================================\n";
    cout << "       MONITORING COMPLETED\n";
    cout << "========================================\n";

    return 0;
}
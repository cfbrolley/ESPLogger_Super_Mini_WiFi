//Libraries. This flight computer uses an MPU6050 over I2C, and a BMP390 and SD reader over SPI.
#include <SdFat.h>
#include <Wire.h> //This can be ditched if everything can be put on an SPI bus to SPEED TOWN yyyyeeEEEAHHH DAWWWWG
#include <SPI.h>
#include <Adafruit_Sensor.h> //wtf does this one even do, idk.
#include <Adafruit_MPU6050.h>
#include <DFRobot_BMP3XX.h>
#include <WiFi.h>
#include <WebServer.h>
#include "Buzzer.h"
#include "Backup_Data.h"
#include "Serial_Debug.h"

//Define constants
#define SEALEVELPRESSURE_HPA 1013.25 
#define droguepin 0
#define mainpin 1
#define mainalti 150 //trigger altitude to activate mainpin
#define buzztime 3000
#define flushtime 2000
#define armcheck 20 //<--This is the minimum height in meters that the deployment charge can go off. Change this for whatever needed.
#define logtime 0 //set logging delay. the logging loop takes approximately 10ms, this is additional time added to the loop
//#define datadebugging

//Variables
float pressure, altitude, altioffset, correctedalt, maxaccel, TTA, ax, ay, az, gx, gy, gz;
float maxalt = 0; 
float alticheck = 0;
byte droguecount = 0;
byte maincount = 0;
byte logtimeout = 0;
long buzzclock;
unsigned long timer, flushclock, apogeetime, liftofftime, timetoapogee;
bool EEPROMenabled = false;
bool armed = false;
bool droguefired = false;
bool mainfired = false;
bool liftoff = false;
bool sensordebugging = false;
bool loggingstarted = false;
int logNumber = 0;
const char* charcomma = ",";

//webserver definitions
const char* ssid = "ESPlogger";  // Enter SSID here
const char* password = "12345678";  //Enter Password here
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

//BMP, MPU and SD things
DFRobot_BMP390L_I2C bmp(&Wire, bmp.eSDOVDD);
Adafruit_MPU6050 mpu; //May need to be changed to 9-axis sensor
SdFat SD;
SdFile logfile;
Buzzer Buzz(3);
Backup_Data Backup(0, 4, 8); //backup logging object - takes EEPROM addresses for 3 data points
Serial_Debug Debug(115200);

void setup() {
//start buzzer library and serial
  Wire.begin();
  Debug.begin();
  Wire.setClock(1000000);

//setup begin tone
  Buzz.begin();
  Buzz.startup();

//setup pins
  pinMode (mainpin, OUTPUT);
  pinMode (droguepin, OUTPUT);
  pinMode(SS, OUTPUT);

//start BMP390
  int rslt;
  while( ERR_OK != (rslt = bmp.begin()) ){
    if(ERR_DATA_BUS == rslt){
      Debug.debugBMP(1);
      Buzz.error();
      while(1);
      }
      else if(ERR_IC_VERSION == rslt){
             Debug.debugBMP(2);
             Buzz.error();
             while(1);
             }
    bmp.setPWRMode(bmp.ePressEN | bmp.eTempEN | bmp.eNormalMode);
    bmp.setOSRMode(bmp.ePressOSRMode2 | bmp.eTempOSRMode1);
    bmp.setODRMode(BMP3XX_ODR_200_HZ);
    bmp.setIIRMode(BMP3XX_IIR_CONFIG_COEF_7);
    delay(2000);
    pressure = bmp.readPressPa();
    altioffset = 44330.0 * (1.0 - pow(pressure / SEALEVELPRESSURE_HPA, 0.1903)); //Figure out an offest for height above ground. There's a better way to do this with the BMP probably.
    }
//start MPU6050
  if (mpu.begin()) {
     mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
     mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
     mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
     }
  else {
       Debug.debugIMU();
       Buzz.error();
       while(1);
       }

  if (SD.begin(7, SD_SCK_MHZ(16))) {     
     #define COMMA logfile.print(",");
     while (SD.exists("FltLog" + String(logNumber) + ".csv")) {
     logNumber++;
     }
     char filename[20];
     snprintf(filename, sizeof(filename), "FltLog%d.csv", logNumber);
     logfile.open(filename, O_WRITE | O_CREAT);     
     delay(2000); //delays are yuck but this might be necessary because the SD card initialises so fast now. Issues seem to happen without it.
     logfile.sync();
     logfile.println();
     logfile.println("~ SUPER MINI LOGGER BETA ~");
     logfile.println();
     logfile.println("ms,pres,alt,rel. alt,ax,ay,az,gx,gy,gz");
     logfile.sync();    
     }
  else {
       EEPROMenabled = true; //EEPROM has limited writes, so to save EEPROM wear, writes will only occur if EEPROMenabled is true (so like... when my dumb ass forgets the SD card...).
       Debug.debugSD();
       Buzz.error();
       while (1); //stopper - this can be changed to a prompt once the EEPROM code is ready to allow logger to continue without the SD card
       }

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_SplashPage);
  server.on("/startLogging", handle_Logging);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");

//indicate setup is done.
  Buzz.success();
  Serial.println("Setup success");
  Wait();
}

void handle_SplashPage() {
  server.send(200, "text/html", SplashPage());
}

void handle_Logging() {
  loggingstarted = true;
  server.send(200, "text/html", SendHTML(pressure,altitude,correctedalt,maxaccel, ax, ay, az, gx, gy, gz)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

void Wait() {
  while (!loggingstarted) {
        delay(100);
        server.handleClient();
        }
}

String SplashPage() {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP32 Data Logger</title>\n";
  ptr +="<style>html { font-family: Verdana; display: inline-block; margin: 0px auto; text-align: center; border: 5px solid black;} #grad1 {height: 650px; background-color: dimgray; background-image: linear-gradient(red, firebrick, dimgray);}";
  ptr +="body{margin-top: 50px;} h1 {color: white;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 18px;color: white;margin-bottom: 5px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body id=grad1>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>Rolley Rocketry</h1>\n";
  ptr +="<h2>ESP Logger v0.1</h2>\n";
  ptr +="<h2>Start logging:</h2>\n";
  ptr += "<a href='/startLogging'>Start Logging</a>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

String SendHTML(float pressure,float altitude,float correctedalt,float maxaccel,float ax,float ay,float az,float gx,float gy,float gz){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP32 Data Logger</title>\n";
  ptr +="<style>html { font-family: Verdana; display: inline-block; margin: 0px auto; text-align: center; border: 5px solid black;} #grad1 {height: 650px; background-color: dimgray; background-image: linear-gradient(red, firebrick, dimgray);}";
  ptr +="body{margin-top: 50px;} h1 {color: white;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 18px;color: white;margin-bottom: 5px;}\n";
  ptr +="</style>\n";
  ptr +="<script>\n";
  ptr +="setInterval(loadDoc,500);\n";
  ptr +="function loadDoc() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.body.innerHTML =this.responseText}\n";
  ptr +="};\n";
  ptr +="xhttp.open(\"GET\", \"/startLogging\", true);\n";
  ptr +="xhttp.send();\n";
  ptr +="}\n";
  ptr +="</script>\n";
  ptr +="</head>\n";
  ptr +="<body id=grad1>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>Rolley Rocketry</h1>\n";
  ptr +="<h2>ESP Logger v0.1</h2>\n";
  ptr +="<p style=text-align:left>Pressure: ";
  ptr +=pressure;
  ptr +="mb</p>";
  ptr +="<p style=text-align:left>Altitude: ";
  ptr +=altitude;
  ptr +="m</p>";
  ptr +="<p style=text-align:left>Corrected altitude: ";
  ptr +=correctedalt;
  ptr +="m</p>";
  ptr +="<p style=text-align:left>Max Z axis acceleration: ";
  ptr +=maxaccel;
  ptr +="mss</p>";
  ptr +="<p style=text-align:left>Current x axis acceleration: ";
  ptr +=ax;
  ptr +="mss</p>";
  ptr +="<p style=text-align:left>Current y axis acceleration: ";
  ptr +=ay;
  ptr +="mss</p>";
  ptr +="<p style=text-align:left>Current z axis acceleration: ";
  ptr +=az;
  ptr +="mss</p>";
  ptr +="<p style=text-align:left>Current x axis rotation: ";
  ptr +=gx;
  ptr +="rs</p>";
  ptr +="<p style=text-align:left>Current y axis rotation: ";
  ptr +=gy;
  ptr +="rs</p>";
  ptr +="<p style=text-align:left>Current z axis rotation: ";
  ptr +=gz;
  ptr +="rs</p>";
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

void readsensors (void) {
//Timestamp for readings  
  timer = millis();
//Read stuff from the BMP sensor, store it in variables
  pressure = bmp.readPressPa()/100; // Read pressure once
  altitude = 44330.0 * (1.0 - pow(pressure / SEALEVELPRESSURE_HPA, 0.1903));
  correctedalt = altitude - altioffset;

//Read stuff from the accelerometer and gyro, store it in variables
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

//Calibration values need to be adjusted for each logger  
  ax = a.acceleration.x - 0.84;
  ay = a.acceleration.y + 0.17;
  az = a.acceleration.z - 10.67;
  gx = g.gyro.x + 0.09;
  gy = g.gyro.y - 0.03;
  gz = g.gyro.z - 0.03;

//liftoff check
  if (!liftoff && az >= 3){
     liftoff = true;
     liftofftime = millis();
     }

//acceleration peak recording
  if (liftoff && !droguefired && az > maxaccel){
     maxaccel = az;
     }     
}

void arming (void) {
//Run arming check. Altitude above ground must be above armcheck before the deployment check runs.
//Disables deployment charge for safety if altitude falls below armcheck.
  if (armed && !droguefired){
     droguedeploy();
     }
  else {
       if (correctedalt > armcheck && !droguefired) {
          armed = true;
          logfile.print("~ARMED~");
          }
       if (armed && droguefired) {
          maindeploy();
          }          
       if (correctedalt < armcheck && armed) {
          armed = false;
          droguecount = 0;
          maincount = 0;
          digitalWrite(droguepin, LOW);
          digitalWrite(mainpin, LOW);
          logfile.print("~DISABLED~");
          }
       }
}

void droguedeploy (void) {
//Run the deployment checks and fires deployment charge if condition is met. 
//Apogee is detected when there have been at least 3 consecutive reductions in altitude. 
  if (correctedalt <= alticheck && !droguefired) {
     droguecount = droguecount + 1;
     if (alticheck > maxalt) {
        maxalt = alticheck;
        apogeetime = millis();
        }
     alticheck = correctedalt;
     }
  else {
       droguecount = 0;
       alticheck = correctedalt;
       }
  if (droguecount > 2) {
     digitalWrite(mainpin, HIGH);
     droguefired = true;
     logfile.print("~DROGUE FIRED~");
     }
}

void maindeploy (void) {
  if (correctedalt <= mainalti && droguefired) {
     maincount = maincount + 1;
     }
  else {
       maincount = 0;
       }
  if (maincount > 2) {
     digitalWrite(mainpin, HIGH);
     mainfired = true;
     logfile.print("~MAIN FIRED~");
     }
}

void endlog (void) {
//footer for log file
  TTA = apogeetime - liftofftime;
  if (!EEPROMenabled){
     logfile.println();
     logfile.print("max altitude: "); COMMA;
     logfile.print(maxalt);
     logfile.print(" m, at: "); COMMA;
     logfile.println(apogeetime);
     logfile.print("liftoff: "); COMMA;
     logfile.print(liftofftime);
     logfile.println(" ms");
     logfile.print("Time to apogee:"); COMMA;
     logfile.print(TTA);
     logfile.println(" ms");
     logfile.println();
     logfile.print(" ~END OF LOG~");
     logfile.println();
     logfile.close();
     }

//Write some minimal data to EEPROM
  if (EEPROMenabled){
     Backup.save(maxalt, maxaccel, TTA); //saves 3 data points - can add a couple more with library modification.
     }
//Ending tone 
  while(1) {
        Buzz.ended();
        server.handleClient();
        }
}

void loop() {
  server.handleClient(); //handle incoming requests from wifi client. suspect this is causing intermittent spikes in the time between sensor logs every 150ms or so though.
  timer = millis();
  while (millis() - timer < logtime){} //pause here for a moment to allow the sensors enough time to get new readings
//Tone indicating logging is happening
  if (timer - buzzclock >= buzztime) {
     Buzz.running();
     buzzclock = timer;
     }
//run through the sensor reads and deployment checks
  readsensors();
  arming();

//log the readings to file
 if (!EEPROMenabled){
    logfile.println();
    logfile.print(timer); COMMA;
    logfile.print(pressure); COMMA;
    logfile.print(altitude); COMMA;
    logfile.print(correctedalt); COMMA;
    logfile.print(ax); COMMA;
    logfile.print(ay); COMMA;
    logfile.print(az); COMMA;
    logfile.print(gx); COMMA;
    logfile.print(gy); COMMA;
    logfile.print(gz); COMMA;
    }

//Serial readouts   
#ifdef datadebugging
    Debug.debugdata(timer, pressure, altitude, correctedalt, ax, ay, az, gx, gy, gz);
#endif

//logging timeout check
  if (droguefired && correctedalt < 10) {
     logtimeout = logtimeout + 1;
     }
  if (logtimeout >= 100) {
     endlog();
     } 

//sync in the SDfat library does the same thing as flush.
  if ((timer - flushclock >= flushtime)){
     logfile.sync();
     flushclock = timer;
     }
}

/* 22/03/2024 - this version is fairly complete and runs on an ESP32-C3 Super Mini based data logger. the HTML parts are pretty basic but are working.

Pin allocations are set. This version of code is intended for a small flight computer/logger for running simple rockets.
logging rate can go over 100hz now if logtime is set to 0, but unless oversampling and filtering is disabled for the bmp390, duplicate values will occur in the data.
The intention is to build a smaller but faster data logger than Logger v4.0.

To do:
-Finish EEPROM backup, probably will put it in a library, it's not that hard. (it's in a library now, but untested)
-Explore adding a 9-axis IMU in place of the MPU6050
-Serial debugging now added as a library!
*/
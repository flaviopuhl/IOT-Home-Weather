#define TIME_OFFSET 1UL * -10800UL // UTC + 0 hour

/***************************************************************************************
**                        OpenWeatherMap variable declaration
***************************************************************************************/
String latitude =  "-36.1111"; // 90.0000 to -90.0000 negative for Southern hemisphere
String longitude = "-56.1111"; // 180.000 to -180.000 negative for West
//const unsigned long postInterval = 5 * 60 * 1000;  // posting interval of 5 minutes  (1L * 1000L; 10 seconds delay for testing)

/***************************************************************************************
**                       JSON buffer dimension
***************************************************************************************/
#define JSON_BUFF_DIMENSION 2500


/***************************************************************************************
**                       Loops
***************************************************************************************/
unsigned long Loop1previousMillis = millis(); // Loop #1
const long Loop1interval = 1 * 60 * 1000;     // Loop #1 interval (seconds)

unsigned long Loop2previousMillis = millis(); // Loop #2
const long Loop2interval = 3000;              // Loop #2 interval (miliseconds)

unsigned long Loop3previousMillis = millis(); // Loop #3
const long Loop3interval = 1 * 60 * 1000;     // Loop #3 interval (seconds)

/***************************************************************************************
**                       Other
***************************************************************************************/
int masterResetCounter=0;

char charBuf[50];

/***************************************************************************************
**   Geolocation - Globally declaration to be available for display 
***************************************************************************************/
String city; 
float lat;
float lon;

/***************************************************************************************
**   OpenWeather - Globally declaration to be available for display 
***************************************************************************************/
//String currentTemp;
//String currentFeelsLike;
//String currentIcon;

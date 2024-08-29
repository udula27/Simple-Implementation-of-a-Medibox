#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WiFi.h>
#include <DHTesp.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>


#define screen_width  128
#define screen_height 64
#define oled_reset -1

#define screen_address 0x3c 


#define NTP_SERVER     "0.asia.pool.ntp.org"
#define UTC_OFFSET     19800
#define UTC_OFFSET_DST 0

// buzzer
#define buzzer 23

#define C 262
#define D 294
#define E 330
#define F 349
#define G 392
#define A 440
#define B 494
#define C_H 523


// buttons

#define ok 33
#define up 32
#define down 27
#define cancel 26
#define dhtpin 12

#define ldr_left 34
#define ldr_right 35

// time variables
char formattedTime[20];
int hours =0;int mins = 0;
int sec = 0;

int alarm_enable[3] = {0,0,0};
int alarmTimes[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
int stop_pressed[3]={0,0,0};
String strhours ="00";
String strmins = "00";
String strsec = "00";
const int utcOffsetsSize = 5;
const String utcOffsets[utcOffsetsSize] = {"+00:00", "+01:00", "+02:00", "+03:00", "+05:30"};


int menu_state = 0;
int substate = 1;
int alarmstate;
int currentOffsetIndex = 0;

Adafruit_SSD1306 display(screen_width, screen_height,&Wire,oled_reset);
DHTesp dhtSensor;

/////////////////////////////Assignment 02////////////////////
WiFiClient espClient;
PubSubClient mqttClient(espClient);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

char temp_array[6];
char ldr_arr_L[4];
char ldr_arr_R[4];

Servo servo;

int theta_off=30;
float gamma_i = 0.75;

/////////////////////////////////////////////////////////////

//==============================================================//
                /* general purpose functions  */
//==============================================================//


void oled(String text, int x=0, int y=0 ,int z=0)
{
  if (z==0)
  {
    display.clearDisplay();
  }
  
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(x,y);
  display.println(text);
  display.display();
}

void printLocalTime() 
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    oled("connection error");
    return;
  }
  strftime(formattedTime, sizeof(formattedTime), "%H:%M:%S", &timeinfo);



                                          /*ALARM 1*/
  //==========================================================================================================//



  if(timeinfo.tm_hour==alarmTimes[0] && timeinfo.tm_min==alarmTimes[1] && alarm_enable[0]==1 && !(stop_pressed[0]))
  {
    
    
    while(!(stop_pressed[0]))
    {
      ring_buzzer();
      oled("medicine time");
      delay(100);
      stop_pressed[0] = digitalRead(cancel);
    }
    oled("alarm off");
    delay(500);
  }
  else if (timeinfo.tm_hour==alarmTimes[0] && timeinfo.tm_min != alarmTimes[1] && alarm_enable[0]==1)
  {
    stop_pressed[0]= 0;
  }



                                         /*ALARM 2*/
  //==========================================================================================================//


  if(timeinfo.tm_hour==alarmTimes[3] && timeinfo.tm_min==alarmTimes[4]  && alarm_enable[1]==1 && !(stop_pressed[1]))
  {
    while(!(stop_pressed[1]))
    {
      ring_buzzer();
      oled("medicine time");
      delay(100);
      stop_pressed[1] = digitalRead(cancel);
    }
    oled("alarm off");
    delay(500);
  }
  else if (timeinfo.tm_hour==alarmTimes[3] && timeinfo.tm_min != alarmTimes[4] && alarm_enable[1]==1)
  {
    stop_pressed[1]= 0;
  }




                                         /*ALARM3*/
  //==========================================================================================================//

  if(timeinfo.tm_hour==alarmTimes[6] && timeinfo.tm_min==alarmTimes[7] && alarm_enable[2]==1 && !(stop_pressed[2]))
  {
    
    
    while(!(stop_pressed[2]))
    {
      ring_buzzer();
      oled("medicine time");
      delay(100);
      stop_pressed[2] = digitalRead(cancel);
    }
    oled("alarm off");
    delay(500);
  }
  else if (timeinfo.tm_hour==alarmTimes[6] && timeinfo.tm_min != alarmTimes[7] && alarm_enable[2]==1)
  {
    stop_pressed[2]= 0;
  }
}

void ring_buzzer()
{
  tone(buzzer,A);
  delay(500);
  noTone(2);
}

void set_alarm()
{
  int alarm_set = 0;
  oled(strhours + ":" + strmins + ":" + strsec);
  delay(100);
  while(1)
  {
    if(digitalRead(up)==HIGH)
    {
      hours = hours + 1;
      hours = hours % 24;
      strhours = hours < 10? "0"+ String(hours):String(hours);
      delay(100);
    }

    else if(digitalRead(down)==HIGH)
    {
      hours = hours -1;
      if( hours < 0)
      {
        hours = 0;
      }
      strhours = hours < 10? "0"+ String(hours):String(hours);
      delay(100);
    }
    else if(digitalRead(ok) == HIGH)
    {
      delay(200);
      while(1) 
      {
        if(digitalRead(up)==HIGH)
        {
          mins = mins + 1;
          mins = mins % 60;
          strmins = mins < 10? "0"+ String(mins):String(mins);
          delay(100);
        }
        else if(digitalRead(down)==HIGH)
        {
          mins = mins -1;
          if( mins <0)
          {
            mins = 0;
          }
          strmins = mins < 10? "0"+ String(mins):String(mins);
          delay(100);
        }
        else if(digitalRead(ok) == HIGH)
        {
          delay(200);
          while(1) 
          {
            if(digitalRead(up)==HIGH)
            {
              sec = sec +1;
              sec = sec % 60;
              strsec = sec < 10? "0"+ String(sec):String(sec);
              delay(100);
            }
            else if(digitalRead(down)==HIGH)
            {
              sec = sec -1;
              if( mins <0)
              {
                sec = 0;
              }
              strsec = sec < 10? "0"+ String(sec):String(sec);
              delay(100);
            }
            else if(digitalRead(ok) == HIGH)
            {
              alarm_set = 1;
              delay(100);
              break;
            }
            oled(strhours + ":" + strmins + ":" + strsec);
            oled("Set seconds",0,20,1);
          }
          break;
        }
        oled(strhours + ":" + strmins + ":" + strsec);
        oled("Set minutes",0,20,1);
      }
      break;
    }
    oled(strhours + ":" + strmins + ":" + strsec);
    oled("Set hours",0,20,1);
  }
  menu_state = 5;// exit
}

int utcOffsetToSeconds(const String& utcOffset) 
{

    int hours, minutes;
    char sign;
    sscanf(utcOffset.c_str(), "%c%d:%d", &sign, &hours, &minutes);

    // Convert hours and minutes to seconds
    int totalSeconds = hours * 3600 + minutes * 60;

    // Adjust sign
    if (sign == '-')
    {

    
        totalSeconds *= -1;
    }

    return totalSeconds;
}

void check_temp()
{
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  ///////////////////////
  String(data.temperature, 2).toCharArray(temp_array, 6);
  //////////////////////
  if(data.temperature>35)
  {
    oled("high temperature",0,20,1);
  }else if(data.temperature<15)
  {
    oled("low temperature",0,20,1);
  }
  if(data.humidity>50)
  {
    oled("high humidity",0,30,1);
  }else if(data.humidity<20)
  {
    oled("low humidity",0,30,1);
  }
}


//=============================================================//
                  /* menue functions */
//============================================================//


void disable_alarm()
{
  oled("Disable alarm");
  while(1) 
  {  oled("Disable alarm");
    if (digitalRead(up)==HIGH)
    { 
      delay(100);
      menu_state = 5;
      return;  
    }
    else if (digitalRead(down)==HIGH)
    {
      delay(100);
      menu_state = 2; 
      return;
    }else if (menu_state == 6)
    {
      break;
    }

    if (digitalRead(ok) == HIGH)
    {
      delay(100);
      alarmstate = 1;
      substate = 1;
      
      
      while(1)
      {
        if (substate == 3)
        {
          delay(100);
          oled("Alarm three");
          dalarm3();
          
          
        }
        else if (substate == 2)
        {
          delay(100);
          oled("Alarm two");
          dalarm2();
          
        }
       
        else if(substate == 1)
        {
          delay(100);
          oled("Alarm one"); 
          dalarm1();
          
        
        }else if(alarmstate == 0)
        {
          delay(100);
          menu_state = 6;
          break;
        }
    
      }
      alarm_enable[0] = 0;
    }

  }
}

void set_alarm_1()
{
  oled("set alarm 1");
  while(1)
  {
    if (digitalRead(up)==HIGH)
    { 
      delay(100);
      menu_state = 1;
      return;  
    }
    else if (digitalRead(down)==HIGH)
    {
      delay(100);
      menu_state = 3; 
      return;
    }
    if (digitalRead(ok) == HIGH)
      {
        delay(100);
        set_alarm();
        delay(100);
        alarm_enable[0] =1;
        alarmTimes[0] = hours;
        alarmTimes[1] = mins;
        alarmTimes[2] = sec;
        menu_state = 6;

        //todo

        break;
      }
  }
  

}

void set_alarm_2() 
{
  oled("set alarm 2");
  
  while(1)
  {
    if (digitalRead(up)==HIGH)
    { 
      delay(100);
      menu_state = 2;
      return;  
    }
    else if (digitalRead(down)==HIGH)
    {
      delay(100);
      menu_state = 4; 
      return;
    }
    if (digitalRead(ok) == HIGH)
      {
        delay(100);
        set_alarm();
        alarm_enable[1] =1;
        delay(100);
        // todo
        
        alarmTimes[3] = hours;
        alarmTimes[4] = mins;
        alarmTimes[5] = sec;
        menu_state = 6;
        break;
      }
  }
  
}
void set_alarm_3() 
{
  oled("set alarm 3");
  
  while(1)
  {
    if (digitalRead(up)==HIGH)
    { 
      delay(100);
      menu_state = 3;
      return;  
    }
    else if (digitalRead(down)==HIGH)
    {
      delay(100);
      menu_state = 5; 
      return;
    }
    if (digitalRead(ok) == HIGH)
      {
        delay(100);
        set_alarm();
        alarm_enable[2] =1;
        delay(100);
        // todo
        menu_state = 6;
        alarmTimes[6] = hours;
        alarmTimes[7] = mins;
        alarmTimes[8] = sec;
        break;
      }
  }
  
}


void set_current_time()
{
  oled("set current time");
  while(1)
  {
    if (digitalRead(up)==HIGH)
    { 
      delay(100);
      menu_state = 4;
      return;  
    }
    else if (digitalRead(down)==HIGH)
    {
      delay(100);
      menu_state = 1; 
      return;
    }
    if (digitalRead(ok) == HIGH)
      {
        
       
        delay(100);
        while(1)
        {
            
            oled(utcOffsets[currentOffsetIndex]);
            
            
            if (digitalRead(up) == HIGH) {
              currentOffsetIndex = (currentOffsetIndex + 1) % utcOffsetsSize;
              delay(200); // Button debounce
            }
            
            if (digitalRead(down) == HIGH) {
              currentOffsetIndex = (currentOffsetIndex - 1 + utcOffsetsSize) % utcOffsetsSize;
              delay(200); // Button debounce
            }
            
            if (digitalRead(ok) == HIGH) {
              String selectedOffset = utcOffsets[currentOffsetIndex];
              int offsetInSeconds = utcOffsetToSeconds(selectedOffset);
              configTime(offsetInSeconds, UTC_OFFSET_DST, NTP_SERVER);
              delay(1000); // Delay to avoid multiple button presses
              break;
            }
          
        }
        menu_state = 6;
        break;
      }
    if (digitalRead(cancel) == HIGH)
      {
        
       
        delay(100);
        menu_state = 6;
        break;
      }
  }
}

//========================================================//
                /* disable alarm substates */
//========================================================//

void dalarm1()
{
  while(1)
  {
    if(digitalRead(ok)==HIGH)
    {
      delay(100);
      alarm_enable[0] = 0;
      oled("alarm disabled");
      substate = 7;//dne
      alarmstate = 0;
      break;
    }
    else if(digitalRead(up)==HIGH)
    {
      delay(100);
      substate = 3;
      break;
    }
    else if(digitalRead(down)==HIGH)
    {
      delay(100);
      substate = 2;
      break; 
    }
  }

}

void dalarm2()
{
  while(1)
  {
    if(digitalRead(ok)==HIGH)
    {
      delay(100);
      alarm_enable[1] = 0;
      oled("alarm disabled");
      substate = 7;//dne
      alarmstate = 0;

      break;
    }
    else if(digitalRead(up)==HIGH)
    {
      delay(100);
      substate = 1;
      break;
    }
    else if(digitalRead(down)==HIGH)
    {
      delay(100);
      substate = 3;
      break; 
    }
  }
}

void dalarm3()
{
  while(1)
  {
    if(digitalRead(ok)==HIGH)
    {
      delay(100);
      alarm_enable[2] = 0;
      oled("alarm disabled");
      substate = 7;//dne
      alarmstate = 0;

      break;
    }
    else if(digitalRead(up)==HIGH)
    {
      delay(100);
      substate = 2;
      break;
    }
    else if(digitalRead(down)==HIGH)
    {
      delay(100);
      substate = 1;
      break; 
    }
  }
}



void menue()
{
  int state = 1;
  menu_state =1;
  oled("Disable alarm");
  while(1)
  {
    if(menu_state == 5)
    {
      delay(100);
      
      set_current_time();
      delay(100);
    }
    else if( menu_state == 2)
    {
      delay(100);
      
      set_alarm_1();   
      delay(100);
    }
    else if(menu_state == 1)
    {
      delay(100);
      
      disable_alarm();
      delay(100);
    }
    else if(menu_state == 3)
    {
      delay(100);
      
      set_alarm_2();
      delay(100);
    }
  
    else if(menu_state == 4)
    {
      delay(100);
      
      set_alarm_3();
      //set_current_time();
      delay(100);
    
    
    }
    else if(digitalRead(cancel)==HIGH || menu_state == 6)
    {
      delay(100);
      break;
    }
  }
}

void setup() 
{
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, screen_address);
  delay(2000);
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(250);
    oled("connecting to WiFi");
  }
  oled("connected");
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
  // buzzer
  pinMode(buzzer, OUTPUT);
  //buttons
  pinMode(ok,INPUT);
  pinMode(up,INPUT);
  pinMode(down,INPUT);
  pinMode(cancel,INPUT);

  //ldr
  pinMode(ldr_left,INPUT);
  pinMode(ldr_right,INPUT);

  dhtSensor.setup(dhtpin,DHTesp::DHT22);

  /////////////////////
  setup_mqtt();

  timeClient.begin();
  timeClient.setTimeOffset(5.5 * 3600);

  servo.attach(14);

  /////////////////////
}
void setup_mqtt() {
  mqttClient.setServer("test.mosquitto.org", 1883);
  mqttClient.setCallback(recieveCallback);
}

void connectTOBroker() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("MQTT Connected");
      mqttClient.subscribe("UD-ON-OFF");
      //mqttClient.subscribe("UD-ADMIN-SCH-ON");
      mqttClient.subscribe("UD-SERVO-_MIN");
      mqttClient.subscribe("UD-SERVO_CF");
    } else {
      Serial.print("Failed To connect to MQTT Broker");
      Serial.print(mqttClient.state());
      delay(5000);
    }
  }
}

void recieveCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char payload_arr[length];
  Serial.print("Message Recieved: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    payload_arr[i] = (char)payload[i];
  }
  Serial.println();
  if (strcmp(topic, "UD-ON-OFF") == 0) {
    if (payload_arr[0] == '1') {
      digitalWrite(13, HIGH);
    } else {
      digitalWrite(13, LOW);
    }
  } 
  else if (strcmp(topic, "UD-SERVO-MIN") == 0) {
    theta_off = String(payload_arr).toInt(); 

  } 
  else if (strcmp(topic, "UD-SERVO_CF") == 0) {
    gamma_i = String(payload_arr).toFloat();
  }
}

void update_light() {

  

  float lsv = analogRead(ldr_left) * 1.00;
  float rsv = analogRead(ldr_right) * 1.00;

  float lsv_cha = (float)(lsv - 1016.00) / (8.00 - 1016.00);
  
  float rsv_cha = (float)(rsv - 1016.00) / (8.00 - 1016.00);
  

  update_angle(lsv_cha, rsv_cha);

  String(lsv_cha).toCharArray(ldr_arr_L, 4);
  String(rsv_cha).toCharArray(ldr_arr_R, 4);
}

void update_angle(float lsv, float rsv) {
  float max_intensity = lsv;
  float d = 1.5;

  if (rsv > max_intensity) {
    max_intensity = rsv;
    d = 0.5;
  }

  int theta = theta_off * d + (180 - theta_off) * max_intensity * gamma_i;
  theta = min(theta, 180);

  servo.write(theta);
}





void loop() {
  // put your main code here, to run repeatedly:
  //////////////////////////////
  if (!mqttClient.connected()) {
    Serial.println("Reconnecting to MQTT Broker");
    connectTOBroker();
  }

  mqttClient.loop();

  // updateTemp();
  // Serial.println(tempArr);
  mqttClient.publish("UD-TEMP", temp_array);
  
  delay(50);

  // delay(50);

  update_light();

  

  mqttClient.publish("UD-LIGHT-L", ldr_arr_L);
  delay(50);
  mqttClient.publish("UD-LIGHT-R", ldr_arr_R);
  delay(100);
  /////////////////////////////


  printLocalTime();
  check_temp();
  
  
  oled(formattedTime) ;
  if (digitalRead(ok)== HIGH)
  {
    menue();
    delay(50);
  }
  
}

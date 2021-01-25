// M5Stack 6060PUSH example for M5Stack Core2 and M5Stack Core by Martin Glass (Jan-2021)
// Valid for M5Stack Core2, but can be amended for M5Stack Core

// This code demonstrates the use of Serial2 to communicate with the 6060PUSH.

// It also demonstrates use of some of the LCD display routines (see documentation here: http://docs.m5stack.com/#/en/api/lcd )

// The 6060PUSH instruction Set is limited (GRBL)...
// Check ID - ID
// Set Device ID - ID=???
// Move - ID???:X???.?
// Read Position - ID???P
// Find Zero Position - ID???Z
// Set Feed Speed - ID???:F????
// Read Busy ? - ID???I


/*
 * Copyright © 2021 <M. Glass>
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This software consists of voluntary contributions made by many individuals
 * and is licensed under the MIT license.
*/

/*
* Notes on the PUSH6060 setup
* ---------------------------
* 
* The 6060PUSH has a calibration issue (at least on mine). 
* A command of X40 moves the Axis by 50mm (instead of 40mm) 
* and the full extent of 57mm is achieved with a command of X45!!!
* 
* The 6060Push takes its power from a 9V-24V supply (5mm socket).
* The same power (at 9V-24V) is connected to the orange HT3.96 RS485 sockets on the 6060Push
* 
* To connect between the Core2/Core and the 6060Push the recommendation is to use the PLC Proto Module.
* 
* The PLC Proto Module has a small RS485-to-TTL ancillary PCB board included, 
* which converts the 9V-24V power supply and serial communication signal lines for the 6060Push to 5V/3.3V for the Core2/Core.
* There are no instructions nor schematics for this small ancillary PCB board, but it is fairly obvious how it is supposed to be wired in 
* (it's on the corner of the PCB, between the M-Bus and the RS485 connector).
* But this won't work with the Core2, as the ancillary PCB board doesn'tfit with the Core2. 
* 
* First, the haptic motor in the Core2 obstructs the ancilliary board. 
* Secondly, because the motor has a metal case, there is a risk of shorting the 9V-24V power line with M-Bus pins 16 and 17.
* 
* In order to use the PLC Proto Mobule, you need to connect the ancillary board by wire, and keep it away from the M-Bus connector.
* 
* As mentioned, you'll need to wire up pins 16 and 17 on the PLC Proto Module ancillary board to the Core2 M-bus.
* You'll also need to wire that connects GND between the ancillary board and the Core2 (you can use the M-Bus GND pin or any other GND pin).
* 
* IMPORTANTLY, the ancillary board provides 9V-24V to the HPWR connectors on the MBus (pins 25,27, 29), 
* which on a Core these pins will provide 9V-24V power to the Core, but not on the Core2!! 
* The HWPWR pins are not connected in the Core2, so you need to have a power wire going from thePLC Proto Module 5V supply to the 5V input on the Core2. 
* This isn't obvious to do. 
* 
* There are several places to connect to the 5V supply on the PLC Proto module. But you need to wire into the 5V input pin inside the Core2, 
* which is one of the pins on the 4-pin connector inside the Core2, next to the USB connector. the pins are marked DP,DM,GND and 5V. 
* You want the 5V pin (and connect that by a wire to a 5V supply pin on the PLC Proto module board.
* 
* It may be better just to get the M5Stack RS485-to-TTL converter Unit instead of the PLC Proto Module !!!
* https://m5stack.com/collections/m5-unit/products/isolated-rs485-unit
* 
* The other issue is that the 6060Push doesn't come with a HT3.96 4-pin plug, to connect into the RS485 socket on the 6060Push.
* Unfortunately, the PLC Proto Module (and the RS485 Unit) only comes with 1 x HT3.96 4-pin connector, not two.
* You'll need to source one HR3.96 plug to go onto the 6060Push RS485 HT3.96 socket and they're not easy to find. 
* Here's an Aliexpress link here: https://www.aliexpress.com/item/32751855312.html
* 
*/

#include <M5Core2.h> //for M5Stack Core2 only
//#include <M5Stack.h> //uncomment for M5Stack Core
#define RX_PIN   13   // M5Stack Core is 16, M5Stack Core2 is 13
#define TX_PIN   14   // M5Stack Core is 17, M5Stack Core2 is 14

#define X_LOCAL 40
#define Y_LOCAL 40

#define X_OFF 160
#define Y_OFF 30

float distance = 0;
int limit = 45; // 45 is the absolute X-command limit, which equates to 57 mm of movement, because a command of X45 gives 50mm movement, due to mis-calibration by the 6060PUSH

int decimal = 0; // post decimal place variable
int incr = 10000; //Movement increment in microns (default is 10mm)
String incText;

  void head(const char *string) {
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_MAGENTA, TFT_BLUE);
    M5.Lcd.fillRect(0, 0, 320, 30, TFT_BLUE);
    M5.Lcd.setTextDatum(TC_DATUM); // Align Text = Top Centre
    M5.Lcd.drawString(string, 160, 3, 4);
  }

  void foot(const char *string, int column, int row) {
      M5.Lcd.setTextColor(YELLOW);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setCursor(column, row);
      M5.Lcd.println(string);
  }


  void subtext() {                      // set and display the increment/delta value
      switch (incr) {
      case 10000:                          
        incText=" 10mm";
        break;
      case 5000: 
        incText="  5mm";
        break;
      case 2000:
        incText="  2mm";
        break;
      case 1000: 
        incText="  1mm";
        break;
      case 100: 
        incText="0.1mm";
        break;
      default: 
        incText="error";
      }
      M5.Lcd.setTextColor(YELLOW);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setCursor(10, 205);
      M5.Lcd.println(incText);

  }
  

  void screenRefresh() {                      // refresh screen and display all variables
      M5.Lcd.fillScreen(TFT_BLACK);
      head("PUSH6060");
      foot(" Incr",10,180);
      foot("Move Up",130,205);
      foot("Home",230,205);
      subtext();
      M5.Lcd.progressBar(110,80,200,20,distance*100/limit); // display a progress bar
      xPosPrint(distance); // display the X position
  }


//  void xPosPrint(int x) {               // display the X-position (distance)
  void xPosPrint(float x) {               // display the X-position (distance)
    M5.Lcd.setCursor(110,115);
//    M5.Lcd.printf("X: %d\r\n",x);
    M5.Lcd.printf("X: %.3f\r\n",x);
    M5.Lcd.setCursor(110,140);
    M5.Lcd.printf("%.3f mm\r\n",x*57.0/float(limit));
    
  }


void setup() {
    M5.begin(true, true, true, false);    // start M5Stack Core2
    //M5.begin(); // start M5Stack Core
    //M5.Power.begin(); / M5Stack Core only (not Core2)

    Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);  //Configure serial port 2
    delay(500);
    Serial2.print("ID=123\r\n");                     //Serial port 2 output ID=123\r\n, configuration 6060 motor ID is 123
    
    //Simple start-up test
    Serial2.print("ID123Z\r\n");
    distance=0;
    screenRefresh();
    delay(500);
    distance = float(incr/1000.0);
    Serial2.printf("ID123:X%.3f\r\n",distance);
//    Serial2.printf("ID123:X%d\r\n",distance);
    screenRefresh();
    //
}


void loop() {
    if(M5.BtnA.wasPressed()){           // Btn A changes the increment/delta
      switch (incr) {
      case 10000:                          
        incr = 100;
        break;
      case 5000: 
        incr = 10000;
        break;
      case 2000:
        incr = 5000;
        break;
      case 1000: 
        incr = 2000;
        break;
      case 100: 
        incr = 1000;
        break;
      default: 
        incr=1000; // shouldn't ever get here
      }
  screenRefresh();
    }
    
    if(M5.BtnB.wasPressed()){           // Btn B moves the travel by the increment/delta, by sending ID123:Xxx\r\n 
      if((distance+float(incr/1000.0)) <= 45.0){         // 45 is the absolute limit (which is 57 mm of movement, because a command of X45 gives 50mm movement, due to mis-calibration by the 6060PUSH manufacturer 
         distance += float(incr/1000.0);
//         Serial2.printf("ID123:X%d\r\n",distance);
         Serial2.printf("ID123:X%.3f\r\n",distance);
         screenRefresh();
      }
      else {
        head("TOO FAR");
      }
    }
    
    if(M5.BtnC.wasPressed()){                      //Press C to send ID123Z\r\n and the motor returns to the origin.
      distance = 0; //MG added, needed to reset distance after homing
      screenRefresh();
      Serial2.print("ID123Z\r\n");
      screenRefresh();
    }
    if(Serial2.available()){                       //Serial port 2 receives the message returned by 6060 and prints
      char c = Serial2.read();
      Serial.print(c);
    }

    M5.update();
    }

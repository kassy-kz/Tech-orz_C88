#include <Bridge.h>
#include <Parse.h>

bool returnFlag = false;
bool switchFlag = false;
long timerCount = 0;

void sendTweet() {
  Process process; 

  // Send the curl command:
  Serial.print("Sending tweet...: ");
  Serial.println("iftttCurl.sh"); 
  process.runShellCommand("iftttCurl.sh"); 

  // Read out the response:
  Serial.print("Response: ");
  // Use the process process to read in any response from Linux:
  while (process.available()) {
    char c = process.read();
    Serial.write(c);
  }

}

void sendPush() {
  Process process; 

  // Send the curl command:
  Serial.print("Sending push...: ");
  Serial.println("kassyCuro.sh"); // Print command for debug
  process.runShellCommand("kassyCurl.sh"); // Send command through Shell

  // Read out the response:
  Serial.print("Response: ");
  // Use the process process to read in any response from Linux:
  while (process.available()) {
    char c = process.read();
    Serial.write(c);
  }
}


void setup() {
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(13, OUTPUT);
   
  Bridge.begin();
  Serial.begin(9600);
   
  while (!Serial); // wait for a serial connection
  Serial.println("Parse Starter Project");
   
  Parse.begin("YOUR_APP_ID", "YOUR_API_KEY");

  // Start push service
  Parse.startPushService();
  Serial.print("Push Installation ID:");
  Serial.println(Parse.getInstallationId());

  // tmp code
  //sendTweet();
}

void loop() {
  // ポストの開閉を検知してAndroid端末にプッシュ通知投げる処理
  if(digitalRead(12)) {
    sendPush();
    Serial.println("send push data");
    delay(1000);
  }

  // Push待ち受け
  if (Parse.pushAvailable()) {
    ParsePush push = Parse.nextPush();
   
    String message = push.getJSONBody();
    Serial.print("New push message size: ");
    Serial.println(message.length());
    Serial.print("New push message content: ");
    Serial.println(message);
 
    String command = push.getString("alert");
    if (command == "return home") {
      returnFlag = true;
      timerCount = 0;
    }    
    push.close();
  }

  // ポスト確認スイッチ検知
  if(digitalRead(11)) {
    switchFlag = true;
  }

  // 帰宅している
  if(returnFlag) {
    // スイッチが押されたら抜ける
    if(switchFlag) {
      returnFlag = false;
      switchFlag = false;
    }
    // まだスイッチが押されていない
    else {
      // 1secまつ
      delay(1000);
      timerCount+=1000;
      // 30分経過したらツイート
      if(timerCount > 180000) {
        Serial.println("send Tweet");
        //sendTweet();
        returnFlag = false;
        switchFlag = false;
      }
    }            
  }
  
}

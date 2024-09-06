#include <HTTP_Method.h>
#include <WebServer.h>
#include <SPI.h>
#include <Ticker.h>
#include<WiFi.h>
#include <dummy.h>
#include <time.h>

//数据采集超参数
#define LED2 2
#define DATACOUNT 144
#define VOUTSTART 1636
#define VOUTEND 6552
#define SAMPLE 16
#define AUTOTIMES 50

////巴法云连接超参数
#define TCP_SERVER_ADDR "bemfa.com"
#define TCP_SERVER_PORT 8344
String TCP_UID = "19001126c7e745ff8ceee3da2f6ccebc";
String topic = "smell";

/*vars for wifi connection and server*/
const char *ssid = "HONOR 30S";
const char *password = "lyl33333333";
IPAddress staticIP(192,168,43,115);
IPAddress gateway(192,168,43,1);
IPAddress subnet(255,255,255,0);
WiFiServer server(80);

String responseHeaders =
    String("") +
    "HTTP/1.1 200 OK\r\n" +
    "Content-Type: text/html\r\n" +
    "Connection: keep-alive\r\n" +
    "\r\n";

//String wxHeaders = String("") + "Connection: keep-alive\r\n";

String myhtmlPage = String("") +
    "<!DOCTYPE html>\n" +                                                                                                                                                                                                                                    
    "<html>\n" +                                                                                                                                                                                                                                             
    "<head>\n" +                                                                                                                                                                                                                                             
    "  <style type=\"text/css\">\n" +                                                                                                                                                                                                                         
    " .container{\n" +                                                                                                                                                                                                                                       
    "   margin:0 auto;\n" +                                                                                                                                                                                                                                  
    "   width:1000px;\n" +                                                                                                                                                                                                                                   
    " }\n" +                                                                                                                                                                                                                                                 
    "  \n" +                                                                                                                                                                                                                                                 
    "  </style>\n" +                                                                                                                                                                                                                                         
    "          \n" +                                                                                                                                                                                                                                         
    " <meta charset=\"UTF-8\"></meta><title>Convolutional neural network facilitates gases discrimination for temperature-modulated MOS gas sensor</title>\n" +
    "</head>\n" +                                                                                   
    " <body>\n" +                                                                                                                                                                                                                                            
    "   <h2 align=\"center\">Convolutional neural network facilitates gases discrimination </h2>\n" +                                                                                         
    "   &nbsp;&nbsp;&nbsp;&nbsp;We propose to use CNN for feature extraction and pattern recognition, which effectively improves the recognition accuracy of the sensor and realizes the classification and identification of BTX gas by a single sensor.\n R=10000*(409500-data)/data" +
    "   <br></br>\n" +                                                                                                                                                                                                                                        
    "   <form method=\"post\"action=\"Start\">\n" +                                                                                                                                                                                                           
    "     <table align=\"center\"><tr><td> </td><td>\n" +                                                                                                                                                                                                      
    "       <button name=\"Start\"type=\"submit\"value=\"0\"id=\"0\">Start</button>\n" +                                                                                                                                                           
    "     </td></tr>\n" +                                                                                                                                                                                                                                      
    "      </table>\n" +                                                                                                                                                                                                                                      
    "   </form>\n" +                                                                                                                                                                                                                                          
    "     <table align=\"center\"><tr>\n" +                                                                                                                                                                                                                    
    "     <td><form method=\"post\"action=\"Sensor1\"><button name=\"sensor1\"type=\"submit\"value=\"1\"id=\"1\" style=\"width:140px;height:20px\">NO.1 sensor data</button></form></td>\n" +                                                                                                            
    "     <td><form method=\"post\"action=\"Sensor2\"><button name=\"sensor2\"type=\"submit\"value=\"2\"id=\"2\" style=\"width:140px;height:20px\">NO.2 sensor data</button></form></td>\n" +                                                                                                            
    "     <td><form method=\"post\"action=\"Sensor3\"><button name=\"sensor3\"type=\"submit\"value=\"3\"id=\"3\" style=\"width:140px;height:20px\">NO.3 sensor data</button></form></td>\n" +                                                                                                            
    "     <td><form method=\"post\"action=\"Sensor4\"><button name=\"sensor4\"type=\"submit\"value=\"4\"id=\"4\" style=\"width:140px;height:20px\">NO.4 sensor data</button></form></td>\n" +                                                                                                            
    "   </tr></table>\n" +   
    "<table align=\"center\">\n" +
    "    <tr>\n" +
    "        <td><form method=\"post\"action=\"autoMode\"><button name=\"autoMode\"type=\"submit\"value=\"5\"id=\"5\" style=\"width:140px;height:20px\">auto sensor</button></form></td>\n" +
    "    </tr>\n" + 
    "</table>\n" +                                                                                                                                                                                                                              
    "   <div class=\"container\">\n" +                                                                                                                                                                                                                        
    "     <canvas id=\"drawing\" width=\"1000\" height=\"600\" style=\"border:0px solid #000000;\">A drawing of something.</canvas>\n" +                                                                                                                       
    "   </div>\n" +  
    "<table align=\"center\">\n" +
    "<tr><div style=\"text-align: center;color: red\" id='count_div'>counting....</div></tr>\n" +
    "<tr><td>\n" +
    "   <button onclick = \"erase_data()\">clear_data</button>\n" +
    "</td></tr>\n" +
    "</table>\n" +
    "<table align=\"center\">\n" +
    "<tr>\n" +
    "     <td><button onclick=\"save_data(1)\">save sensor1 data</button></td>\n" +
    "     <td><button onclick=\"save_data(2)\">save sensor2 data</button></td>\n" +
    "     <td><button onclick=\"save_data(3)\">save sensor3 data</button></td>\n" +
    "     <td><button onclick=\"save_data(4)\">save sensor4 data</button></td>\n" +
    "</tr></table>\n" +                                                                                                                                                                                                                                          
    "   <script>\n" +                                                                                                                                                                                                                                         
    "     var LineChart = function() {\n" +                                                                                                                                                                                                                    
    "       this.ctx = drawing.getContext(\"2d\");\n" +                                                                                                                                                                                                         
    "       this.cWidth = this.ctx.canvas.width;\n" +                                                                                                                                                                                                           
    "       this.cHeight = this.ctx.canvas.height;\n" +                                                                                                                                                                                                         
    "       this.axisWidth = 2;\n" +                                                                                                                                                                                                                            
    "       this.gridWidth = 1;\n" +                                                                                                                                                                                                                            
    "       this.plotWidth = 0.5;\n" +                                                                                                                                                                                                                          
    "       this.widthMargin = 80;    // make sure widthMargin and height Margin are bigger than arrowWidth, or part of the arrow will go off the canvas\n" +                                                                                                   
    "       this.heightMargin = 40;\n" +                                                                                                                                                                                                                        
    "       this.originWidth = this.widthMargin;\n" +                                                                                                                                                                                                           
    "       this.originHeight = this.cHeight - this.heightMargin;\n" +                                                                                                                                                                                          
    "       this.arrowWidth = 15;\n" +                                                                                                                                                                                                                          
    "       this.arrowHeight = this.arrowWidth * Math.tan(20 * 2*Math.PI/360);\n" +                                                                                                                                                                             
    "       this.YgridMargin = (this.originHeight - this.heightMargin - this.arrowWidth) / 10;\n" +                                                                                                                                                             
    "       this.XgridMargin = (this.cWidth - this.originWidth - this.widthMargin - this.arrowWidth) / 1;\n" +                                                                                                                                                  
    "       this.deltaY = 20000;\n" +                                                                                                                                                                                                                          
    "       this.deltaX = 20;\n" +                                                                                                                                                                                                                              
    "     }\n" +                                                                                                                                                                                                                                               
    "     LineChart.prototype.init = function() {\n" +                                                                                                                                                                                                         
    "       this.clearCanvas();\n" +                                                                                                                                                                                                                            
    "       this.drawGrid();\n" +                                                                                                                                                                                                                               
    "       this.drawAxis();\n" +                                                                                                                                                                                                                               
    "     }\n" +                                                                                                                                                                                                                                               
    "     LineChart.prototype.clearCanvas = function() {\n" +                                                                                                                                                                                                  
    "       this.ctx.clearRect(0,0,this.cWidth,this.cHeight);\n" +                                                                                                                                                                                              
    "     }\n" +                                                                                                                                                                                                                                               
    "     LineChart.prototype.drawAxis = function() {\n" +                                                                                                                                                                                                     
    "     /* x axis */\n" +                                                                                                                                                                                                                                    
    "       this.ctx.beginPath();\n" +                                                                                                                                                                                                                          
    "       this.ctx.strokeStyle = \"black\";\n" +                                                                                                                                                                                                              
    "       this.ctx.lineWidth = this.axisWidth;\n" +                                                                                                                                                                                                           
    "       // draw line\n" +                                                                                                                                                                                                                                   
    "       this.ctx.moveTo(this.originWidth,this.originHeight);\n" +                                                                                                                                                                                           
    "       this.ctx.lineTo((this.cWidth - this.widthMargin),this.originHeight);\n" +                                                                                                                                                                           
    "       // draw arrow\n" +                                                                                                                                                                                                                                  
    "       this.ctx.lineTo((this.cWidth - this.widthMargin - this.arrowWidth),(this.originHeight - this.arrowHeight));\n" +                                                                                                                                    
    "       this.ctx.moveTo((this.cWidth - this.widthMargin),this.originHeight);\n" +                                                                                                                                                                           
    "       this.ctx.lineTo((this.cWidth - this.widthMargin - this.arrowWidth),(this.originHeight + this.arrowHeight));\n" +                                                                                                                                    
    "       this.ctx.stroke();\n" +                                                                                                                                                                                                                             
    "     /* y axis */\n" +                                                                                                                                                                                                                                    
    "       this.ctx.beginPath();\n" +                                                                                                                                                                                                                          
    "       this.ctx.strokeStyle = \"black\";\n" +                                                                                                                                                                                                              
    "       this.ctx.lineWidth = this.axisWidth;\n" +                                                                                                                                                                                                           
    "                                        \n" +                                                                                                                                                                                                           
    "       this.ctx.moveTo(this.originWidth,this.originHeight);\n" +                                                                                                                                                                                           
    "       this.ctx.lineTo(this.originWidth,this.heightMargin);\n" +                                                                                                                                                                                           
    "                                                        \n" +                                                                                                                                                                                           
    "       this.ctx.lineTo((this.originWidth - this.arrowHeight), (this.heightMargin + this.arrowWidth));\n" +                                                                                                                                                 
    "       this.ctx.moveTo(this.originWidth,this.heightMargin);\n" +                                                                                                                                                                                           
    "       this.ctx.lineTo((this.originWidth + this.arrowHeight), (this.heightMargin + this.arrowWidth));\n" +                                                                                                                                                 
    "       this.ctx.stroke();\n" +                                                                                                                                                                                                                             
    "     }\n" +                                                                                                                                                                                                                                               
    "     LineChart.prototype.drawGrid = function() {\n" +                                                                                                                                                                                                     
    "     /* y grid */\n" +                                                                                                                                                                                                                                    
    "       this.ctx.beginPath();\n" +                                                                                                                                                                                                                          
    "       this.ctx.strokeStyle = \"#ddd\";\n" +                                                                                                                                                                                                               
    "       this.ctx.lineWidth = this.gridWidth;\n" +                                                                                                                                                                                                           
    "       this.ctx.font = \"15px Arial\";\n" +                                                                                                                                                                                                                
    "                                   \n" +                                                                                                                                                                                                                
    "       var i = 1;\n" +                                                                                                                                                                                                                                     
    "       var ytick = 0;\n" +                                                                                                                                                                                                                                 
    "       while((this.originHeight - i * this.YgridMargin) >= (this.arrowWidth + this.heightMargin)) {\n" +                                                                                                                                                   
    "       this.ctx.moveTo(this.originWidth,(this.originHeight - i * this.YgridMargin));\n" +                                                                                                                                                                  
    "       this.ctx.lineTo((this.cWidth - this.arrowWidth - this.widthMargin),(this.originHeight - i * this.YgridMargin));\n" +                                                                                                                                
    "       ytick = i * this.deltaY;\n" +                                                                                                                                                                                                                       
    "       // ytick = parseFloat(ytick).toFixed(1);\n" +                                                                                                                                                                                                       
    "       this.ctx.fillText(ytick,0,(this.originHeight - i * this.YgridMargin + 7.5));\n" +                                                                                                                                                                   
    "       i++;\n" +                                                                                                                                                                                                                                           
    "       }\n" +                                                                                                                                                                                                                                              
    "       this.ctx.stroke();\n" +                                                                                                                                                                                                                             
    "     /* x grid */\n" +                                                                                                                                                                                                                                    
    "       this.ctx.beginPath();\n" +                                                                                                                                                                                                                          
    "       this.ctx.strokeStyle = \"#ddd\";\n" +                                                                                                                                                                                                               
    "       this.ctx.lineWidth = this.gridWidth;\n" +                                                                                                                                                                                                           
    "       this.ctx.font = \"15px Arial\";\n" +                                                                                                                                                                                                                
    "                                   \n" +                                                                                                                                                                                                                
    "       i = 0;\n" +                                                                                                                                                                                                                                         
    "       var xtick = 0;\n" +                                                                                                                                                                                                                                 
    "       while((this.originWidth + i * this.XgridMargin) <= (this.cWidth - this.widthMargin - this.arrowWidth)) {\n" +                                                                                                                                       
    "       this.ctx.moveTo((this.originWidth + i * this.XgridMargin),this.originHeight);\n" +                                                                                                                                                                  
    "       this.ctx.lineTo((this.originWidth + i * this.XgridMargin),(this.arrowWidth + this.heightMargin));\n" +                                                                                                                                              
    "       xtick = i * this.deltaX;\n" +                                                                                                                                                                                                                       
    "       // xtick = parseFloat(xtick).toFixed(0);\n" +                                                                                                                                                                                                       
    "       // this.ctx.fillText(xtick,(this.originWidth + i * this.XgridMargin - 18),(this.cHeight - this.heightMargin + 18));\n" +                                                                                                                            
    "       i++;\n" +                                                                                                                                                                                                                                           
    "       }\n" +                                                                                                                                                                                                                                              
    "       this.ctx.stroke();\n" +                                                                                                                                                                                                                             
    "     }\n" +                                                                                                                                                                                                                                               
    "     LineChart.prototype.plotdata = function(data, index) {\n" + 
    "       data_height = this.originHeight - data[0] / 200000 * (this.cHeight - 2 * this.heightMargin - this.arrowWidth);\n" + 
    "       data_width = this.originWidth;\n" + 
    "       this.ctx.beginPath();\n" + 
    "       let obj = {\n" + 
    "         1 : () => { this.ctx.strokeStyle = \"red\"; },\n" + 
    "         2 : () => { this.ctx.strokeStyle = \"blue\"; },\n" +
    "         3 : () => { this.ctx.strokeStyle = \"orange\"; },\n" +
    "         4 : () => { this.ctx.strokeStyle = \"green\"; },\n" +
    "       };\n" +
    "       obj[index]();\n" +
    "       this.ctx.lineWidth = this.plotWidth;\n" +
    "       this.ctx.moveTo(data_width,data_height);   // move to first point\n" +
    "       for(i = 1;i < data.length;i++) {\n" +
    "         data_height = this.originHeight - data[i] / 200000 * (this.cHeight - 2 * this.heightMargin - this.arrowWidth);\n" +
    "         data_width = this.originWidth + (this.cWidth - 2 * this.widthMargin - this.arrowWidth) / (data.length - 1) * i;\n" +
    "         this.ctx.lineTo(data_width,data_height);\n" +
    "       }\n" +
    "       this.ctx.stroke();\n" +
    "     }\n" +                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
    "       \n" +                                                                                                                                                                                                                                            
    "     var LineChart = new LineChart();\n" +                                                                                                                                                                                                                
    "     LineChart.init();\n" +                                                                                                                                                                                                                               
    "                    \n" +                                                                                                                                                                                                                                                                                                                                                                                                                                          
    "   </script>\n" +    
    "<script>\n" +
    "function save_data(index)\n" +
    "{\n" +
    "    let gets = \"data:text/csv;charset=utf-8,\\ufeff\";\n" +
    "    let leng = sessionStorage.length;\n" +
    "    let i=0;\n" +
    "    while(leng--)\n" +
    "    {\n" +
    "        if (sessionStorage.key(i)[0] == index)\n" +
    "        {\n" +
    "            gets = gets + sessionStorage.getItem(sessionStorage.key(i)) + \"\\n\";\n" +
    "        }\n" +
    "        i++;\n" +
    "    }\n" +
    "    var link = document.createElement(\"a\");\n" +
    "    //为a标签设置属性\n" +
    "    link.setAttribute(\"href\", gets);\n" +
    "    const file_name = \"sensor\" + index + \"\" + \".csv\"\n" +
    "    link.setAttribute(\"download\", file_name);\n" +
    "    //点击a标签\n" +
    "    link.click();\n" +
    "}\n" +
    "function erase_data(){\n" +
    "    var counter = document.getElementById('count_div');\n" +
    "    counter.textContent = 0;\n" +
    "    sessionStorage.clear();" +
    "}\n" +
    "var sess_counter = sessionStorage.getItem('sample_counter');\n" +
    "var counter = document.getElementById('count_div');\n" +
    "if(sess_counter == null){\n" +
    "    counter.textContent = String(0);\n" +
    "}\n" +
    "else {\n" +
    "    counter.textContent = sess_counter;\n" +
    "}\n" +
    "</script>\n" +                                                                                                                                                                                                                                    
    " </body>\n" +
    "</html>\n";

String autopage = String("") +
"<!DOCTYPE html>\n" +
"<html lang=\"en\">\n" +
"<head>\n" +
"    <style>\n" +
"    table {\n" +
"        width: 100%;\n" +
"        border-collapse: collapse; /* 使边框合并为一个 */\n" +
"    }\n" +
"    td {\n" +
"        font-size: 10px;\n" +
"        border: 1px solid #000000; /* 边框颜色 */\n" +
"        text-align: left; /* 文本对齐方式 */\n" +
"        padding: 4px; /* 单元格内边距 */\n" +
"        background-color: #d3d1d4;\n" +
"    }\n" +
"    .no-wrap {\n" +
"      white-space: nowrap;\n" +
"    }\n" +
"</style>\n" +
"    <meta charset=\"UTF-8\">\n" +
"    <title>Neural network facilitates gases discrimination for temperature-modulated MOS gas sensor</title>\n" +
"</head>\n" +
"<body>\n" +
"    <h2 align=\"center\">Auto Sample for temperature-modulated MOS gas sensor</h2>\n" +
"     &nbsp;&nbsp;&nbsp;&nbsp;We propose to use CNN for feature extraction and pattern recognition, which effectively improves the recognition accuracy of the sensor and realizes the classification and identification of gas by a single sensor.\n" +
"    R=10000*(409500-data)/data <br/>\n" +
"    <form method=\"post\"action=\"END\">\n" +
"        <table align=\"center\"><tr><th>\n" +
"           <button name=\"END\"type=\"submit\"value=\"0\"id=\"0\"style=\"width:140px;height:20px\">END AUTO</button>\n" +
"        </th></tr>\n" +
"        </table>\n" +
"    </form>\n" +
"</body>\n" +
"</html>\n" +
"<table id='data-table'></table>\n";

String sampleAdd = String("") +
"<script>\n" +
"var sess_counter = sessionStorage.getItem('sample_counter');\n" + 
"if(sess_counter == null){\n" +
"      sessionStorage.setItem('sample_counter', String(1));\n" +
"}\n" +
"else{\n"
"      sessionStorage.setItem('sample_counter', parseInt(sess_counter)+1);\n" +
"}\n" +
"</script>\n";

int Vout_adj=0;
int cnt=0;
unsigned char detec_sta=0;
long data1[DATACOUNT],data2[DATACOUNT],data3[DATACOUNT],data4[DATACOUNT];
unsigned char detc_cnt=0;
Ticker Vadj;
Ticker detc;
String curr_string = "";
bool preConnected = false;
unsigned long preStart = millis();

// VOUT() used to heat sensor
void VOUT(){
  unsigned char dataH, dataL;
  // SPI_MODE1在上升沿时改变SPI输出数据，下降沿时采样数据
  // pin15负责时钟同步发送准备帧，即上升沿
  // pin18负责处理时钟同步
  digitalWrite(15, LOW);
  delayMicroseconds(20);
  //SPI.write(dataH);
  //SPI.write(dataL);
  // pin23负责发送SPI数据
  // 9830代表了从3V开始取高12位
  // 16370代表了5V取高12位对应4095
  // MEMS加热电压1.8V就不能给到5V？TGS2602是5V的加热电压
  if(detec_sta==2){
    Vout_adj=Vout_adj+4;
    SPI.write16(Vout_adj);
  }else{
    SPI.write16(VOUTSTART);
  }
  delayMicroseconds(20);
  digitalWrite(15, HIGH);
}

long det_sensor(unsigned char j){
  long detec_tmp=0;
  // ESP32本身就是12位ADC引脚，为什么还需要一个额外的12位ADC？
  // ESP32的ADC引脚时电容性引脚
  // 读取的是电压，0-4095总共4096个数，代表了从0-5V
  // 电压 = (number/4095)*3.3
  // 电阻 = U/I = (3.3-(number/4095)*3.3)/(number*3.3/(4095*10^4))
  // 10KΩ是最优的电阻根据传感器测量电阻
  // 读取100次ADC数据值？更加精确的获取测量的电压值？
  for (int i=0;i<100;i++){
    switch(j){
      case 1: detec_tmp+=analogRead(36);  break;
      case 2: detec_tmp+=analogRead(39);  break;
      case 3: detec_tmp+=analogRead(34);  break;
      case 4: detec_tmp+=analogRead(35);  break;
      default:  break;
    }
  }
  return detec_tmp;
}

// collect the data from sensor
void detec_ad(){
    if(detc_cnt>=DATACOUNT) return;
    data1[detc_cnt] = det_sensor(1);
    data2[detc_cnt] = det_sensor(2);
    data3[detc_cnt] = det_sensor(3);
    data4[detc_cnt] = det_sensor(4);
    detc_cnt++;
}

//void sendTCPBemfa(const String &msg){
//  checkBemfa();
//  bemfaclient.print(msg);
//  Serial.println(msg);
//}

//void connectBemfa(){
//  Serial.println("connecting bemfa tcp");
//  if(bemfaclient.connect(TCP_SERVER_ADDR, atoi(TCP_SERVER_PORT))){
//    String tcpHeader = "";
//    tcpHeader = "cmd=1&uid=" + TCP_UID + "&topic=" + topic + "\r\n";
//    Serial.println(tcpHeader);
//    sendTCPBemfa(tcpHeader);
//    preConnected = true;
//    bemfaclient.setNoDelay(true); 
//  }
//  else{
//    bemfaclient.stop();
//    preConnected = false;
//    Serial.println("connect failed");
//  }
//  preStart = millis();
//}

//void checkBemfa(){
//  while(!bemfaclient.connected()){
//    delay(500);
//    Serial.println("client disconnect");
//    if(preConnected == true){
//      preConnected = false;
//      preStart = millis();
//      bemfaclient.stop();
//    }
//    else if(millis() - preStart > 1000) connectBemfa();
//  }
//}

//void bemfaCallback(){
//  checkBemfa();
//  while(bemfaclient.available()){
//     char c = bemfaclient.read();
//     curr_string += c;
//  }
//  Serial.println("接收的消息为:" + curr_string);
////  if(curr_string)
//}

void httpCallback(){
  //看似是esp32和网页端的数据互通，实际上是一种TCP通讯，ESP32作为TCP服务器端，浏览器发出对某一IP地址的请求，ESP32接收请求并响应对应的数据给浏览器进行展示。
  //故要解决自动采集数据并上传网页端，必须换种网页端的实现。
  WiFiClient client = server.available();
  if(client)
  {
    delay(5);     // wait for request to arrive
    if(client.connected())
    {
      while(client.available())     // read everything in data buffer
      {
        char c = client.read();
        curr_string += c;
      }
      Serial.println(curr_string);
      //web part
      if(curr_string.startsWith("GET / HTTP/1.1"))
      {
        client.print(responseHeaders);
        client.print(myhtmlPage);
        client.print("\r\n");
      }
      else if(curr_string.startsWith("POST /Start"))
      {
        client.print(responseHeaders);
        client.print(myhtmlPage);
        client.print("\r\n"); 
        singleSample(client, false);       
      }
      else if(curr_string.startsWith("POST /Sensor1"))
      {
        handleSensorData(client, data1, "1", cnt);
      }
      else if(curr_string.startsWith("POST /Sensor2"))
      {
        handleSensorData(client, data2, "2", cnt);
      }
      else if(curr_string.startsWith("POST /Sensor3"))
      {
        handleSensorData(client, data3, "3", cnt);
      }
      else if(curr_string.startsWith("POST /Sensor4"))
      {
        handleSensorData(client, data4, "4", cnt);
      }
      else if(curr_string.startsWith("POST /autoMode")){
        client.print(responseHeaders);
        client.print(autopage);
        autoSample(client);
      }
      else if(curr_string.startsWith("POST /END")){
        client.print(responseHeaders);
        client.print(myhtmlPage);
      }
      //wx part
      else if(curr_string.endsWith("Start")){
        singleSample(client, true);
        client.print(responseHeaders);
        wxSensorData(client, data1, 1, cnt);
        wxSensorData(client, data2, 2, cnt);
        wxSensorData(client, data3, 3, cnt);
        wxSensorData(client, data4, 4, cnt);
      }
      else if(curr_string.endsWith("Status")){
        client.print(responseHeaders);
        client.print("online");
      }
      else if(curr_string.endsWith("Auto")){
        client.print(responseHeaders);
        wxAutoSensor(client);
      }
      curr_string = "";
    }
    client.stop();
  }
}

void setup()
{
  Serial.begin(115200);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  SPI.setDataMode(SPI_MODE1);
  delay(1000);

  pinMode(15, OUTPUT);  //DAC SPI CS_
  pinMode(LED2, OUTPUT);//LED Board
  digitalWrite(15, HIGH);
  digitalWrite(LED2, LOW);

  Vadj.attach_ms(2,VOUT);  // loop VOUT+4 operator for every 2 millisecond

  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);
  

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("CONNECTED");
  
  server.begin();

//  connectBemfa();

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

void loop()
{
  delay(1);
  httpCallback();
//  bemfaCallback();
}

void wxAutoSensor(WiFiClient _client){
  for(int i=0;i<AUTOTIMES;i++){
    delay(1000);
    singleSample(_client, true);
    wxSensorData(_client, data1, 1, cnt);
    wxSensorData(_client, data2, 2, cnt);
    wxSensorData(_client, data3, 3, cnt);
    wxSensorData(_client, data4, 4, cnt);
  }
}

void wxSensorData(WiFiClient _client, long *_data, int sensorNumber, int cnt_temp){
  String datastr = String(sensorNumber) + "_" + cnt_temp + ",";
  for(int i=0;i<DATACOUNT;i++){
    datastr = datastr + String(_data[i]) + ",";
  }
  _client.print(datastr);
}

void handleSensorData(WiFiClient _client, long _data[], String sensorNumber, int cnt_temp) {
        _client.print(responseHeaders);
        _client.print(myhtmlPage);
        //作图
        _client.print("<script>\n");
        _client.print("var data = new Array(");
        _client.print(DATACOUNT);
        _client.print(");\n");
        for(unsigned char det_cnt=0;det_cnt<DATACOUNT;det_cnt++){
          _client.print("data[");
          _client.print(det_cnt);
          _client.print("]=");
          _client.print(_data[det_cnt]);
          _client.print(";\n");
        }
        _client.print("LineChart.plotdata(data," + sensorNumber + ");\n");
        //将数据缓存到sessionStorage上用以保存到本地
        _client.print("sessionStorage.setItem(\"" + sensorNumber + "_\"+'");
        _client.print(cnt_temp);
        _client.print("', data);\n");
//        _client.print("var counter = document.getElementById('count_div');\n");
//        _client.print("counter.textContent = String(parseInt(counter.textContent)+1);\n");
        _client.print("</script>\n");
        _client.print("\r\n");
        //文本数据
        _client.print("<div1>");
        for(unsigned char det_cnt=0;det_cnt<DATACOUNT;det_cnt++){
          _client.print(_data[det_cnt]);
          _client.print(",\n");
        }
        _client.print("</div1>");
        _client.print("\r\n");
}

void singleSample(WiFiClient _client, bool iswx){
  //增加计数
  cnt++;
  if(!iswx) _client.print(sampleAdd);

  
  Vout_adj=VOUTSTART;
  detec_sta=1;
  digitalWrite(LED2,HIGH);
  while(1){
    if(detec_sta==1){
      detc.attach_ms(SAMPLE,detec_ad);
      detec_sta=2;
      detc_cnt=0;
    }
    if(Vout_adj>=VOUTEND){
      detc.detach();
      if(detec_sta==2){
        detec_sta=3;
        digitalWrite(LED2,LOW);
      }
      break;
    }
  }
}

String showData(int sensor, int cnt_t){
  String res = String("") + 
  "var tableBody = document.getElementById('data-table');\n" +
  "var tr = document.createElement('tr');\n" +
  "var td = document.createElement('td');\n" +
  "var tdh = document.createElement('td');\n" +
  "tdh.className = \"no-wrap\";\n" +
  "tdh.textContent = \"" + String(sensor) + "_" + String(cnt_t) + "\";\n" +
  "td.className = \"no-wrap\";\n" +
  "td.textContent = data;\n" 
  "tr.appendChild(tdh);\n" +
  "tr.appendChild(td);\n" +
  "tableBody.appendChild(tr);\n";
  return res;
}

void autoSample(WiFiClient _client){
  for(int i=0;i<AUTOTIMES;i++){
    delay(1000);
    singleSample(_client, false);
    autoSensor(_client, data1, 1, cnt);
    autoSensor(_client, data2, 2, cnt);
    autoSensor(_client, data3, 3, cnt);
    autoSensor(_client, data4, 4, cnt);
  }
}

void autoSensor(WiFiClient _client, long _data[], int sensorNumber, int cnt_temp){
  _client.print("<script>\nvar data = new Array();\n");
    for(unsigned char det_cnt=0;det_cnt<DATACOUNT;det_cnt++){
      _client.print("data.push(");
      _client.print(_data[det_cnt]);
      _client.print(");\n");
    }
    _client.print(showData(sensorNumber, cnt_temp));
    _client.print("sessionStorage.setItem(\"");
    _client.print(sensorNumber);
    _client.print("_");
    _client.print(cnt_temp);
    _client.print("\", data);\n");
    _client.print("</script>\n");
}

#include "Datastructure.h"
#include <ArduinoJson.h>
#include <string>
#include <stack>

using namespace std;

stack<char> myStack;
stack<char> myStack2;

String dateTime1 = "";
String latitude1 = "";
String longitude1 = "";
String depth1 = "";
String temp1 = "";
String DO_value1 = "";

String myString = "";

void copy(String& value) {
    value = "";
    while (!myStack.empty()) {
        myStack2.push(myStack.top());
        myStack.pop();
    }
    while (!myStack2.empty()) {
        value += myStack2.top();
        myStack2.pop();
    }
}

String getData(String& value, String DataType) {
    value += ',';
    int numberOfString = value.length();
    int count = 0;
    if (!value.isEmpty()) {
        for (int i = 0; i < numberOfString; i++) {
            if (value[i] == ',') {
                count++;
                switch (count) {
                case 1:
                    copy(dateTime1);
                    break;
                case 2:
                    copy(latitude1);
                    break;
                case 3:
                    copy(longitude1);
                    break;
                case 4:
                    copy(depth1);
                    break;
                case 5:
                    copy(temp1);
                    break;
                case 6:
                    copy(DO_value1);
                    break;
                default:
                    break;
                }
            }
            else {
                myStack.push(value[i]);
            }
        }
    }
    if (DataType == "dateTime0") { return dateTime1; }
    else if (DataType == "latitude0") { return latitude1; }
    else if (DataType == "longitude0") { return longitude1; }
    else if (DataType == "depth0") { return depth1; }
    else if (DataType == "temp0") { return temp1; }
    else if (DataType == "DO_value0") { return DO_value1; }
    else return "";
}

struct Node {
  String data;
  Node* next;
};

Node* head = NULL;
Node* tail = NULL;

int listSize = 0;

void addDataToList(String data) {
  Node* newNode = new Node;
  newNode->data = data;
  newNode->next = NULL;
  if (head == NULL) {
    head = newNode;
    tail = newNode;
  } else {
    tail->next = newNode;
    tail = newNode;
  }
  listSize++;
  
  if (listSize > 100) {
    Node* temp = head;
    head = head->next;
    delete temp;
    listSize--;
  }
}

String intToString(int number) {
    if (number == 0) {
        return "0";
    }

    String result = "";
    bool isNegative = false;

    // Xử lý số âm
    if (number < 0) {
        isNegative = true;
        number = -number;
    }

    // Chia từng chữ số và chuyển đổi thành ký tự
    while (number > 0) {
        char digit = '0' + (number % 10);
        result = digit + result;
        number /= 10;
    }
    // Thêm dấu âm nếu là số âm
    if (isNegative) {
        result = '-' + result;
    }
    return result;
}

void displayDataList(WiFiClient& client) {

  int  oderNumber = 1;

  Node* current = head;
 
    client.println("<style>");
    client.println("table {");

    client.println("  margin-left: auto;");
    client.println("  margin-right: auto;");
    client.println("}");
    client.println("th, td {");
    client.println("  padding: 15px;");
    client.println("}");
    client.println("</style>");
    client.println("<table>");
    client.println("<thead>");
    client.println("<tr>");
    client.println("<th>STT</th>");
    client.println("<th>Date Time</th>");
    client.println("<th>Latitude</th>");
    client.println("<th>Longitude</th>");
    client.println("<th>Depth(mm)</th>");
    client.println("<th>Temp(oC)</th>");
    client.println("<th>DO Value(ug/L)</th>");
    
    while(current != NULL){

        String htmlString = "<tr><td>" + intToString(oderNumber) + "</td><td>" + getData(current->data, "dateTime0") 
        + "</td><td>" + getData(current->data, "latitude0") + "</td><td>" + getData(current->data, "longitude0") 
        + "</td><td>" + getData(current->data, "depth0") + "</td><td>" + getData(current->data, "temp0") 
        + "</td><td>" + getData(current->data, "DO_value0")+ "</td></tr>";
       
        // String htmlString =  intToString(oderNumber);
        // String dateTime =  getData(current->data, "dateTime0");
        // String latitude =  getData(current->data, "latitude0");
        // String longitude =  getData(current->data, "longitude0");
        // String depth =  getData(current->data, "depth0");
        // String temp =  getData(current->data, "temp0");
        // String DO_value = getData(current->data, "DO_value0");

        client.println(htmlString);
        current = current->next;
        oderNumber++;
    }
    client.println("</tr>");
    client.println("</thead>");
    client.println("<tbody>");

    client.println("</tbody>");
    client.println("</table>");
}
//Using no_ota.csv partition, we have ~2MB for sketch and ~2MB for SPIFFS
void SPIFFS_saveStringDataToFile( String fileName_string, String fileContent_string){
  File writeFile;
  String locationFileSaveData = "/";
  locationFileSaveData = locationFileSaveData + fileName_string + ".csv"; //se truyen vao dataTime
  Serial.println("Writing file:" + locationFileSaveData);
  writeFile = SPIFFS.open(locationFileSaveData, FILE_APPEND);		// mo file de ghi du lieu
  if(writeFile){
    if(writeFile.println(fileContent_string)){
      Serial.println("File written successfully");
    } else {
      Serial.println("Write failed");
    }
    writeFile.close();
  }
  else{
    Serial.println("Failed to open file for writing");
    return;
  }
}

void SPIFFS_checkOutOfMemory() {
  size_t totalBytes = SPIFFS.totalBytes();
  size_t usedBytes = SPIFFS.usedBytes();

  // Tính toán sự chênh lệch
  size_t freeBytes = totalBytes - usedBytes;

  // Kiểm tra xem có cần xóa tệp không
  if (freeBytes <= 100 * 1024) { // 100KB

    // Mở thư mục SPIFFS
    File root = SPIFFS.open("/");

    // Lặp qua các tệp và xóa chúng (sẽ xóa các tệp cũ trước)
    while (File file = root.openNextFile()) {
      Serial.print("Deleting file: ");
      Serial.println(file.name());
      file.close();
      SPIFFS.remove(file.name());

      // Cập nhật lại dung lượng đã sử dụng
      usedBytes = SPIFFS.usedBytes();
      freeBytes = totalBytes - usedBytes;

      // Kiểm tra xem đã đạt đến điều kiện cần xóa đủ tệp chưa
      if (freeBytes > 100 * 1024) // Nếu đã đủ, thoát khỏi vòng lặp
        break;
    }
    root.close();
  }
}
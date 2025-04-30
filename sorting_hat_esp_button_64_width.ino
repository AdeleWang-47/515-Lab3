#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "sorting_hat_model.h"  // 已转换好的ML模型

// OLED配置
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 按钮引脚（需对应接线）
#define BUTTON_A  25  // GPIO15
#define BUTTON_B  26   // GPIO2
#define BUTTON_C  27   // GPIO4
#define BUTTON_D  14  // GPIO13（推荐使用此代替GPIO34，后者不支持输入上拉）

// 题目与选项
const char* questions[] = {
    "1. What do you value?",
    "2. What to do if someone cheats?",
    "3. Favorite subject?",
    "4. How do you face challenges?",
    "5. How do friends describe you?",
    "6. What to do with a mystery book?",
    "7. Preferred pet?",
    "8. How do you solve problems?",
    "9. What kind of friends do you like?",
    "10. Dream career?"
};

const char* options[][4] = {
    {"A) Bravery", "B) Loyalty", "C) Intelligence", "D) Ambition"},
    {"A) Call them out", "B) Let them be", "C) Inform teacher", "D) Gain from it"},
    {"A) Defense Arts", "B) Herbology", "C) Charms", "D) Potions"},
    {"A) Face head-on", "B) Team up", "C) Plan first", "D) Outsmart it"},
    {"A) Bold", "B) Kind", "C) Smart", "D) Resourceful"},
    {"A) Read it now", "B) Check safety", "C) Study it", "D) Use for gain"},
    {"A) Owl", "B) Toad", "C) Cat", "D) Phoenix"},
    {"A) Act fast", "B) Find a compromise", "C) Analyze first", "D) Outsmart"},
    {"A) Adventurous", "B) Loyal", "C) Thoughtful", "D) Powerful"},
    {"A) Auror", "B) Healer", "C) Scholar", "D) Minister"}
};

int responses[10] = {0};
int questionIndex = 0;

// 初始化模型
Eloquent::ML::Port::DecisionTree clf;

void setup() {
    Serial.begin(115200);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 allocation failed");
        for (;;);
    }
    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.println("Sorting Hat Ready!");
    display.display();
    delay(2000);

    pinMode(BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_B, INPUT_PULLUP);
    pinMode(BUTTON_C, INPUT_PULLUP);
    pinMode(BUTTON_D, INPUT_PULLUP);

    showQuestion();
}

void loop() {
    checkButtons();  // 按钮等待
}

void showQuestion() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(questions[questionIndex]);

    for (int i = 0; i < 4; i++) {
        display.setCursor(10, 20 + (i * 10));
        display.println(options[questionIndex][i]);
    }
    display.display();
}

void checkButtons() {
    bool buttonPressed = false;

    while (!buttonPressed) {
        if (digitalRead(BUTTON_A) == LOW) {
            responses[questionIndex] = 1;
            buttonPressed = true;
            Serial.println("Button A pressed");
        } else if (digitalRead(BUTTON_B) == LOW) {
            responses[questionIndex] = 2;
            buttonPressed = true;
            Serial.println("Button B pressed");
        } else if (digitalRead(BUTTON_C) == LOW) {
            responses[questionIndex] = 3;
            buttonPressed = true;
            Serial.println("Button C pressed");
        } else if (digitalRead(BUTTON_D) == LOW) {
            responses[questionIndex] = 4;
            buttonPressed = true;
            Serial.println("Button D pressed");
        }
        delay(50);  // 防抖
    }

    nextQuestion();
}

void nextQuestion() {
    questionIndex++;
    if (questionIndex < 10) {
        showQuestion();
    } else {
        classifyHouse();
    }
}

void classifyHouse() {
    display.clearDisplay();
    display.setCursor(10, 10);
    display.println("Sorting...");

    float features[10];
    for (int i = 0; i < 10; i++) {
        features[i] = (float)responses[i];
    }

    int house = clf.predict(features);  // 调用模型预测

    display.setCursor(10, 30);
    display.print("House: ");
    switch (house) {
        case 0: display.println("Gryffindor"); break;
        case 1: display.println("Hufflepuff"); break;
        case 2: display.println("Ravenclaw"); break;
        case 3: display.println("Slytherin"); break;
    }

    display.display();
    Serial.println("Sorting complete!");
}

#if LOGGING
#define setupLogger(rate) Serial.begin(rate);
#define log(content) Serial.print(content);
#define logln(content) Serial.println(content);
#else
#define setupLogger(rate)
#define log(content)
#define logln(content)
#endif

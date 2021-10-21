enum show_t { no, yes, force };

void connect_wifi_get_time(void);
void SetDigit(uint8_t digit, uint8_t value, show_t show);
void ShowDigit(uint8_t digit);
void Update_time_display();
void get_time(void);
long long getTimestamp(String url);

void network_init(String ssid, String password);
void network_reconnect();
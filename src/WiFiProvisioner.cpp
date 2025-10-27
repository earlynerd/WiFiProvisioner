#include "WiFiProvisioner.h"
#include "internal/provision_html.h" // Include the corrected header
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>

#define WIFI_PROVISIONER_LOG_DEBUG 0
#define WIFI_PROVISIONER_LOG_INFO 1
#define WIFI_PROVISIONER_LOG_WARN 2
#define WIFI_PROVISIONER_LOG_ERROR 3

#define WIFI_PROVISIONER_DEBUG // Uncomment to enable debug prints via Serial

#ifdef WIFI_PROVISIONER_DEBUG
#define WIFI_PROVISIONER_DEBUG_LOG(level, format, ...)                         \
  do {                                                                         \
    if (level >= WIFI_PROVISIONER_LOG_INFO) { /* Adjust log level here if needed */ \
      Serial.printf("[WiFiProv-%s] " format "\n",                              \
                    (level == WIFI_PROVISIONER_LOG_DEBUG)  ? "DEBUG"           \
                    : (level == WIFI_PROVISIONER_LOG_INFO) ? "INFO"            \
                    : (level == WIFI_PROVISIONER_LOG_WARN) ? "WARN"            \
                                                           : "ERROR",          \
                    ##__VA_ARGS__);                                            \
    }                                                                          \
  } while (0)
#else
#define WIFI_PROVISIONER_DEBUG_LOG(level, format, ...) do {} while (0) // Empty macro
#endif

namespace {
// --- Helper functions (convertRRSItoLevel, networkScan, sendStandardHeaders) ---
// --- (Unchanged from the previous correct version) ---
/**
 * @brief Converts a Received Signal Strength Indicator (RSSI) value to a signal
 * strength level (0-4).
 */
int convertRRSItoLevel(int rssi) {
  //  Convert RSSI to 0 - 4 Step level
  int numlevels = 4;
  int MIN_RSSI = -100;
  int MAX_RSSI = -55;

  if (rssi <= MIN_RSSI) { // Use <= for clarity
    return 0;
  } else if (rssi >= MAX_RSSI) {
    return numlevels;
  } else {
    // Linear interpolation
    float inputRange = (float)(MAX_RSSI - MIN_RSSI);
    float outputRange = (float)numlevels;
    int level = (int)round(((float)(rssi - MIN_RSSI) * outputRange) / inputRange);
    // Ensure level is at least 1 if within range but calculated as 0
    return max(1, level);
  }
}

/**
 * @brief Scans for available Wi-Fi networks and populates a JSON document.
 */
void networkScan(JsonDocument &doc) {
  JsonArray networks = doc["network"].to<JsonArray>();

  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO,
                             "Starting Network Scan...");
  // WiFi.scanNetworks will return the number of networks found, or -1/-2 on error.
  int n = WiFi.scanNetworks(false, false); // Async=false, ShowHidden=false
  if (n > 0) {
     WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Found %d networks.", n);
    for (int i = 0; i < n; ++i) {
      JsonObject network = networks.add<JsonObject>();
      int rssiVal = WiFi.RSSI(i);
      network["rssi"] = convertRRSItoLevel(rssiVal);
      network["ssid"] = WiFi.SSID(i);
      // Determine authentication mode (0 = Open, 1 = Secured)
      network["authmode"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? 0 : 1;
       WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "  [%d] SSID: %s, RSSI: %d (%d), Auth: %d", i, WiFi.SSID(i).c_str(), rssiVal, network["rssi"].as<int>(), network["authmode"].as<int>());
    }
  } else if (n == 0) {
      WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_WARN, "No networks found during scan.");
  } else {
      WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR, "WiFi scan failed with code: %d", n);
  }
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Network scan complete.");
}


/**
 * @brief Sends standard HTTP headers for a response.
 */
void sendStandardHeaders(WiFiClient &client, int statusCode, const char *contentType) {
    client.print("HTTP/1.1 "); client.print(statusCode); client.println(" OK");
    client.print("Content-Type: "); client.println(contentType);
    client.println("Cache-Control: no-cache, no-store, must-revalidate");
    client.println("Pragma: no-cache");
    client.println("Expires: -1");
    client.println("Connection: close"); // Important: close connection after response
    // Content-Length will be added separately if known, otherwise chunked or close is needed
}


} // end anonymous namespace

// --- Class Constructor and Destructor ---
// --- (Unchanged) ---
WiFiProvisioner::Config::Config(
    const char *apName, const char *htmlTitle, const char *themeColor,
    const char *svgLogo, const char *projectTitle, const char *projectSubTitle,
    const char *projectInfo, const char *footerText,
    const char *connectionSuccessful, const char *resetConfirmationText,
    const char *inputText, int inputLength, bool showInputField,
    bool showResetField, const char *usernameText,
    const char *servicePasswordText, bool showLoginFields)
    : AP_NAME(apName), HTML_TITLE(htmlTitle), THEME_COLOR(themeColor),
      SVG_LOGO(svgLogo), PROJECT_TITLE(projectTitle),
      PROJECT_SUB_TITLE(projectSubTitle), PROJECT_INFO(projectInfo),
      FOOTER_TEXT(footerText), CONNECTION_SUCCESSFUL(connectionSuccessful),
      RESET_CONFIRMATION_TEXT(resetConfirmationText), INPUT_TEXT(inputText),
      INPUT_LENGTH(inputLength), SHOW_INPUT_FIELD(showInputField),
      SHOW_RESET_FIELD(showResetField), USERNAME_TEXT(usernameText),
      SERVICE_PASSWORD_TEXT(servicePasswordText),
      SHOW_LOGIN_FIELDS(showLoginFields) {}

WiFiProvisioner::WiFiProvisioner(const Config &config)
    : _config(config), _server(nullptr), _dnsServer(nullptr),
      _apIP(192, 168, 4, 1), _netMsk(255, 255, 255, 0), _dnsPort(53),
      _serverPort(80), _wifiDelay(100), _wifiConnectionTimeout(10000), // Default 10 seconds
      _serverLoopFlag(false) {}

WiFiProvisioner::~WiFiProvisioner() {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "WiFiProvisioner destructor called.");
    releaseResources();
}

// --- Public Methods (getConfig, releaseResources, startProvisioning, loop, callbacks) ---
// --- (Unchanged) ---
WiFiProvisioner::Config &WiFiProvisioner::getConfig() { return _config; }

void WiFiProvisioner::releaseResources() {
  _serverLoopFlag = true; // Signal loop to stop if running

  // Webserver
  if (_server != nullptr) {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Stopping web server...");
    _server->stop();
    delete _server;
    _server = nullptr;
     WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Web server stopped and deleted.");
  }

  // DNS
  if (_dnsServer != nullptr) {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Stopping DNS server...");
    _dnsServer->stop();
    delete _dnsServer;
    _dnsServer = nullptr;
     WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "DNS server stopped and deleted.");
  }

  // WiFi - Don't necessarily change mode here, depends on context
  // if (WiFi.getMode() != WIFI_STA) {
  //   WiFi.mode(WIFI_STA);
  //   delay(_wifiDelay);
  // }
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Resources released.");
}

bool WiFiProvisioner::startProvisioning() {
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Starting provisioning process...");
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Disconnecting existing WiFi connection.");
  WiFi.disconnect(false, true); // Disconnect, don't erase credentials yet
  delay(_wifiDelay);

  releaseResources(); // Ensure clean state before starting

  _server = new WebServer(_serverPort);
  _dnsServer = new DNSServer();

  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Setting WiFi mode to AP+STA.");
  if (!WiFi.mode(WIFI_AP_STA)) {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR,
                               "Failed to set WiFi mode to AP+STA.");
    releaseResources(); // Clean up allocated resources on failure
    return false;
  }
  delay(_wifiDelay);

  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Configuring soft AP (IP: %s)...", _apIP.toString().c_str());
  if (!WiFi.softAPConfig(_apIP, _apIP, _netMsk)) {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR,
                               "Failed to configure soft AP IP settings.");
     releaseResources();
    return false;
  }
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Starting soft AP named: '%s'", _config.AP_NAME);
  if (!WiFi.softAP(_config.AP_NAME)) {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR,
                               "Failed to start soft AP.");
    releaseResources();
    return false;
  }
  delay(_wifiDelay); // Allow AP to initialize
  IPAddress actualApIP = WiFi.softAPIP(); // Get the actual IP
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Soft AP started. IP address: %s", actualApIP.toString().c_str());


  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Starting DNS server...");
  _dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  // Start DNS server, mapping all domains ('*') to the AP's IP address
  if (!_dnsServer->start(_dnsPort, "*", actualApIP)) {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR,
                               "Failed to start DNS server.");
    releaseResources();
    return false;
  }
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "DNS server started.");

  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Setting up web server handlers.");
  // --- Define Server Routes ---
  _server->on("/", HTTP_GET, [this]() { this->handleRootRequest(); });
  _server->on("/configure", HTTP_POST, [this]() { this->handleConfigureRequest(); });
  _server->on("/update", HTTP_GET, [this]() { this->handleUpdateRequest(); });
  _server->on("/factoryreset", HTTP_POST, [this]() { this->handleResetRequest(); });

  // --- Captive Portal Routes ---
  // Redirect common captive portal checks to the root page
  _server->on("/generate_204", HTTP_GET, [this]() { this->handleRootRequest(); }); // Android
  _server->on("/fwlink", HTTP_GET, [this]() { this->handleRootRequest(); }); // Microsoft
  _server->on("/hotspot-detect.html", HTTP_GET, [this]() { this->handleRootRequest(); }); // Apple
  _server->on("/success.html", HTTP_GET, [this]() { this->handleRootRequest(); }); // Some systems might request this after connection
  _server->on("/ncsi.txt", HTTP_GET, [this]() { this->handleRootRequest(); }); // Microsoft NCSI
  _server->on("/connecttest.txt", HTTP_GET, [this]() { this->handleRootRequest(); }); // Other system check

  // --- Fallback Route ---
  _server->onNotFound([this]() { this->handleRootRequest(); });

  _server->begin(); // Start the web server
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO,
                             "Web server started. Access portal at http://%s/",
                             actualApIP.toString().c_str());

  _serverLoopFlag = false; // Reset loop flag before starting the loop
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Entering server loop...");
  loop(); // Enter the server loop (blocking until provisioning complete/failed)

  // --- After loop() exits ---
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Exited server loop. Provisioning process finished.");
  // At this point, releaseResources() should have been called by loop() or handleConfigureRequest()
  
  // Return true only if the final WiFi status is connected
  bool connected = (WiFi.status() == WL_CONNECTED);
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "startProvisioning returning: %s", connected ? "true (connected)" : "false (not connected)");
  return connected;
}


void WiFiProvisioner::loop() {
    // WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Server loop tick..."); // Too noisy
  while (!_serverLoopFlag) {
    // DNS requests are typically for captive portal redirection
    if (_dnsServer) {
      _dnsServer->processNextRequest();
    }

    // Handle incoming HTTP client requests
    if (_server) {
      _server->handleClient();
    }
     yield(); // IMPORTANT: Allow ESP32 background tasks (like WiFi) to run
     delay(1); // Small delay to prevent tight loop hogging CPU
  }
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Server loop finished (_serverLoopFlag is true).");
  // Don't call releaseResources here, it's called by the function that sets the flag
}

WiFiProvisioner &WiFiProvisioner::onProvision(ProvisionCallback callback) {
  provisionCallback = std::move(callback);
  return *this;
}


WiFiProvisioner &WiFiProvisioner::onInputCheck(InputCheckCallback callback) {
  inputCheckCallback = std::move(callback);
  return *this;
}


WiFiProvisioner &
WiFiProvisioner::onFactoryReset(FactoryResetCallback callback) {
  factoryResetCallback = std::move(callback);
  return *this;
}


WiFiProvisioner &WiFiProvisioner::onSuccess(SuccessCallback callback) {
  onSuccessCallback = std::move(callback);
  return *this;
}


// --- Private Request Handlers ---

void WiFiProvisioner::handleRootRequest() {
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Handling root request '/'.");

  // Call the onProvision callback if it's set
  if (provisionCallback) {
     WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Calling onProvision callback.");
    provisionCallback();
  }

  // Get configuration values as C-strings for JS injection
  const char *showResetFieldStr = _config.SHOW_RESET_FIELD ? "true" : "false";
  const char *showLoginFieldsStr = _config.SHOW_LOGIN_FIELDS ? "true" : "false";
  const char *showInputFieldStr = _config.SHOW_INPUT_FIELD ? "true" : "false"; // Added this

  char inputLengthStr[12];
  snprintf(inputLengthStr, sizeof(inputLengthStr), "%d", _config.INPUT_LENGTH);

  // Get client
  WiFiClient client = _server->client();
  if (!client) {
       WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR, "No client available for root request.");
       return;
  }
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Client connected for root request.");

  // --- Send Headers ---
   sendStandardHeaders(client, 200, "text/html");
   client.println(); // End of headers


  // --- Send HTML Body ---
  // Send fixed parts and inject dynamic config values
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Sending HTML body parts...");
  client.write_P(index_html1, strlen_P(index_html1));            // Before Title
  client.print(_config.HTML_TITLE);                             // Title
  client.write_P(index_html2, strlen_P(index_html2));            // Before Theme Color
  client.print(_config.THEME_COLOR);                            // Theme Color
  client.write_P(index_html3, strlen_P(index_html3));            // Before Logo
  client.print(_config.SVG_LOGO);                               // Logo SVG
  client.write_P(index_html4, strlen_P(index_html4));            // Before Project Title (and includes up to end of <div class="header">)
  client.print(_config.PROJECT_TITLE);                          // Project Title
  //client.write_P(index_html5, strlen_P(index_html5));            // Before Project Subtitle
  //client.print(_config.PROJECT_SUB_TITLE);                      // Project Subtitle
  //client.write_P(index_html6, strlen_P(index_html6));            // Before Project Info
  //client.print(_config.PROJECT_INFO);                           // Project Info
  //client.write_P(index_html7, strlen_P(index_html7));            // HTML up to Device Key Input Block (end of hiddenPassword block)

  client.write_P(index_html8, strlen_P(index_html8));            // Device Key Input Block HTML

  client.write_P(index_html9_username_block, strlen_P(index_html9_username_block)); // Username block HTML
  client.write_P(index_html9_service_password_block, strlen_P(index_html9_service_password_block)); // Service password block HTML
  client.write_P(index_html9_part2, strlen_P(index_html9_part2)); // Submit button, start of footer


  // --- Inject JS Constants Correctly within the <script> tag ---
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Injecting JavaScript constants...");
  // Start of script and first constant definition
  client.write_P(js_const_part1, strlen_P(js_const_part1));   // `const title_logo = \``
  client.print(_config.SVG_LOGO);                             // Inject logo SVG (duplicate needed for JS)
  client.write_P(js_const_part1a, strlen_P(js_const_part1a)); // `\`; const title_text = \``
  client.print(_config.PROJECT_TITLE);                        // Inject project title
  client.write_P(js_const_part1b, strlen_P(js_const_part1b)); // `\`; const title_sub = \``
  client.print(_config.PROJECT_SUB_TITLE);                    // Inject subtitle
  client.write_P(js_const_part1c, strlen_P(js_const_part1c)); // `\`; const title_info = \``
  client.print(_config.PROJECT_INFO);                         // Inject info text

  client.write_P(js_const_part1d, strlen_P(js_const_part1d)); // `\`; const input_name_text = \``
  client.print(_config.INPUT_TEXT);                           // Prints Device Key label
  client.write_P(js_const_part2, strlen_P(js_const_part2));   // `\`; const input_lenght = `
  client.print(inputLengthStr);                               // Prints length for Device Key
  client.write_P(js_const_part3, strlen_P(js_const_part3));   // `; const connection_successful_text = \``
  client.print(_config.CONNECTION_SUCCESSFUL);                // Prints success message
  client.write_P(js_const_part4, strlen_P(js_const_part4));   // `\`; const footer_text = \``
  client.print(_config.FOOTER_TEXT);                          // Prints footer
  client.write_P(js_const_part5, strlen_P(js_const_part5));   // `\`; const reset_confirmation_text = \``
  client.print(_config.RESET_CONFIRMATION_TEXT);              // Prints reset confirm text
  client.write_P(js_const_part6, strlen_P(js_const_part6));   // `\`; const username_text = \``
  client.print(_config.USERNAME_TEXT);                        // Prints Username label
  client.write_P(js_const_part7, strlen_P(js_const_part7));   // `\`; const service_password_text = \``
  client.print(_config.SERVICE_PASSWORD_TEXT);                // Prints Service Password label
  client.write_P(js_const_part8, strlen_P(js_const_part8));   // `\`; const show_input_field = `
  client.print(showInputFieldStr);                            // Prints "true" or "false" for device key field visibility
  client.write_P(js_const_part8a, strlen_P(js_const_part8a)); // `; const show_login_fields = `
  client.print(showLoginFieldsStr);                           // Prints "true" or "false" for login fields visibility
  client.write_P(js_const_part9, strlen_P(js_const_part9));   // `; const reset_show = `
  client.print(showResetFieldStr);                            // Prints "true" or "false" for reset link visibility

  // --- Send Remainder of HTML (rest of <script> and </html>) ---
  client.write_P(index_html13, strlen_P(index_html13)); // Includes closing semicolon for reset_show and rest of JS/HTML

  // Connection: close header handles closing
  client.stop();
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Root request handled, response sent.");
}


// --- handleUpdateRequest (Added favicon handler) ---
void WiFiProvisioner::handleUpdateRequest() {
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Handling update request '/update'.");
  JsonDocument doc; // Use StaticJsonDocument if possible for fixed size

  // Determine which fields to show based on config
  doc["show_code"] = _config.SHOW_INPUT_FIELD;
  doc["show_login"] = _config.SHOW_LOGIN_FIELDS;
  
  // Perform network scan and populate the document
  networkScan(doc);

  WiFiClient client = _server->client();
   if (!client) {
       WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR, "No client available for update request.");
       return;
  }
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Client connected for update request.");
  
  // Calculate JSON length (important for Content-Length header)
  size_t jsonLength = measureJson(doc);
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "JSON response length: %d", jsonLength);

  // Send headers and JSON payload
  sendStandardHeaders(client, 200, "application/json");
  client.print("Content-Length: "); client.println(jsonLength); // Add Content-Length
  client.println(); // End headers
  serializeJson(doc, client);

  client.stop(); // Close connection
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Update request handled, response sent.");
}



// --- handleConfigureRequest (Remains largely the same, added logging) ---
void WiFiProvisioner::handleConfigureRequest() {
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Handling configure request '/configure'.");
  if (!_server->hasArg("plain")) {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_WARN,
                               "Configure request missing 'plain' argument (body).");
    sendBadRequestResponse();
    return;
  }

  String requestBody = _server->arg("plain");
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Request body: %s", requestBody.c_str());

  JsonDocument doc; // Use StaticJsonDocument if size is predictable
  auto error = deserializeJson(doc, requestBody);
  if (error) {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_WARN,
                               "JSON parsing failed: %s", error.c_str());
    sendBadRequestResponse();
    return;
  }

  // Extract data from JSON
  const char *ssid_connect = doc["ssid"];
  const char *pass_connect = doc["password"]; // Might be null
  const char *input_connect = doc["code"];     // Might be null
  const char *username_connect = doc["username"]; // Might be null
  const char *service_pass_connect = doc["service_password"]; // Might be null

  // Log received data, masking passwords if desired
  WIFI_PROVISIONER_DEBUG_LOG(
      WIFI_PROVISIONER_LOG_INFO, "Received config - SSID: '%s', Pass: '%s', Code: '%s', User: '%s', SrvPass: '%s'",
      ssid_connect ? ssid_connect : "NULL", pass_connect ? "***" : "NULL", // Mask password
      input_connect ? input_connect : "NULL", username_connect ? username_connect : "NULL",
      service_pass_connect ? "***" : "NULL"); // Mask password

  // --- Basic Validation ---
  if (!ssid_connect || strlen(ssid_connect) == 0) {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_WARN,
                               "SSID missing or empty in configure request.");
    sendBadRequestResponse(); // Send 400 Bad Request
    return;
  }

  // --- Connection Logic ---
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Disconnecting existing WiFi connection before attempting new one.");
  WiFi.disconnect(false, true); // Disconnect, keep AP mode, don't erase SDK creds yet
  delay(_wifiDelay);

  if (!connect(ssid_connect, pass_connect)) { // connect() handles logging internally
    handleUnsuccessfulConnection("ssid"); // Send failure response {success: false, reason: "ssid"}
    // Keep server running to allow user retry
    return;
  }

   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "WiFi connection successful to SSID: %s", ssid_connect);

  // --- Input Validation (if applicable) ---
  if (_config.SHOW_INPUT_FIELD && inputCheckCallback) {
     WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Performing input check for code: %s", input_connect ? input_connect : "NULL");
      // Pass empty string "" if input_connect is NULL or points to an empty string
      if (!inputCheckCallback(input_connect ? input_connect : "")) {
         WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_WARN,
                                   "Input check callback failed for device key.");
          handleUnsuccessfulConnection("code"); // Send {success: false, reason: "code"}
          WiFi.disconnect(false, true); // Disconnect WiFi if check fails
          // Keep server running
          return;
      }
      WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Input check successful.");
  } else {
       WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Input check skipped (not required, field hidden, or no callback).");
  }


  // --- Optional Service Login Validation ---
  // if (_config.SHOW_LOGIN_FIELDS && serviceLoginCheckCallback) {
  //    if (!serviceLoginCheckCallback(username_connect ? username_connect : "", service_pass_connect ? service_pass_connect : "")) {
  //       WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_WARN, "Service login check callback failed.");
  //       handleUnsuccessfulConnection("login"); // Send {success: false, reason: "login"}
  //       WiFi.disconnect(false, true);
  //       return;
  //    }
  //    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Service login check successful.");
  // }

  // --- Success ---
  handleSuccesfulConnection(); // Send {success: true} response to client immediately

   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Provisioning fully successful, calling onSuccess callback.");
  if (onSuccessCallback) {
    onSuccessCallback(ssid_connect, pass_connect, input_connect,
                      username_connect, service_pass_connect);
  }

  // --- Stop Provisioning Process ---
   // Short delay can sometimes help client process the {success:true} before server shuts down fully
   delay(500);

  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Provisioning complete. Setting loop flag to stop server.");
  _serverLoopFlag = true; // Signal loop() to exit and release resources
}


bool WiFiProvisioner::connect(const char *ssid, const char *password) {
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO,
                             "Attempting to connect to SSID: '%s'", ssid ? ssid : "NULL");

  if (!ssid || strlen(ssid) == 0) {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR,
                               "connect() called with invalid SSID.");
    return false;
  }

   // Ensure mode is STA or AP+STA. If only AP, switch to STA.
   wifi_mode_t currentMode = WiFi.getMode();
   if (currentMode == WIFI_OFF || currentMode == WIFI_AP) {
        WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Setting WiFi mode to WIFI_STA for connection attempt.");
        if (!WiFi.mode(WIFI_STA)) {
             WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR, "Failed to set WiFi mode to STA.");
             return false;
        }
        delay(_wifiDelay);
   } else {
        WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "WiFi mode is already STA or AP+STA (%d).", currentMode);
   }


  // Use WiFi.begin(ssid, password) which handles NULL or empty password correctly
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Calling WiFi.begin()...");
   WiFi.begin(ssid, password ? password : ""); // Pass empty string if password is NULL

  // Wait for connection with timeout
  unsigned long startTime = millis();
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Waiting up to %dms for WiFi connection...", _wifiConnectionTimeout);
  int lastStatus = -1; // Track status changes

  while (WiFi.status() != WL_CONNECTED) {
     int currentStatus = WiFi.status();
     if(currentStatus != lastStatus) {
         #ifdef WIFI_PROVISIONER_DEBUG // Only print status changes if debugging
         const char* statusStr = "";
         switch(currentStatus) {
             case WL_NO_SHIELD: statusStr = "WL_NO_SHIELD"; break;
             case WL_IDLE_STATUS: statusStr = "WL_IDLE_STATUS"; break;
             case WL_NO_SSID_AVAIL: statusStr = "WL_NO_SSID_AVAIL"; break;
             case WL_SCAN_COMPLETED: statusStr = "WL_SCAN_COMPLETED"; break;
             case WL_CONNECTED: statusStr = "WL_CONNECTED"; break; // Should not print here, but include
             case WL_CONNECT_FAILED: statusStr = "WL_CONNECT_FAILED"; break;
             case WL_CONNECTION_LOST: statusStr = "WL_CONNECTION_LOST"; break;
             case WL_DISCONNECTED: statusStr = "WL_DISCONNECTED"; break;
             default: statusStr = "UNKNOWN"; break;
         }
         WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "WiFi Status: %d (%s)", currentStatus, statusStr);
         #endif
         lastStatus = currentStatus;
     }

     // Check for permanent failures
     if (currentStatus == WL_NO_SSID_AVAIL || currentStatus == WL_CONNECT_FAILED) {
         WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR, "WiFi connection failed permanently. Status: %d", currentStatus);
         // Don't disconnect here, let the caller handle it based on failure reason
         return false;
     }

    // Check timeout
    if (millis() - startTime >= _wifiConnectionTimeout) {
      WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR,
                                 "WiFi connection timeout reached for SSID: '%s'. Last Status: %d",
                                 ssid, currentStatus);
      // Don't disconnect here, let the caller handle it
      return false;
    }
     delay(500); // Check status roughly twice per second
     yield(); // Allow background tasks
  }

  // Connected successfully
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO,
                             "Successfully connected to SSID: '%s'. IP Address: %s", ssid, WiFi.localIP().toString().c_str());
  return true;
}


void WiFiProvisioner::sendBadRequestResponse() {
  WiFiClient client = _server->client();
   if (!client) {
        WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR, "No client for Bad Request response.");
        return;
   }

  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_WARN, "Sending 400 Bad Request response.");
  client.println("HTTP/1.1 400 Bad Request");
  client.println("Content-Type: text/plain");
  client.println("Cache-Control: no-cache, no-store, must-revalidate");
  client.println("Pragma: no-cache");
  client.println("Expires: -1");
  client.println("Connection: close");
  client.println("Content-Length: 12"); // Length of "Bad Request\n"
  client.println();
  client.println("Bad Request"); // Simple body

  client.stop();
}


void WiFiProvisioner::handleSuccesfulConnection() {
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Sending successful connection response {success: true}.");
  JsonDocument doc; // Small JSON, StaticJsonDocument<32> would work
  doc["success"] = true;

  WiFiClient client = _server->client();
   if (!client) {
        WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR, "No client for successful connection response.");
        return;
   }

   size_t jsonLength = measureJson(doc);
   sendStandardHeaders(client, 200, "application/json");
   client.print("Content-Length: "); client.println(jsonLength);
   client.println(); // End headers
   serializeJson(doc, client);

  // Client will see {success: true} and display its own success page
  client.stop();
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Successful connection response sent.");
}


void WiFiProvisioner::handleUnsuccessfulConnection(const char *reason) {
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_WARN, "Sending unsuccessful connection response {success: false, reason: %s}.", reason ? reason : "unknown");
  JsonDocument doc; // StaticJsonDocument<64> likely enough
  doc["success"] = false;
  doc["reason"] = reason ? reason : "unknown";

  WiFiClient client = _server->client();
   if (!client) {
        WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR, "No client for unsuccessful connection response.");
        return;
   }

  size_t jsonLength = measureJson(doc);
  sendStandardHeaders(client, 200, "application/json"); // Still 200 OK, payload indicates error
  client.print("Content-Length: "); client.println(jsonLength);
  client.println(); // End headers
  serializeJson(doc, client);

  client.stop(); // Client JS should handle showing error based on reason
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Unsuccessful connection response sent.");

  // Optionally disconnect WiFi if connection failed due to bad credentials etc.
  // WiFi.disconnect(false, true); // Keep AP running for retry
}


void WiFiProvisioner::handleResetRequest() {
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Handling factory reset request '/factoryreset'.");
  if (factoryResetCallback) {
     WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Calling factory reset callback.");
    factoryResetCallback(); // Execute user-defined reset actions (e.g., clear preferences)
     WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Factory reset callback executed.");
  } else {
       WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_WARN, "Factory reset requested, but no callback registered.");
  }


  WiFiClient client = _server->client();
   if (!client) {
       WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_ERROR, "No client for factory reset response.");
       return;
   }
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Sending factory reset success response.");

  // Send a simple 200 OK response to acknowledge the reset was triggered
  sendStandardHeaders(client, 200, "text/plain");
  client.println("Content-Length: 13"); // "Reset Success"
  client.println();
  client.print("Reset Success");

  client.stop();
  WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Factory reset response sent.");

  // It's generally recommended to restart the ESP32 after a factory reset
  // You might want a small delay before restarting
   WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Restarting device after factory reset in 1 second...");
   delay(5000);
   ESP.restart();
}

// Sends a 204 No Content response for favicon requests to prevent errors
void WiFiProvisioner::handleFaviconRequest() {
    WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_INFO, "Handling favicon request '/favicon.ico'.");
    WiFiClient client = _server->client();
    if (client) {
        client.println("HTTP/1.1 204 No Content");
        client.println("Connection: close");
        client.println();
        client.stop();
        WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_DEBUG, "Sent 204 No Content for favicon.");
    } else {
        WIFI_PROVISIONER_DEBUG_LOG(WIFI_PROVISIONER_LOG_WARN, "No client for favicon request.");
    }
}
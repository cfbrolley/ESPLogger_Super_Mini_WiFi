const char SPLASH[] PROGMEM = R"=====(
  <!DOCTYPE html> 
  <html>
  <head>
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">
  <title>ESP32 Data Logger</title>
  <style>
    html {
         font-family: Verdana; 
         display: inline-block; 
         margin: 0px auto; 
         text-align: center; 
         border: 5px solid black;
         } 
    #grad1 {
           height: 650px; 
           background-color: dimgray; 
           background-image: linear-gradient(red, firebrick, dimgray);
           }
    body{
        margin-top: 50px;
        } 
        h1 {
           color: white;margin: 50px auto 30px;
           }
    p {
      font-size: 18px;
      color: white;
      margin-bottom: 5px;
      }
  </style>
  </head>
  <body id=grad1>
  <div id=\"webpage\">
  <h1>
    Rolley Rocketry
  </h1>
  <h2>
    ESP Logger v0.1
  </h2>
  <h2>
    Start logging:
  </h2>
  <a href='/startLogging'>
    Start Logging
  </a>
  </div>
  </body>
  </html>
)=====";
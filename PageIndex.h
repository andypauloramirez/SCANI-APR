const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <title>SCANI MI INVERNADERO</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <style>
      html {font-family: Arial; display: inline-block; text-align: center;}
      p {font-size: 1.2rem;}
      body {margin: 0;}
      .topnav {overflow: hidden; background-color: #2ECC71; color: black; font-size: 1.7rem;}
      .content {padding: 10px; }
      .card {background-color: white; box-shadow: 0px 0px 10px 1px rgba(140,140,140,.5); border: 1px solid #2ECC71; border-radius: 15px;}
      .card.header {background-color: #2ECC71; color: black; border-bottom-right-radius: 0px; border-bottom-left-radius: 0px; border-top-right-radius: 12px; border-top-left-radius: 12px;}
      .cards {max-width: 1200px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));}
      .reading {font-size: 2.8rem;}
      .packet {color: #bebebe;}
      .temperatureColor {color: #fd7e14;}
      .conductivityColor {color: #b42f2f;}
      .humidityColor {color: #1b78e2;}
      .phColor {color: #066e5f;}
      .uvColor {color: #8a0ecd;}
      .eco2Color {color: #7a7a7a;}
      .bateryColor{color: #000000;}
      .whiteColor{color: #ffffff;}

      .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
      .switch input {display: none}
      .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
      .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
      input:checked+.slider {background-color: #2ECC71}
      input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}

    </style>
  </head>
  
  <body>
  
    <div class="topnav">
      <h3 class="whiteColor">SCANI DASHBOARD</h3>
    </div>
    
    <br>

    %UPDATE1%
    %UPDATE2%
    %UPDATE3%
    %UPDATE4%
    %UPDATE5%
    %UPDATE6%
    %UPDATE7%
    
    <div class="content">
      <div class="cards">
         

        <div class="card">
            <div class="card header">
              <h2 class="whiteColor">Nodo A1</h2>
            </div>
            
            <br>
            
            <h4 class="temperatureColor">Temperatura del drenaje</h4>
            <p class="temperatureColor"><span class="reading"><span id="temp_2"></span> &deg;C</span></p>
            <h4 class="humidityColor">Humedad del sustrato</h4>
            <p class="humidityColor"><span class="reading"><span id="humsustrato"></span> &percnt;</span></p>
            <h4 class="conductivityColor">Conductividad del drenaje</h4>
            <p class="conductivityColor"><span class="reading"><span id="conductivity_2"></span> mS</span></p>
            <h4 class="bateryColor">Nivel bater&iacute;a</h4>
            <p class="bateryColor"><span class="reading"><span id="temp2"></span>&percnt;</span></p>

          </div>

        <div class="card">
            <div class="card header">
              <h2 class="whiteColor">Nodo A</h2>
            </div>
            
            <br>
            
            <h4 class="temperatureColor">Temperatura del ambiente</h4>
            <p class="temperatureColor"><span class="reading"><span id="temperambiente"></span> &deg;C</span></p>
            <h4 class="humidityColor">Humedad del del ambiente</h4>
            <p class="humidityColor"><span class="reading"><span id="humambiente"></span> &percnt;</span></p>
            <h4 class="uvColor">Radiaci&oacute;n UV</h4>
            <p class="uvColor"><span class="reading"><span id="u_v"></span> mW/cm<sup>2</sup></span></p>
            <h4 class="eco2Color">CO<sub>2</sub> equivalente</h4>
            <p class="eco2Color"><span class="reading"><span id="eco_2"></span> ppm</span></p>
            <h4 class="bateryColor">Nivel bater&iacute;a</h4>
            <p class="bateryColor"><span class="reading"><span id="temp2"></span>&percnt;</span></p>

        </div>

        <div class="card">
            <div class="card header">
              <h2 class="whiteColor">Tanque de Buffer</h2>
            </div>
            
            <br>
            
            <h4 class="temperatureColor">Temperatura de soluci&oacute;n buff</h4>
            <p class="temperatureColor"><span class="reading"><span id="temp_1"></span> &deg;C</span></p>
            <h4 class="phColor">Nivel de pH</h4>
            <p class="phColor"><span class="reading"><span id="p_H"></span> </span></p>
            <h4 class="conductivityColor">Conductividad de soluci&oacute;n buff</h4>
            <p class="conductivityColor"><span class="reading"><span id="conductivity_1"></span> mS</span></p>


        </div>
        `
      </div>
    </div>
    
    <script>

      document.getElementById("p_H").innerHTML = "NN"; 
      document.getElementById("temp_1").innerHTML = "NN"; 
      document.getElementById("temp_2").innerHTML = "NN";
      document.getElementById("conductivity_1").innerHTML = "NN"; 
      document.getElementById("conductivity_2").innerHTML = "NN";
      document.getElementById("eco_2").innerHTML = "NN";
      document.getElementById("u_v").innerHTML = "NN";
      document.getElementById("humsustrato").innerHTML = "NN"; 
      document.getElementById("temperambiente").innerHTML = "NN";
      document.getElementById("humambiente").innerHTML = "NN"; 
      //------------------------------------------------------------

      //------------------------------------------------------------
      if (!!window.EventSource) {
        var source = new EventSource('/events');
        
        source.addEventListener('open', function(e) {
          console.log("Events Connected");
        }, false);
        
        source.addEventListener('error', function(e) {
          if (e.target.readyState != EventSource.OPEN) {
            console.log("Events Disconnected");
          }
        }, false);
        
        source.addEventListener('message', function(e) {
          console.log("message", e.data);
        }, false);

        source.addEventListener('medicionesJSON', function(e) {
          console.log("medicionesJSON", e.data);
          
          var obj = JSON.parse(e.data);
          
            document.getElementById("p_H").innerHTML = obj.pH.toFixed(2);
            document.getElementById("temp_1").innerHTML = obj.Temperatura1.toFixed(2);
            document.getElementById("temp_2").innerHTML = obj.Temperatura2.toFixed(2);
            document.getElementById("conductivity_1").innerHTML = obj.Conductividad1.toFixed(2);
            document.getElementById("conductivity_2").innerHTML = obj.Conductividad2.toFixed(2);
            document.getElementById("eco_2").innerHTML = obj.eCO2.toFixed(2);
            document.getElementById("u_v").innerHTML = obj.uv.toFixed(2);
            document.getElementById("humsustrato").innerHTML = obj.Humedadsustrato.toFixed(2);
            document.getElementById("temperambiente").innerHTML = obj.Tempambiente.toFixed(2);
            document.getElementById("humambiente").innerHTML = obj.Humedadambiente.toFixed(2);
        }, false);

        source.addEventListener('estadosJSON', function(e) {
          console.log("estadosJSON", e.data);
          
          var obj = JSON.parse(e.data);
          
            document.getElementById("output1").checked = obj.EV1;
            if(obj.EV1){
              document.getElementById("outputState1").innerHTML = "ON";
            }
            else {
              document.getElementById("outputState1").innerHTML = "OFF";
            }
            
            document.getElementById("output2").checked = obj.EV2;
            if(obj.EV2){
              document.getElementById("outputState2").innerHTML = "ON";
            }
            else {
              document.getElementById("outputState2").innerHTML = "OFF";
            }

            document.getElementById("output3").checked = obj.EV3;
            if(obj.EV3){
              document.getElementById("outputState3").innerHTML = "ON";
            }
            else {
              document.getElementById("outputState3").innerHTML = "OFF";
            }

            document.getElementById("output4").checked = obj.EV4;
            if(obj.EV4){
              document.getElementById("outputState4").innerHTML = "ON";
            }
            else {
              document.getElementById("outputState4").innerHTML = "OFF";
            }
            
            document.getElementById("output5").checked = obj.WP1;
            if(obj.WP1){
              document.getElementById("outputState5").innerHTML = "ON";
            }
            else {
              document.getElementById("outputState5").innerHTML = "OFF";
            }
            
            document.getElementById("output6").checked = obj.WP2;
            if(obj.WP2){
              document.getElementById("outputState6").innerHTML = "ON";
            }
            else {
              document.getElementById("outputState6").innerHTML = "OFF";
            }
            
            document.getElementById("output7").checked = obj.AP;
            if(obj.AP){
              document.getElementById("outputState7").innerHTML = "ON";
            }
            else {
              document.getElementById("outputState7").innerHTML = "OFF";
            }
            
        }, false);
   
      }

//-------------------------BOTONES SLIDERS-----------------------------------
      function toggleCheckbox(element, ID) {
        var xhr = new XMLHttpRequest();
        switch(ID){
          case 1:
            if(element.checked){ xhr.open("GET", "/update1?state=1", true); }
            else { xhr.open("GET", "/update1?state=0", true); }
            break;
          case 2:
            if(element.checked){ xhr.open("GET", "/update2?state=1", true); }
            else { xhr.open("GET", "/update2?state=0", true); }
            break;
          case 3:
            if(element.checked){ xhr.open("GET", "/update3?state=1", true); }
            else { xhr.open("GET", "/update3?state=0", true); }
            break;
          case 4:
            if(element.checked){ xhr.open("GET", "/update4?state=1", true); }
            else { xhr.open("GET", "/update4?state=0", true); }
            break;
          case 5:
            if(element.checked){ xhr.open("GET", "/update5?state=1", true); }
            else { xhr.open("GET", "/update5?state=0", true); }
            break;
          case 6:
            if(element.checked){ xhr.open("GET", "/update6?state=1", true); }
            else { xhr.open("GET", "/update6?state=0", true); }
            break;
          case 7:
            if(element.checked){ xhr.open("GET", "/update7?state=1", true); }
            else { xhr.open("GET", "/update7?state=0", true); }
            break;  
        }
        xhr.send();  
      }
     
    </script>
  </body>
</html>
)rawliteral";

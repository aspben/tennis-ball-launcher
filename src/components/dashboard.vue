<template>
  <h1>Ball launcher</h1>   

  <div v-if="this.service">
    <div class="slidecontainer">
      <div class="launcherProperty">Feed : {{convertFeed(feedSpeed)}} {{feedSpeed}} </div>
      <input type="range" min="0" max="180" v-model="feedSpeed" @change="editFeed(feedSpeed)" class="slider feedRange" id="myRange">
    </div>

    <div class="slidecontainer">
      <div class="launcherProperty">Speed : {{ballSpeed}}</div>
      <input type="range" min="0" max="180" v-model="ballSpeed" @change="editSpeed(ballSpeed)" class="slider oneToTen" id="myRange">
    </div>

    <div class="slidecontainer">
      <div class="launcherProperty">Spin : {{spinValue}}</div>
      <input type="range" min="0" max="180" v-model="spinValue" @change="editSpin(spinValue)" class="slider oneToTen" id="myRange">
    </div>

    <div class="slidecontainer onOffSlider">
      <div class="launcherProperty">Oscillate : {{oscillateValue === "1" ? true : false}}</div>
      <input type="range" min="0" max="1" step="1" v-model="oscillateValue" @change="editOscillate(oscillateValue)" class="slider trueFalse" id="myRange">
    </div>
  </div>
  

  <!--<div class="speech" @click="speechOn = !speechOn">
    <button v-if="!speechOn" >
      <svg xmlns="http://www.w3.org/2000/svg" width="36" height="36" viewBox="0 0 24 24"><path d="M19 11h-1.7c0 .74-.16 1.43-.43 2.05l1.23 1.23c.56-.98.9-2.09.9-3.28zm-4.02.17c0-.06.02-.11.02-.17V5c0-1.66-1.34-3-3-3S9 3.34 9 5v.18l5.98 5.99zM4.27 3L3 4.27l6.01 6.01V11c0 1.66 1.33 3 2.99 3 .22 0 .44-.03.65-.08l1.66 1.66c-.71.33-1.5.52-2.31.52-2.76 0-5.3-2.1-5.3-5.1H5c0 3.41 2.72 6.23 6 6.72V21h2v-3.28c.91-.13 1.77-.45 2.54-.9L19.73 21 21 19.73 4.27 3z"/></svg>
    </button>
    <button v-else>
      <svg xmlns="http://www.w3.org/2000/svg" width="36" height="36" viewBox="0 0 24 24"><path d="M12 14c1.66 0 2.99-1.34 2.99-3L15 5c0-1.66-1.34-3-3-3S9 3.34 9 5v6c0 1.66 1.34 3 3 3zm5.3-3c0 3-2.54 5.1-5.3 5.1S6.7 14 6.7 11H5c0 3.41 2.72 6.23 6 6.72V21h2v-3.28c3.28-.48 6-3.3 6-6.72h-1.7z"/></svg>
    </button>
  <div class="box">{{runtimeTranscription}}</div>
  </div>
  
  <p class="smallText">Dernière commande: {{sentences[0]}}</p>-->

  <button class="btn" @click="connectToLauncher">Connect Ball Launcher</button>
  <div>
    Server: {{server ? server.connected : null}} 

  </div>
  <div>
    Service: {{JSON.stringify(service)}}
  </div>


</template>

<script>
let SpeechRecognition =
  window.SpeechRecognition || window.webkitSpeechRecognition;
let recognition = SpeechRecognition ? new SpeechRecognition() : false;


export default {
  name: "OneAndOnly",

  data: () => ({
    runtimeTranscription: "",
    lang: "fr-CA",
    text: "",
    error: false,
    toggle: false,
    speaking: false,
    sentences: [],
    characteristic: null,
    device: null,
    server: null,
    service: null,
    feedSpeed: 0,
    ballSpeed:0,
    spinValue: 0,
    oscillateValue: 0,
    speechOn: false,
    speed: 5, // km/h
    delay: 2, // s entre chaque balle
    spin: 100, // RPM positif = topspin; négatif = backspin
    working: false, // is the ball launcher in operation
  }),

  mounted() {
    this.checkCompatibility();
    //this.startSpeechRecognition();
  },
  methods: {
    convertFeed(feed) {
      if( feed < 52 ) {
        return "stop"
      } else { 
        // between 52 and 180 the feed pass from 1 per 8 sec to 1 per second
        return `${Math.round((9 - ((feed - 52) / (180 - 52)  * 7 + 1))* 10) / 10} sec.`
        

      }
      
    },

      connectToLauncher() {

        navigator.bluetooth.requestDevice({
        filters: [{ services: ['ed24d43a-a623-11eb-bcbc-0242ac130002'] }],
      })
        .then(device => device.gatt.connect())
        .then(server => server.getPrimaryService('ed24d43a-a623-11eb-bcbc-0242ac130002'))
        .then(service => this.service = service)
        .then(() => {
          this.editFeed(30)
          this.editSpin(90)
        })        
        .catch(error => alert(`Oups error: ${error}`))       
  },


    async editFeed (newFeed) {
      let characteristic = await this.service.getCharacteristic("febe13d2-a623-11eb-bcbc-0242ac130002");
      await characteristic.writeValue(Uint8Array.of(newFeed));        
    },

    async editSpeed (newSpeed) {
      let characteristic = await this.service.getCharacteristic("0c60be4a-a624-11eb-bcbc-0242ac130002");
      await characteristic.writeValue(Uint8Array.of(newSpeed));
    },

    async editSpin (newSpin) {
        let characteristic = await this.service.getCharacteristic("1216429c-a624-11eb-bcbc-0242ac130002");
      await characteristic.writeValue(Uint8Array.of(newSpin));
    },

    async editOscillate (oscillateValue) {      
      let characteristic = await this.service.getCharacteristic("ac9fbcb2-ac32-11eb-8529-0242ac130003");
      await characteristic.writeValue(Uint8Array.of(oscillateValue));
    },

   

    processCommand(command) {
      let commandArray = command.split(" ");
      // check speech for the spin keyWord and edit Spin
      let spinCommand = commandArray.indexOf("spin");
      if (spinCommand != -1) {
        this.spin = commandArray[spinCommand + 1];
      }
      // check speech for the spin keyWord and edit Spin
      let speedCommand = commandArray.indexOf("vitesse");
      if (speedCommand != -1) {
        this.speed = commandArray[speedCommand + 1];
      }

      // check speech for the spin keyWord and edit Spin
      let delayCommand = commandArray.indexOf("délai");
      if (delayCommand != -1) {
        this.delay = commandArray[delayCommand + 1];
      }

      let startWords = ["go", "marche", "shoot"];
      let startCommand = commandArray.filter((value) =>
        startWords.includes(value)
      );
      if (startCommand.length > 0) {
        this.working = true;
        this.startLed()
      }

      let stopWords = ["stop", "Arrete", "pause"];
      let stopCommand = commandArray.filter((value) =>
        stopWords.includes(value)
      );
      if (stopCommand.length > 0) {
        this.working = false;
        this.stopLed()
      }
    },

    checkCompatibility() {
      if (!recognition) {
        this.error =
          "Speech Recognition is not available on this browser. Please use Chrome or Firefox";
      }
    },
    endSpeechRecognition() {
      recognition.stop();
      this.toggle = false;
      this.$emit("speechend", {
        sentences: this.sentences,
        text: this.sentences.join(". "),
      });
    },
    startSpeechRecognition() {
      if (!recognition) {
        this.error =
          "Speech Recognition is not available on this browser. Please use Chrome or Firefox";
        return false;
      }
      console.console.log("startSpeechRecognition has started");
      this.toggle = true;
      recognition.lang = this.lang;
      recognition.interimResults = true;

      recognition.addEventListener("speechstart", (event) => {
        this.speaking = true;
        console.console.log("I heard something");
      });

      recognition.addEventListener("speechend", (event) => {
        this.speaking = false;
      });

      recognition.addEventListener("result", (event) => {
        const text = Array.from(event.results)
          .map((result) => result[0])
          .map((result) => result.transcript)
          .join("");
        this.runtimeTranscription = text;
      });

      recognition.addEventListener("end", () => {
        if (this.runtimeTranscription !== "") {
          this.sentences = [this.runtimeTranscription, ...this.sentences];
          this.processCommand(this.runtimeTranscription);

          this.$emit(
            "update:text",
            `${this.text}${this.sentences.slice(-1)[0]}. `
          );
        }
        this.runtimeTranscription = "";
        recognition.stop();
        if (this.toggle) {
          // keep it going.
          recognition.start();
        }
      });
      recognition.start();
    },
  },
};
</script>


<style>

body {
    /*Removes the default margin preset by the browser*/
    margin: 0;
    min-height: 100vh;
    /* Used to vertically center <main> when <main>'s margin is set to auto */
    background-color: #b4c1cb;
    /* Image for texture and lightening effect on background */
    background-image: 
                linear-gradient(135deg, rgba(255,255,255,.5) 30%, rgba(0,0,0,.1)),
                url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAANCAYAAACZ3F9/AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsIAAA7CARUoSoAAAAEnSURBVChTPZJJcoQwDEXlqfv+d8s2m9wCsEX+kxGmXIA1/EEuP79/t5aNMYx37mLFWqu23I3VWrO1ltXazJ13rVHEoqiUwqFVFd2uRtr8zzkjx8yVZ1ZJdnVlUxQhfdPdFft8P3EOIntO0NScRAKJRBHNQIHqvC47zzMagkpexFvrL8UsejhtvfpJXZzzDTFR3eKTInR679F5PZpLQbs0h3kwElVQ0hgC2cCXW1XxjkNvS9rSQJaWLATpdVbIjkFhfYtiULPx62qKT7PmvJRbRXsEwh4Djbb7u0WIxsVtOai8IcM3zl7a/sw08jMZKkmDZObIzGLGCgzJ6JoANscNSsS1ZiRlUc42kDHpuY4uV6Ffj+N4rxPJqSHoypQbV0UxDNRDE0byD7qr9Go2pueNAAAAAElFTkSuQmCC');
                
}

main {
    margin: auto;
}

input[type="range"] {
    /* To remove default styles appplied by webkit browsers */
    -webkit-appearance: none; 
    width: 85vw;
    background: transparent;
    position: relative;
}   

input[type="range"].trueFalse {
    /* To remove default styles appplied by webkit browsers */
    -webkit-appearance: none; 
    width: 30vw;
    background: transparent;
    position: relative;
}   

.slidecontainer {
  margin: 40px 20px 40px 20px
}

.onOffSlider {
  display: flex;
  align-content: flex-start;
  justify-content:space-around ;
}

.launcherProperty {
  text-align: left;
  font-size:large;
  margin: 16px;
}

input[type="range"]:focus {
    /* To remove default (mostly blue) outline appplied by browsers on :focus */
    outline: 0;
}

/* SLIDER THUMB -> represented by the orange ball in the design */
/* Webkit based browsers (chrome, opera, ...) */
input[type="range"]::-webkit-slider-thumb {
     /* Removes the default styling */
    -webkit-appearance: none;

    width: 3em;
    height: 3em;
    margin: .5em 0;
    background: hsl(27, 98%, 50%);
    border-radius: 100%;

    /* The inset shadow gives depth and makes the thumb look 3 dimensional and the other one shows a subtle effect of the shadow it creates because of the light source coming from the top left of the page */
    box-shadow: 
                inset .8em .8em 5px -8px rgba(255, 255, 255, .4),
                inset -.4em -.4em 10px -3px rgba(0,0,0,.2),
                0.7em 0.5em 7px -0.5em rgba(0,0,0,.4);;

    cursor: pointer;
}

/*All the same stuff for firefox */
input[type=range]::-moz-range-thumb {
    width: 3em;
    height: 3em;
    margin: .5em 0;
    background: #fd8421;
    border-radius: 100%;
    box-shadow: 
                inset .8em .8em 5px -8px rgba(255, 255, 255, .4),
                inset -.4em -.4em 10px -3px rgba(0,0,0,.2),
                0.7em 0.5em 7px -0.5em rgba(0,0,0,.4);
    cursor: pointer;
    border: 0;
}


/* RUNNABLE TRACK -> represented by the indented carve in which the ball (thumb) moves */
/* Webkit browsers */
input[type="range"]::-webkit-slider-runnable-track {
    background: transparent;
    width: 100vw;
    height: 4em;
    padding: 0 1em;
    border-radius: 2em;

    /* These shadow are what create the skeumorphic indent on the track. The first one is dark to show places in the indent that light don't reach and the second one shows places where light meets thereby creating the illusion of depth */ 
    box-shadow: 
                inset .6em 1em 10px rgba(0,0,0,.15),
                inset -.6em -1em 10px rgba(255, 255, 255, .415);
}
/* For firefox */
input[type=range]::-moz-range-track {
    background: transparent;
    width: 100vw;
    height: 4em;
    padding: 0 1em;
    border-radius: 2em;
    box-shadow: 
                inset .6em 1em 10px rgba(0,0,0,.15),
                inset -.6em -1em 10px rgba(255, 255, 255, .415);
}

/* For the numbering in the range's design - doesn't show on firefox but shows on chrome and opera */
input[type="range"].oneToTen::before {
    content: "0 1 2 3 4 5 6 7 8 9";
    position: absolute;
    text-align: center;
    color: #c4c4c4;
    font-size: 1em;

    /*Serves to spread the letters to fit the whole width of the track */
    letter-spacing: 2.5vw;

    top: 0; bottom: 0;
    left: 0; right: 0;
    padding: 1.6em 0.6em 1.6em 0.6em;

    /* Makes the letters look embossed adding to the skeumorphic theme of the design */
    text-shadow: .5px .5px 0.9px rgba(0,0,0, .4);

    z-index: -1;
}

/* For the numbering in the range's design - doesn't show on firefox but shows on chrome and opera */
input[type="range"].feedRange::before {
    content: "stop 1 2 3 4 5 6 7 8";
    position: absolute;
    text-align: center;
    color: #c4c4c4;
    font-size: 1em;

    /*Serves to spread the letters to fit the whole width of the track */
    letter-spacing: 2.5vw;

    top: 0; bottom: 0;
    left: 0; right: 0;
    padding: 1.6em 0.6em 1.6em 0.6em;

    /* Makes the letters look embossed adding to the skeumorphic theme of the design */
    text-shadow: .5px .5px 0.9px rgba(0,0,0, .4);

    z-index: -1;
}

/* For the numbering in the range's design - doesn't show on firefox but shows on chrome and opera */


input[type="range"].trueFalse::before {
    position: absolute;
    text-align: center;
    color: #c4c4c4;
    font-size: 1em;

    /*Serves to spread the letters to fit the whole width of the track */
    letter-spacing: 2.5vw;

    top: 0; bottom: 0;
    left: 0; right: 0;
    padding: 1.6em 0.6em 1.6em 0.6em;
    
    

    /* Makes the letters look embossed adding to the skeumorphic theme of the design */
    text-shadow: .5px .5px 0.9px rgba(0,0,0, .4);

    z-index: -1;
}



.speech {
  display: flex;
  
}



.box {
  flex-grow: 1;
  border: 2px solid grey;
  height: 35px;
  border-radius: 8px;
}

.smallText {
  font-size: 8px;
  text-align: end;
}

.btn {

    background-color: #fd8421;
    border-radius: 20px;
    box-shadow: 
                inset .8em .8em 5px -8px rgba(255, 255, 255, .4),
                inset -.4em -.4em 10px -3px rgba(0,0,0,.2),
                0.7em 0.5em 7px -0.5em rgba(0,0,0,.4);
    cursor: pointer;
    border: 0;

  position: relative;
  display: block;
  margin: 30px auto;
  padding: 12px 24px;
  overflow: hidden;
  border-width: 0;
  outline: none;

  color: #ecf0f1;
  
  transition: background-color .3s;
}

.speech button { 
  display: inline-block;

    font-size: 36px;
    border: none;
    margin: 0;
    text-decoration: none;
    background: transparent;
    color: #ffffff;
    font-family: sans-serif;
    font-size: 1rem;
    cursor: pointer;
    text-align: center;
    transition: background 250ms ease-in-out, 
                transform 150ms ease;
    -webkit-appearance: none;
    -moz-appearance: none;

}

.btn:hover, .btn:focus {
  background-color: #27ae60;
}





</style>

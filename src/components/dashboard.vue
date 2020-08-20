<template>
  <h1>Ball launcher</h1>
  <svg
    v-if="!this.working"
    @click="this.working = true"
    xmlns="http://www.w3.org/2000/svg"
    height="48"
    viewBox="0 0 24 24"
    width="48"
  >
    <path d="M0 0h24v24H0z" fill="none" />
    <path d="M8 5v14l11-7z" />
  </svg>
  <svg
    v-if="this.working"
    @click="this.working = false"
    xmlns="http://www.w3.org/2000/svg"
    height="48"
    viewBox="0 0 24 24"
    width="48"
  >
    <path d="M0 0h24v24H0z" fill="none" />
    <path d="M6 19h4V5H6v14zm8-14v14h4V5h-4z" />
  </svg>

  <p>Vitesse = {{speed}}</p>
  <p>Délais = {{delay}}</p>
  <p>Spin = {{spin}}</p>
  <p>characteristic: {{characteristic}}</p>

  <div class="box">{{runtimeTranscription}}</div>
  <p class="smallText">Dernière commande: {{sentences[0]}}</p>

  <button @click="connectToLauncher">Connect Ball Launcher</button>
  <button @click="startLed">Start</button>
  <button @click="stopLed">Stop</button>
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
    service: null,

    speed: 5, // km/h
    delay: 2, // s entre chaque balle
    spin: 100, // RPM positif = topspin; négatif = backspin
    working: false, // is the ball launcher in operation
  }),

  mounted() {
    this.checkCompatibility();
    this.startSpeechRecognition();
  },
  methods: {
    async connectToLauncher() {
      let device = await navigator.bluetooth
        .requestDevice({
          filters: [
            {
              name: "LED",
            },
          ],
          optionalServices: ["19b10000-e8f2-537e-4f6c-d104768a1214"]
        })

        let server = await device.gatt.connect()
        this.service = await  server.getPrimaryService("19b10000-e8f2-537e-4f6c-d104768a1214");
        
    },

    async startLed () {
      this.characteristic = await this.service.getCharacteristic("19b10001-e8f2-537e-4f6c-d104768a1214" );
          var newValue = Uint8Array.of(1);
        this.characteristic.writeValue(newValue);
        console.log('green on')

        this.characteristic = await this.service.getCharacteristic("19b10002-e8f2-537e-4f6c-d104768a1214" );
          var newValue = Uint8Array.of(0);
        this.characteristic.writeValue(newValue);
        console.log('red Off')
    },

    async stopLed () {
      this.characteristic = await this.service.getCharacteristic("19b10001-e8f2-537e-4f6c-d104768a1214" );
          var newValue = Uint8Array.of(0);
        this.characteristic.writeValue(newValue);
        console.log('green Off')

        this.characteristic = await this.service.getCharacteristic("19b10002-e8f2-537e-4f6c-d104768a1214" );
          var newValue = Uint8Array.of(1);
        this.characteristic.writeValue(newValue);
        console.log('red On')
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
      console.log("startSpeechRecognition has started");
      this.toggle = true;
      recognition.lang = this.lang;
      recognition.interimResults = true;

      recognition.addEventListener("speechstart", (event) => {
        this.speaking = true;
        console.log("I heard something");
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
.box {
  border: 2px solid grey;
  height: 35px;
  border-radius: 8px;
}
.smallText {
  font-size: 8px;
  text-align: end;
}
</style>

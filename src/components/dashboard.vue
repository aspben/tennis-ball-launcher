<template>
  <h1>Ball launcher</h1>
  <p>{{runtimeTranscription}}</p>
  <p>Dermière commande: {{sentences[-1]}}</p>
  <p>En fonction: {{working}}</p>
  <p>Vitesse = {{speed}}</p>
  <p>Délais = {{delay}}</p>
  <p>Spin = {{spin}}</p>

  <button @click="connectToLauncher">Connect Ball Launcher</button>
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
    connectToLauncher() {
      navigator.bluetooth
        .requestDevice({
          acceptAllDevices: true,
        })
        .then((device) => {
          alert("work Somehow");
          alert(device);
        })
        .catch((error) => {
          alert(error);
        });
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
      }

      let stopWords = ["stop", "Arrete", "pause"];
      let stopCommand = commandArray.filter((value) =>
        stopWords.includes(value)
      );
      if (stopCommand.length > 0) {
        this.working = false;
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
          this.sentences = [...this.sentences, this.runtimeTranscription];
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

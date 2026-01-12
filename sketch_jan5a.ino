#include <WiFi.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>


// ===== WLAN Einstellungen =====
const char* ssid = "ESP32-UNO";
const char* password = "12345678";


// ===== Server & WebSocket =====
WebServer server(80);
WebSocketsServer webSocket(81);


// ===== HTML-Seite =====
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 UNO Debug</title>
<style>
    
/* ===== Reset & Base ===== */
* {
    box-sizing: border-box;
    font-family: "Segoe UI", Roboto, Arial, sans-serif;
}

/* ===== Body ===== */
body {
    margin: 0;
    padding: 0;
    background: radial-gradient(circle at top, #0f2027, #000);
    color: #e0e0e0;
    display: flex;
    flex-direction: column;
    align-items: center;
    height: 100vh;
}

.game-Container {
    position: relative;
    width: 97%;
    height: 100%;
    background: rgba(20, 20, 30, 1);
    margin: 14px;
    border-radius: 14px;
    box-shadow: 0 0 30px rgba(0, 255, 255, 0.55);
}

/* ===== game-Window ===== */
.game-Window {
    position: absolute;
    display: flex;
    flex-direction: column;
    width: 100%;
    height: calc(100vh - 28px);
    background: rgba(20, 20, 30, 1);
    border-radius: 14px;
    align-items: center;
    justify-content: center;
    gap: 10px;
}

/* ===== viewer-Window ===== */
.viewer-Window {
    position: absolute;
    display: flex;
    width: 100%;
    height: calc(100vh - 28px);
    background: rgba(20, 20, 30, 1);
    border-radius: 14px;
    align-items: center;
    justify-content: center;
    gap: 10px;
}

/* ===== end-Window ===== */
.end-Window {
    position: absolute;
    display: flex;
    width: 100%;
    height: calc(100vh - 28px);
    background: rgba(20, 20, 30, 1);
    border-radius: 14px;
    color: #fff;
    padding: 30px 40px;
    text-align: center;
    font-family: Arial, sans-serif;
    flex-direction: column;
    justify-content: center;
    align-items: center;
}

.end-Window button {
    margin-top: 20px;
    padding: 10px 20px;
    border: flex;
    border-radius: 8px;
    background-color: #4a4aff;
    color: white;
    font-weight: bold;
    cursor: pointer;
    transition: background-color 0.2s;
}

.end-Window button:hover {
    background-color: #6565ff;
}

.end-Panel {
    display: flex;
    width: 50%;
    height: 50%;
    margin-bottom: 20%;
    background-color: #020617;
    border-radius: 14px;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    box-shadow: 0 0 30px rgba(0, 255, 255, 0.15);

}

.ranking-container {
    display: flex;
    flex-direction: column;
    gap: 12px;
    margin-top: 20px;
}

.ranking-row {
    background: #1e293b;
    color: white;
    padding: 12px 16px;
    border-radius: 10px;
    font-size: 1.1rem;

    transform: translateY(40px);
    opacity: 0;
    animation: slideUp 0.5s ease-out forwards;
}

.ranking-row.gold {
    background: linear-gradient(135deg, #ffd700, #ffae00);
    color: black;
    font-weight: bold;
}

@keyframes slideUp {
    to {
        transform: translateY(0);
        opacity: 1;
    }
}


/* ===== readyup-Window ===== */
.readyup-Window {
    position: absolute;
    display: flex;
    width: 100%;
    height: calc(100vh - 28px);
    background: rgba(20, 20, 30, 1);
    border-radius: 14px;
    align-items: center;
    justify-content: center;
    gap: 10px;
}
.readyup-Main {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;

    height: 100%;
    width: 60%;
    gap: 10px;

}


.readyup-Side {
    width: 40%;
}

.readyup-Line {
    width: 2px; /* Dicke der Linie */
    background-color: rgba(255, 255, 255, 0.2); /* Farbe/Transparenz */
    height: 80%; /* Höhe der Linie relativ zum Fenster */
    margin: 0 10px; /* Abstand zu Main & Side */

}

.playerreadyLine {
    width: 88%;
    height: 1px;
    background-color: rgba(255, 255, 255, 0.1);
    margin: 6px 0;
}


/* ===== einlog-Window ===== */
.einlog-Window {
    
    position: absolute;
    display: flex;
    width: 100%;
    height: calc(100vh - 28px);
    border-radius: 14px;
    align-items: center;
    justify-content: center;
    gap: 10px;
}

.einlog-Label {
    font-size: 16px;
    margin-bottom: 8px;
    font-weight: bold;
    font-style: italic;
}

.einlog-Textarea {
    box-shadow: 0 0 30px rgba(0, 255, 255, 0.55);
    border-radius: 8px;
    border: 1px solid #334155;
    background: #020617;
    padding: 10px;
    color: #94a3b8;
    font-size: 14px;
    width: 200px;
}

.einlog-Button {
    margin-left: 10px;
    padding: 8px 16px;
    border-radius: 8px;
    border: none;
    font-weight: 600;
    cursor: pointer;
    transition: transform 0.1s ease, box-shadow 0.1s ease, background 0.2s;
    font-size: large;
}

.einlog-Button:hover {
    background: rgba(0, 255, 255, 0.55);
    box-shadow: 0 0 30px rgba(0, 255, 255, 0.55);
    transform: translateY(-2px);
}

.debugLog {
    width: 250px;
    height: 120px;
    resize: none;
    border-radius: 12px;
    border: 1px solid #334155;
    background: #020617;
    color: #94a3b8;
    padding: 10px;
    font-size: 12px;
    box-shadow: inset 0 0 10px rgba(0, 0, 0, 0.6);

    position: fixed;
    bottom: 10px;
    left: 10px;
    z-index: 9999;
}





/* ===== Button Panel ===== */
.button-panel {
    display: flex;
    align-items: center;
    gap: 10px;
    margin-bottom: 12px;
}

/* ===== Buttons ===== */
button {
    padding: 8px 16px;
    border-radius: 8px;
    border: none;
    font-weight: 600;
    cursor: pointer;
    transition: transform 0.1s ease, box-shadow 0.1s ease, background 0.2s;
}

.decks-Panel {
    display: flex;
    flex-direction: row;
    margin-top: 10px;
    align-items: center;
    justify-content: center;
    border-radius: 12px;
    width: 100%;
    gap: 17%;
}

.draw-Deck, .discard-Deck {
    display: flex;
    align-items: center;
    justify-content: center;
    background: linear-gradient(135deg, #f59e0b, #b45309);
    border: 2px solid #fbbf24;
    border-radius: 12px;
    width: 140px;
    height: 180px;
    box-shadow: 0 0 15px rgba(245, 158, 11, 0.5);
}

.draw-Deck:hover, .discard-Deck:hover {
    box-shadow: 0 0 20px rgba(245, 158, 11, 0.8);
    transform: translateY(-2px);
}

.draw-Deck {
    cursor: pointer;
}

.card {
    width: 80px;
    height: 120px;
    background: #1e293b;
    border-radius: 10px;
    color: white;
    border: 2px solid #475569;
    display: flex;
    align-items: center;
    justify-content: center;
    font-weight: bold;
    cursor: default;
}

.card-clickable {
    cursor: pointer;
}

.card-clickable:hover {
    transform: translateY(-5px);
    box-shadow: 0 5px 15px rgba(0,0,0,0.6);
}

.card-disabled {
    opacity: 0.4;
}

.discardDeck-Card,
.draw-Deck-Count {
    width: 60px;
    height: 90px;
    border: 2px solid white;
    display: flex;
    align-items: center;
    justify-content: center;
    font-size: 24px;
}
.discardDeck-Card {
    border-radius: 10px;
    box-shadow: 0 0 15px rgba(239, 68, 68, 0.5);
}
.draw-Deck-Count {
    background: linear-gradient(135deg, #1e293b, #0f172a);
    color: white;
    border-radius: 10px;
}
.draw-Deck-Count:hover {
    box-shadow: 0 0 20px rgba(30, 41, 59, 0.8);
    transform: translateY(-2px);
}

.player-Cards {
    display: flex;
    flex-direction: row;
    gap: 8px;
    flex-wrap: wrap;
    justify-content: center;
    margin-top: 14px;
    overflow-x: auto;

}


/* Overlay behind the modal */
#playerSelectPanelOverlay {
    display: none;
    position: fixed;
    top: 0; left: 0;
    width: 100vw; height: 100vh;
    background: rgba(0, 0, 0, 0.6); /* semi-transparent black */
    z-index: 999;
}

/* The modal panel itself */
#playerSelectPanel {
    display: none;
    position: fixed;
    top: 50%; left: 50%;
    transform: translate(-50%, -50%);
    background: #1e1e2f; /* dark background */
    color: #ffffff;
    padding: 25px 30px;
    border-radius: 12px;
    box-shadow: 0 8px 20px rgba(0,0,0,0.4);
    text-align: center;
    z-index: 1000;
    min-width: 280px;
    font-family: Arial, sans-serif;
}

/* Title of the modal */
#playerSelectTitle {
    font-size: 18px;
    margin-bottom: 15px;
    font-weight: bold;
}

/* Container for the buttons */
#playerButtons {
    display: flex;
    flex-wrap: wrap;
    justify-content: center;
    gap: 10px;
    margin-bottom: 15px;
}

/* Player buttons */
#playerButtons button {
    padding: 8px 15px;
    border: none;
    border-radius: 8px;
    background-color: #4a4aff;
    color: white;
    cursor: pointer;
    font-weight: bold;
    transition: background-color 0.2s, transform 0.2s;
}

#playerButtons button:hover {
    background-color: #6565ff;
    transform: scale(1.05);
}

/* Cancel button */
#playerSelectPanel button.cancel-btn {
    background-color: #ff4a4a;
}

#playerSelectPanel button.cancel-btn:hover {
    background-color: #ff6a6a;
}

/* Overlay und Modal können die gleiche Klasse wie beim Spielerpanel nutzen */
#colorSelectPanelOverlay {
    display: none;
    position: fixed;
    top:0; left:0;
    width:100vw; height:100vh;
    background: rgba(0,0,0,0.6);
    z-index: 999;
}

#colorSelectPanel {
    display: none;
    position: fixed;
    top:50%; left:50%;
    transform: translate(-50%, -50%);
    background: #1e1e2f;
    color: #fff;
    padding: 25px 30px;
    border-radius: 12px;
    box-shadow: 0 8px 20px rgba(0,0,0,0.4);
    text-align: center;
    z-index: 1000;
    min-width: 280px;
    font-family: Arial, sans-serif;
}

#colorButtons {
    display: flex;
    justify-content: center;
    gap: 10px;
    margin-bottom: 15px;
}

#colorButtons button {
    padding: 8px 15px;
    border: none;
    border-radius: 8px;
    background-color: #4a4aff;
    color: white;
    cursor: pointer;
    font-weight: bold;
    transition: background-color 0.2s, transform 0.2s;
}

#colorButtons button:hover {
    background-color: #6565ff;
    transform: scale(1.05);
}

#colorSelectPanel .cancel-btn {
    background-color: #ff4a4a;
}

#colorSelectPanel .cancel-btn:hover {
    background-color: #ff6a6a;
}

/* Container für alle Spieler */
#opponentCards {
    display: flex;
    flex-direction: column;
    gap: 10px;
    padding: 10px;
    font-family: Arial, sans-serif;
}

/* Einzelner Spieler */
.playerInfo {
    background-color: #1e1e2f;
    color: white;
    padding: 10px 15px;
    border-radius: 10px;
    box-shadow: 0 4px 8px rgba(0,0,0,0.3);
    display: flex;
    justify-content: space-between;
    align-items: center;
    transition: transform 0.2s, background-color 0.2s;
}

/* Hover-Effekt für Spielerpanel */
.playerInfo:hover {
    transform: scale(1.02);
    background-color: #2a2a40;
}

/* Highlight für Spieler am Zug */
.playerInfo.currentTurn {
    border: 2px solid #ffcc00;
    background-color: #333366;
    font-weight: bold;
}

/* Kleine Anzeige für letzte Karte */
.playerInfo .lastCard {
    background-color: #444;
    padding: 3px 8px;
    border-radius: 6px;
    font-weight: bold;
    margin-left: 10px;
    min-width: 50px;
    text-align: center;
}

/* ===== Top Right Floating Button ===== */
.top-right-btn {
    position: fixed;
    top: 18px;
    right: 24px;
    z-index: 9999;

    width: 36px;
    height: 36px;

    border-radius: 50%;
    border: 1px solid rgba(0, 255, 255, 0.25);

    background: rgba(20, 20, 30, 0.9);
    color: #8ff;

    font-size: 18px;
    font-weight: bold;

    cursor: pointer;

    display: flex;
    align-items: center;
    justify-content: center;

    box-shadow:
        0 0 6px rgba(0, 255, 255, 0.15),
        inset 0 0 6px rgba(0, 255, 255, 0.05);

    transition: 
        transform 0.15s ease,
        box-shadow 0.15s ease,
        background 0.15s ease;
}

.top-right-btn:hover {
    background: rgba(30, 30, 45, 1);
    box-shadow:
        0 0 12px rgba(0, 255, 255, 0.4),
        inset 0 0 8px rgba(0, 255, 255, 0.15);
    transform: scale(1.05);
}

.top-right-btn:active {
    transform: scale(0.95);
}

.turn-current {
    font-size: 24px;  /* Größe nur für aktuelle Nachricht */
    font-weight: bold;
    color: #fff;
}

.turn-last {
    font-size: 16px;  /* kleinere Größe für letzte Nachricht */
    color: #aaa;
    display: block;   /* sorgt dafür, dass letzte Nachricht unter currentMessage steht */
}


/* ===== iPad Ansicht ===== */
@media only screen and (min-width: 768px) and (max-width: 1024px) {

    .game-Window,
    .viewer-Window,
    .readyup-Window,
    .end-Window {
        width: 95%;
        height: calc(100vh - 20px);
        margin: 10px;
        border-radius: 12px;
    }

    .card {
        width: 50px;
        height: 70px;
        font-size: 14px;
    }

    #turnIndicator {
        font-size: 16px;
    }

    #discardDeckCard {
        width: 60px;
        height: 80px;
        font-size: 18px;
    }
}

/* ===== Smartphone Ansicht ===== */
@media only screen and (max-width: 767px) {

    body {
        flex-direction: column;
        align-items: center;
    }

    .game-Window,
    .viewer-Window,
    .readyup-Window,
    .end-Window {
        width: 100%;
        height: 100vh;
        margin: 0;
        border-radius: 0;
        box-shadow: none;
        padding: 5px;
    }

    .card {
        width: 40px;
        height: 60px;
        font-size: 12px;
    }

    #turnIndicator {
        font-size: 14px;
    }

    #discardDeckCard {
        width: 50px;
        height: 70px;
        font-size: 16px;
    }

    .playerReadyStatus,
    .playerInfo {
        font-size: 12px;
    }

    #readyupPlayers,
    #opponentCards {
        gap: 5px;
    }
}

</style>
</head>
<body>

  <div class="game-Container" id="gameContainer">

        
    <div class="game-Window" id="gameWindow">

      <div class="opponent-Cards" id="opponentCards"></div>
            
      <div class="turn-Panel" id="turnPanel">
        <div class="turn-Indicator" id="turnIndicator"></div>
        <div class="played-Card" id="playedCard"></div>
      </div>

      <div class="decks-Panel" id="decksPanel">
        <div class="draw-Deck" id="drawDeck" onclick="drawCard()">
          <div class="draw-Deck-Count" id="drawDeckCount"></div>
        </div>
        <div class="discard-Deck" id="discardDeck">
          <div class="discardDeck-Card" id="discardDeckCard"></div>
        </div>                
      </div>

      <div class="player-Cards" id="playerCards"></div>
    </div>
        
    <div class="viewer-Window" id="viewerWindow">

    </div>
        
    <div class="end-Window" id="endWindow">
      <div class="end-Panel" id="endPanel">
        <div id="winnerText" style="font-size: 24px; font-weight: bold; margin-bottom: 30px;"></div>
        <button id="endButton" onclick="SendFertig()">Zurück zum Menü</button>
      </div>
      <div id="rankingPlayer" class="ranking-container"></div>
    </div>
        
    <div class="readyup-Window" id="readyupWindow">
      <div class="readyup-Main" id="readyupMain">
        <button class="readyup-Button" id="readyupButton" onclick="ReadyUp()">Ready Up</button>
        <div class="readyup-Text" id="readyupText">Waiting for players to be ready...</div>
      </div>
      <div class="readyup-Line"></div>
      <div class="readyup-Side" id="readyupSide">
        <div class="readyup-Players" id="readyupPlayers"></div>
      </div>
    </div>

    <div class="einlog-Window" id="einlogWindow">
      <label for="einlogTextarea" class="einlog-Label" id="einlogLabel">Username:</label>
      <textarea placeholder="Enter your username here ..." class="einlog-Textarea" id="einlogTextarea" rows="1" maxlength="16" onkeydown="if(event.key==='Enter'){event.preventDefault(); login();}"></textarea>
      <button class="einlog-Button" id="einlogButton" onclick="login()">↩</button>
    </div>

  </div>

  <div>
    <textarea class="debugLog" id="debugLog"></textarea>
  </div>

  <div id="playerSelectPanelOverlay" onclick="closePlayerSelectPanel()"></div>

  <div id="playerSelectPanel">
    <div id="playerSelectTitle">Choose the player to skip</div>
    <div id="playerButtons"></div>
    <button class="cancel-btn" id="closePlayerSelectButton">Cancel</button>
  </div>

  <div id="colorSelectPanelOverlay" onclick="closeColorSelectPanel()"></div>

  <div id="colorSelectPanel">
    <div id="colorSelectTitle">Choose a color</div>
    <div id="colorButtons"></div>
    <button class="cancel-btn" id="closeColorSelectButton">Cancel</button>
  </div>

  <button id="topRightButton" class="top-right-btn" title="Reset / Menu" onclick="ChangeResetAll()">⟳</button>

<script>
document.addEventListener("DOMContentLoaded", function() {

    console.log("SCRIPT LOADED");

    let ws;

    function connectWebSocket() {
      ws = new WebSocket("ws://" + location.hostname + ":81");

      ws.onopen = () => console.log("WebSocket connected!");
      ws.onclose = () => {
        console.log("WebSocket closed!");
      };
      ws.onerror = (e) => console.log("WebSocket error:", e);

      ws.onmessage = handleMessage;
    }


  const debugLog = document.getElementById("debugLog");

  //Elements of gameWindow
  const opponentCards = document.getElementById("opponentCards");
  const turnIndicator = document.getElementById("turnIndicator");
  const drawDeckCount = document.getElementById("drawDeckCount");
  const discardDeckCard = document.getElementById("discardDeckCard");
  const playerCards = document.getElementById("playerCards");


  //Elements of readyupWindow
  const readyupPlayers = document.getElementById("readyupPlayers");

  //Elements of endWindow
  const winnerText = document.getElementById("winnerText");
  const endButton = document.getElementById("endButton");
  const rankingPlayer = document.getElementById("rankingPlayer");

  //Elements of viewerWindow
    
  //Elements of einlogWindow
  const einlogLabel = document.getElementById("einlogLabel");
  const einlogPanel = document.getElementById("einlogButton");
  const einlogTextarea = document.getElementById("einlogTextarea");

  //Windows
  const gameWindow =document.getElementById("gameWindow");
  const viewerWindow = document.getElementById("viewerWindow");
  const endWindow = document.getElementById("endWindow");
  const einlogWindow = document.getElementById("einlogWindow");
  const readyupWindow = document.getElementById("readyupWindow");

  (function() {
    const oldLog = console.log;
    console.log = function(...args) {
      oldLog.apply(console, args);
      if(debugLog) {
        debugLog.value += args.join(' ') + "\n";
        debugLog.scrollTop = debugLog.scrollHeight;
      }
    }

    const oldWarn = console.warn;
    console.warn = function(...args) {
      oldWarn.apply(console, args);
      if(debugLog) {
        debugLog.value += "WARN: " + args.join(' ') + "\n";
        debugLog.scrollTop = debugLog.scrollHeight;
      }
    }
  })();

  //gameAttributes

  let gamestarted = false;
  let playersReady = [];
  let playersOnline = 0;
  let players = [];
  let playerAtTurn = 0;
  let playerWhoIsAtTurn = -1;
  let lastcardPlayed = "";
  let drawDeckSize = 0;
  let FarbeWunsch = "";
  let lastTurnMessage = "";
  let lastfourCards = [];
  let letzterSpieler = -1;

  //playerAttributes

  let userName = "";
  let userID = "";
  let userNumber = 0;
  let playerHand = [];
  let isviewer = false;
  let ResetAll = false;

  //otherplayerAttributes

  let playersHands = [];


  function handleMessage(event) {
    let data;

    try { data = JSON.parse(event.data); }
        
    catch { console.warn("Non-JSON message:", event.data); return; }

    switch(data.type) {
      case "players": 
          updatePlayerList(data.Playerlist); 
          updatereadyupPlayers();
          break;

      case "playersReady": 
          updatePlayerReadyList(data.listReady); 
          updatereadyupPlayers();
          break;

      case "aktuelleHand": 
          playerHand = data.listHand; 
          console.log("Your hand:", playerHand); 
          updateHandPanel();
          break;

      case "aktuelleHandeSpieler":
          playersHands = data.listHands;
          updatePlayersHandPanels();
          playersHands.forEach(p => {
              console.log(`Player ${p.id} has ${p.handSize} cards`);
          });
          break;

      case "gameStart":
          gamestarted = true;
          changeWindow("game");
          console.log("Game has started");
          break;

      case "join_ok":
          console.log("Join successful");
          userNumber = data.playerNumber;
          userID = data.socketID;
          gamestarted = data.gameStarted;
          if (gamestarted) {
              changeWindow("viewer");
              isviewer = true;
          }
          else {
              changeWindow("readyup");
          }
          console.log(`Logged in as ${userName} | ID: ${userID} | Number: ${userNumber}`);
          break;

      case "gameEnd":
          gamestarted = false;
          console.log("Game has ended.");
          changeWindow("end");
          break;

      case "Username-missing":
          console.warn("Username missing");
          break;

      case "Game-full":
          console.warn("Game is full");
          break;

      case "renumber":
          userNumber = data.playerNumber;
          console.log(`Player number renumbered to: ${userNumber}`);
          break;

      case "Aussetzen":
          console.log("You have been skipped this turn.");
          break;
      
      case "NotYourTurn":
          console.log("It's not your turn.");
          break;
      
      case "InvalidCard":
          console.log("The card you played is invalid.");
          break;

      case "TargetMissing":
          console.log("Target player missing for special card.");
          break;

      case "InvalidTarget":
          console.log("Invalid target player.");
          break;

      case "YourT":
          console.log("It's your turn.");
          playerAtTurn = true;
          playerWhoIsAtTurn = userNumber;
          updateturnIndicator();
          updateHandPanel();
          updatePlayersHandPanels();
          break;

      case "NotYourT":
          console.log("It's not your turn.");
          playerAtTurn = false;
          playerWhoIsAtTurn = data.atTurn;
          updateturnIndicator();
          updateHandPanel();
          updatePlayersHandPanels();
          break;

      case "CardPlayed":
          drawDeckSize = data.drawDeckSize;
          drawDeckCount.textContent = drawDeckSize;
          lastcardPlayed = data.card;
          FarbeWunsch = data.wishFarbe;
          letzterSpieler = data.who;
          lastfourCards[letzterSpieler] = lastcardPlayed;
          updateLastCardPlayed();
          updatePlayersHandPanels();
          console.log(`Card played: ${data.card}`);
          break;

      case "restartLobby":
          gamestarted = false;
          playerAtTurn = 0;
          playerWhoIsAtTurn = -1;
          lastcardPlayed = "";
          drawDeckSize = 0;
          FarbeWunsch = "";
          lastTurnMessage = "";
          lastfourCards = [];
          letzterSpieler = -1;
          isviewer = false;
          playerHand = [];
          playersHands = [];
          playersReady = [];
          changeWindow("readyup");
          console.log("Returning to lobby for new game.");
          break;

      case "reset":
          players = [];
          playersOnline = 0;
          userName = "";
          userID = "";
          userNumber = 0;
          gamestarted = false;
          playerAtTurn = 0;
          playerWhoIsAtTurn = -1;
          lastcardPlayed = "";
          drawDeckSize = 0;
          FarbeWunsch = "";
          lastTurnMessage = "";
          lastfourCards = [];
          letzterSpieler = -1;
          isviewer = false;
          playerHand = [];
          playersHands = [];
          playersReady = [];
          ResetAll = false;
          console.log("Resetting client state.");
          changeWindow("login");
          ws.close();
          connectWebSocket();
          break;
        
      default: console.log("Message:", data); break;
    }
  }

  function changeWindow(targetWindow) {

    einlogWindow.style.display   = "none";
    viewerWindow.style.display  = "none";
    readyupWindow.style.display = "none";
    gameWindow.style.display    = "none";
    endWindow.style.display     = "none";

    if (targetWindow === "login") {
      einlogWindow.style.display = "flex";
    }

    if (targetWindow === "viewer") {
      viewerWindow.style.display = "flex";
    }

    if (targetWindow === "readyup") {
      readyupWindow.style.display = "flex";
      updatereadyupPlayers();
    }

    if (targetWindow === "game") {
      gameWindow.style.display = "flex";
    }

    if (targetWindow === "end") {
      endWindow.style.display = "flex";
      updateWinner();
    }
  }

  function updateWinner() {
    let winnerIndex = -1;
    for (let i = 0; i < playersHands.length; i++) {
      if (playersHands[i].handSize === 1) {
        winnerIndex = i;
        break;
      }
    }

    if (winnerIndex === -1) {
      winnerText.textContent = "No winner found!";
    } else {
      winnerText.textContent = `Player ${winnerIndex}(${players[winnerIndex]}) has won! 🏆`;
    }

    showAnimatedRanking(playersHands);
  }

  window.SendFertig = function() {
    ws.send(JSON.stringify({ type: "Fertig" }));
    console.log("für Fertig gestimmt");
  }

  function showAnimatedRanking(playersHands) {

    const rankingPlayer = document.getElementById("rankingPlayer");
    rankingPlayer.innerHTML = "<h3>Final Rankings:</h3>";

    const ranking = playersHands
      .slice()
      .sort((a, b) => a.handSize - b.handSize);

    ranking.forEach((player, index) => {
      const row = document.createElement("div");
      row.className = "ranking-row";

      if (index === 0) {
          row.classList.add("gold");
      }

      row.style.animationDelay = `${index * 0.15}s`;

      row.textContent = `#${index + 1} Player ${player.id} — ${player.handSize} cards`;

      rankingPlayer.appendChild(row);
    });
  }


  function updateLastCardPlayed() {
    if (!lastcardPlayed) {
      console.log("No last card played");
      return;
    }
    if (!discardDeckCard) {
      console.log("playedCard element not found");
      return;
    }

    discardDeckCard.style.color = "";
    discardDeckCard.style.backgroundColor = "";

    if (lastcardPlayed === "W" || lastcardPlayed === "+4W") {
      const bg = FarbeWunsch
          ? getColorCode(FarbeWunsch)
          : getColorCode("W");
      discardDeckCard.style.backgroundColor = bg;

      discardDeckCard.style.color =
          FarbeWunsch === 'Y' ? "black" : "white";

      discardDeckCard.textContent =
          lastcardPlayed === "+4W" ? "+4" : "W";
    }

    else {
        const colorChar = lastcardPlayed.slice(-1);

        discardDeckCard.textContent =
            lastcardPlayed.replace(colorChar, '');

        discardDeckCard.style.backgroundColor =
            getColorCode(colorChar);

        discardDeckCard.style.color =
            (colorChar === 'Y') ? "black" : "white";
    }


    console.log("Updated last played card:", lastcardPlayed);
  }

  function updateturnIndicator() {
    let currentMessageText;

    if (playerAtTurn) {
      currentMessageText = "Your turn!";
    } else {
      currentMessageText = players[playerWhoIsAtTurn] + "'s turn";
    }

    if (currentMessageText !== lastTurnMessage) {
      turnIndicator.innerHTML = 
        `<span class="turn-current">${currentMessageText}</span>` +
        (lastTurnMessage ? `<span class="turn-last">${lastTurnMessage}</span>` : "");
      lastTurnMessage = currentMessageText;
    }
  }



  function updatereadyupPlayers() {
    if (!readyupPlayers) {
      console.log("readyupPlayers not found");
      return;
    }

    readyupPlayers.innerHTML = "";

    players.forEach((player, index) => {
      const playerEl = document.createElement("div");
      playerEl.className = "playerReadyStatus";

      const ready = playersReady.includes(player);
      playerEl.textContent = `Player ${index}: ${player} ${ready ? "✅ Ready" : "❌ Not Ready"}`;

      readyupPlayers.appendChild(playerEl);

      const playerlineEl = document.createElement("hr");
      playerlineEl.className = "playerreadyLine";
      readyupPlayers.appendChild(playerlineEl);
    });
  }

  function updatePlayersHandPanels() {
    if (!opponentCards) {
      console.log("opponentCards not found");
      return;
    }

    opponentCards.innerHTML = "";

    players.forEach((player, index) => {
      const playerEl = document.createElement("div");
      playerEl.className = "playerInfo";

      if (index === playerWhoIsAtTurn) {
        playerEl.classList.add("currentTurn");
      }

      const leftDiv = document.createElement("div");
      leftDiv.textContent = `Player ${index}: ${playersHands[index].handSize} Cards`;

      const lastCardDiv = document.createElement("div");
      lastCardDiv.className = "lastCard";
      if (lastcardPlayed[index] === '+') {
        const next = lastcardPlayed[index + 1];
        lastCardDiv.textContent = next ? `+${next}` : "—";
      } else {
        lastCardDiv.textContent = lastcardPlayed[index] ?? "—";
      }
      playerEl.appendChild(leftDiv);
      playerEl.appendChild(lastCardDiv);

      opponentCards.appendChild(playerEl);
    });
  }


  function updatePlayerList(list) {
    players = list;
    playersOnline = list.length;
    console.log(`Players online: ${playersOnline}`);
    console.log("Player List:", players);
  }

  function updatePlayerReadyList(listReady) { 
    playersReady = listReady; 
    console.log("Players ready:", playersReady); 
  }

  function updateHandPanel() {

    playerCards.innerHTML = "";

    playerHand.forEach((card, index) => {

      const cardEl = document.createElement("div");
      cardEl.className = "card";

      const colorChar = card.slice(-1);
      const isWild = card === "W" || card === "+4W";

      if (isWild) {
        cardEl.textContent = card;
        cardEl.style.background =
            "linear-gradient(135deg, red, yellow, green, blue)";
        cardEl.style.color = "white";
      } else {
        cardEl.textContent = card.replace(colorChar, '');
        cardEl.style.backgroundColor = getColorCode(colorChar);
        cardEl.style.color = (colorChar === 'Y') ? "black" : "white";
      }

      if (playerAtTurn) {
        cardEl.classList.add("card-clickable");
        cardEl.onclick = () => handleCardClick(card, index);
      } else {
        cardEl.classList.add("card-disabled");
      }

      playerCards.appendChild(cardEl);
    });
  }


  function handleCardClick(card, index) {
    if (card === "W" || card === "+4W") {
        chooseColor(index);
        return;
    }

    if (card[0] === 'S') {
        chooseTargetPlayer(index);
        return;
    }

    // Normale Karte
    window.DropCard(index);
  }


  function chooseTargetPlayer(cardIndex) {
    const overlay = document.getElementById("playerSelectPanelOverlay");
    const panel = document.getElementById("playerSelectPanel");
    const buttonsDiv = document.getElementById("playerButtons");
    const closePlayerSelectButton = document.getElementById("closePlayerSelectButton"); 
    closePlayerSelectButton.onclick = closePlayerSelectPanel;

    buttonsDiv.innerHTML = "";

    for (let i = 0; i < players.length; i++) {
      if (i === userNumber) continue;

      const btn = document.createElement("button");
      btn.textContent = players[i];
      btn.onclick = () => {
        SkipPlayer(cardIndex, i);
        closePlayerSelectPanel();
      };
      buttonsDiv.appendChild(btn);
    }

    overlay.style.display = "block";
    panel.style.display = "block";
  }

  function closePlayerSelectPanel() {
    document.getElementById("playerSelectPanelOverlay").style.display = "none";
    document.getElementById("playerSelectPanel").style.display = "none";
  }

  function SkipPlayer(cardIndex, targetPlayerIndex) {
    window.DropCard(cardIndex, targetPlayerIndex);
  }

  function chooseColor(cardIndex) {
    const overlay = document.getElementById("colorSelectPanelOverlay");
    const panel = document.getElementById("colorSelectPanel");
    const buttonsDiv = document.getElementById("colorButtons");
    const closeColorSelectButton = document.getElementById("closeColorSelectButton"); 
    closeColorSelectButton.onclick = closeColorSelectPanel;

    buttonsDiv.innerHTML = "";

    const colors = ["R", "G", "B", "Y"];
    colors.forEach(color => {
      const btn = document.createElement("button");
      btn.textContent = color;
      btn.style.backgroundColor = getColorCode(color);
      btn.onclick = () => {
          SelectColor(cardIndex, color);
          closeColorSelectPanel();
      };
      buttonsDiv.appendChild(btn);
    });

    overlay.style.display = "block";
    panel.style.display = "block";
  }

  function closeColorSelectPanel() {
    document.getElementById("colorSelectPanelOverlay").style.display = "none";
    document.getElementById("colorSelectPanel").style.display = "none";
  }

  function SelectColor(cardIndex, color) {
    window.DropCard(cardIndex, color);
  }

  function getColorCode(c) {
    switch(c) {
      case "R": return "#ff4a4a";
      case "G": return "#4aff4a";
      case "B": return "#4a4aff";
      case "Y": return "#ffff4a";
      case "W": return "#1e293b";
    }
  }


  window.DropCard = function(cardIndex, wish) {
    const payload = { type: "selectCard", cardIndex: cardIndex };
    if (wish !== undefined) payload.wish = wish;
    ws.send(JSON.stringify(payload));
  };


  window.login = function() {
    const username = einlogTextarea.value.trim();
    if(!username) { 
      console.log("No username entered"); 
      return;
    }
    ws.send(JSON.stringify({
      type: "join",
      username: username
    }));
    userName = username;
    console.log("Logging in as:", username);
  }

  window.ReadyUp = function() { 
    ws.send(JSON.stringify({
      type:"ready"
    })); 
  }
    
  window.drawCard = function() { 
    ws.send(JSON.stringify({
      type:"drawCard"
    })); 
  }

  window.ChangeResetAll = function() { 
    ResetAll = !ResetAll;
    if (ResetAll) {
      console.log("Reset All activated");
      ws.send(JSON.stringify({
        type:"Reset"
      })); 
    }
    else {
      console.log("Reset All deactivated");
      ws.send(JSON.stringify({
        type:"NoReset"
      })); 
    }
  }

  debugLog.addEventListener("click", function() {
    debugLog.style.display = "none";
  });

  connectWebSocket();
  changeWindow("login");
});
</script>
</body>
</html>
)rawliteral";

enum BroadcastType {
  BROADCAST_PLAYERS,
  BROADCAST_READY,
  BROADCAST_GAMESTART,
  BROADCAST_PLAYERHANDS,
  BROADCAST_END,
  BROADCAST_DECK,
  BROADCAST_RESET,
  BROADCAST_RESTART
};

enum MsgType : uint8_t {
  MSG_UNKNOWN,
  MSG_JOIN,
  MSG_READY,
  MSG_DRAW,
  MSG_SELECT,
  MSG_FERTIG,
  MSG_RESET,
  MSG_NORESET,
  MSG_DATAREQ
};

const char* drawDeck[108] = { 
    "0R", "0G", "0B", "0Y", 
    "1R", "1R", "1G", "1G", "1B", "1B", "1Y", "1Y", 
    "2R", "2R", "2G", "2G", "2B", "2B", "2Y", "2Y",
    "3R", "3R", "3G", "3G", "3B", "3B", "3Y", "3Y", 
    "4R", "4R", "4G", "4G", "4B", "4B", "4Y", "4Y",
    "5R", "5R", "5G", "5G", "5B", "5B", "5Y", "5Y", 
    "6R", "6R", "6G", "6G", "6B", "6B", "6Y", "6Y",
    "7R", "7R", "7G", "7G", "7B", "7B", "7Y", "7Y", 
    "8R", "8R", "8G", "8G", "8B", "8B", "8Y", "8Y",
    "9R", "9R", "9G", "9G", "9B", "9B", "9Y", "9Y",
    "+2R", "+2R", "+2G", "+2G", "+2B", "+2B", "+2Y", "+2Y",
    "SR", "SR", "SG", "SG", "SB", "SB", "SY", "SY",
    "RR", "RR", "RG", "RG", "RB", "RB", "RY", "RY",
    "W", "W", "W", "W",
    "+4W", "+4W", "+4W", "+4W" };
uint8_t drawDeckSize = 108;

const char* discardPile[108];
uint8_t discardPileSize = 0;

struct Viewer {
  uint8_t socketID;
  char name[16];
  uint8_t number;
};

Viewer viewers[3];
uint8_t viewerCount = 0;


struct Player {
  uint8_t socketID;
  char name[16];
  uint8_t number;
  bool aussetzen = false;
  bool hatPlusKarte = false;
  bool isready = false;
  bool isFertig = false;
  bool isForReset = false;
  uint8_t handSize = 0;
  const char* hand[24];
};

Player players[4];
uint8_t playerCount = 0;

uint8_t aktuellerSpieler = 0;
int8_t spielerAmZug = -1;
bool uhrzeigersinn = true;
bool letzteKartePlusKarte = false;
uint8_t aktuelleZiehenAnzahl = 0;
bool gamestarted = false;
char aktuelleFarbe = 0;


void removePlayer(uint8_t index);
void broadcast(BroadcastType type);
void DrawCard(uint8_t socketID, uint8_t amount);
const char* drawRandomCard();
MsgType getMsgType(const char* t);
int8_t findPlayerBySocketID(uint8_t socketID);
void StartGame();
void handleJoin(uint8_t socketID, StaticJsonDocument<256>& doc);
void handleReady(uint8_t socketID, StaticJsonDocument<256>& doc);
void handleDraw(uint8_t socketID, StaticJsonDocument<256>& doc);
void handleSelectCard(uint8_t socketID, StaticJsonDocument<256>& doc);
void handleDATAREQ(uint8_t socketID, StaticJsonDocument<256>& doc);
bool isCardPlayable(const char* lastCard, const char* playedCard);
bool saveStats(StaticJsonDocument<1024> &doc);
bool loadStats(StaticJsonDocument<1024> &doc);
void resetDeck();
void saveStatsAfterGame();
void checkIfAllReady();
void updateSpielerZug(uint8_t num);
void checkIfAllAreForReset();
void handleNoReset(uint8_t socketID, StaticJsonDocument<256>& doc);
void handleReset(uint8_t socketID, StaticJsonDocument<256>& doc);
void ResetGame();
void checkIfAllAreFertig();
void handleFertig(uint8_t socketID, StaticJsonDocument<256>& doc);
void RestartGame();


void nextMove() {
  
  if (uhrzeigersinn) {
    aktuellerSpieler++;
    if (aktuellerSpieler >= playerCount) {
      aktuellerSpieler = 0;
    } 
  } 
  else {
    if (aktuellerSpieler == 0) {
      aktuellerSpieler = playerCount - 1;
    }
    else {
      aktuellerSpieler--;
    }
  }
  spielerAmZug = aktuellerSpieler;
  updateSpielerZug(spielerAmZug);
}


// ===== WebSocket Event Handler =====
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {

  switch (type) {

    case WStype_DISCONNECTED: {
      for (uint8_t i = 0; i < playerCount; i++) {
        if (players[i].socketID == num) {
          removePlayer(i);

          for (uint8_t j = 0; j < playerCount; j++) {
            StaticJsonDocument<64> res;
            res["type"] = "renumber";
            res["playerNumber"] = players[j].number;

            String msg;
            serializeJson(res, msg);
            webSocket.sendTXT(players[j].socketID, msg);
          }

          broadcast(BROADCAST_PLAYERS);
          break;
        }
      }
      break;
    }

    case WStype_TEXT: {
      StaticJsonDocument<256> doc;
      if (deserializeJson(doc, payload, length)) return;

      const char* msgType = doc["type"];
      switch (getMsgType(msgType)) {
        case MSG_JOIN:    handleJoin(num, doc); break;
        case MSG_READY:   handleReady(num, doc); break;
        case MSG_DRAW:    handleDraw(num, doc); break;
        case MSG_SELECT:  handleSelectCard(num, doc); break;
        case MSG_FERTIG:  handleFertig(num, doc); break;
        case MSG_RESET:   handleReset(num, doc); break;
        case MSG_NORESET: handleNoReset(num, doc); break;
        case MSG_DATAREQ: handleDATAREQ(num, doc); break;
        default: break;
      }
      break;
    }

    default:
      break;
  }
}

MsgType getMsgType(const char* t) {
  if (!t) return MSG_UNKNOWN;
  if (!strcmp(t, "join"))       return MSG_JOIN;
  if (!strcmp(t, "ready"))      return MSG_READY;
  if (!strcmp(t, "drawCard"))   return MSG_DRAW;
  if (!strcmp(t, "selectCard")) return MSG_SELECT;
  if (!strcmp(t, "Fertig"))     return MSG_FERTIG;
  if (!strcmp(t, "Reset"))      return MSG_RESET;
  if (!strcmp(t, "NoReset"))    return MSG_NORESET;
  if (!strcmp(t, "DataRequest"))return MSG_DATAREQ;
  return MSG_UNKNOWN;
}

void handleNoReset(uint8_t socketID, StaticJsonDocument<256>& doc) {

  int8_t index = findPlayerBySocketID(socketID);
  if (index == -1) return;

  players[index].isForReset = false;
}

void handleReset(uint8_t socketID, StaticJsonDocument<256>& doc) {

  int8_t index = findPlayerBySocketID(socketID);
  if (index == -1) return;

  players[index].isForReset = true;

  checkIfAllAreForReset();
}

void checkIfAllAreForReset(){
  uint8_t s = 0;
  for (uint8_t i = 0; i < playerCount; i++) {
    if (players[i].isForReset == true) {
      s++;
    }
  }

  if (s == playerCount){
    broadcast(BROADCAST_RESET);
    delay(50);
    ResetGame();
  }
}

void ResetGame() {

  // ===== ALLE SOCKETS TRENNEN =====
  for (uint8_t i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++) {
    webSocket.disconnect(i);
  }
  delay(100);

  // ===== Spielstatus =====
  gamestarted = false;

  spielerAmZug = -1;
  aktuellerSpieler = 0;
  uhrzeigersinn = true;

  letzteKartePlusKarte = false;
  aktuelleZiehenAnzahl = 0;
  aktuelleFarbe = 0;

  // ===== Decks =====
  resetDeck();
  discardPileSize = 0;

  // ===== Spieler & Viewer =====
  for (uint8_t i = 0; i < 4; i++) {
    players[i].handSize = 0;
    players[i].isready = false;
    players[i].isFertig = false;
    players[i].isForReset = false;
    players[i].aussetzen = false;
    players[i].hatPlusKarte = false;
  }

  playerCount = 0;
  viewerCount = 0;
}

void removePlayer(uint8_t index) {
  
  for (uint8_t i = index; i < playerCount - 1; i++) {
    players[i] = players[i + 1];
    players[i].number = i;
  }
  playerCount--;
}

void handleFertig(uint8_t socketID, StaticJsonDocument<256>& doc) {

  int8_t index = findPlayerBySocketID(socketID);
  if (index == -1) return;

  players[index].isFertig = true;

  checkIfAllAreFertig();
}

void checkIfAllAreFertig(){
  uint8_t s = 0;
  for (uint8_t i = 0; i < playerCount; i++) {
    if (players[i].isFertig == true) {
      s++;
    }
  }

  if (s == playerCount){
    broadcast(BROADCAST_RESTART);
    RestartGame();
  }
}

void RestartGame() {

  // ===== Reset global game state =====
  gamestarted = false;
  spielerAmZug = -1;
  aktuellerSpieler = 0;
  uhrzeigersinn = true;
  letzteKartePlusKarte = false;
  aktuelleZiehenAnzahl = 0;
  aktuelleFarbe = 0;

  // ===== Reset deck =====
  resetDeck();
  discardPileSize = 0;

  // ===== Reset players =====
  for (uint8_t i = 0; i < playerCount; i++) {
    players[i].handSize = 0;
    players[i].isready = false;
    players[i].isFertig = false;
    players[i].aussetzen = false;
    players[i].hatPlusKarte = false;
  }

  StaticJsonDocument<64> doc;
  doc["type"] = "restartLobby";

  String msg;
  serializeJson(doc, msg);
  webSocket.broadcastTXT(msg);

  broadcast(BROADCAST_PLAYERS);
  broadcast(BROADCAST_READY);
}

void sendError(uint8_t socketID, const char* msg) {
  
  webSocket.sendTXT(socketID, msg);
}

void handleSelectCard(uint8_t socketID, StaticJsonDocument<256>& doc) {

  int8_t index = findPlayerBySocketID(socketID);
  if (index == -1) return;

  if (spielerAmZug != players[index].number) {
    sendError(socketID, "NotYourTurn");
    return;
  }
  
  if (players[index].aussetzen) {
    players[index].aussetzen = false;
    nextMove();
    return;
  }

  if (!doc["cardIndex"].is<int>()) {
    sendError(socketID, "InvalidCard");
    return;
  }

  int cardIndex = doc["cardIndex"];
  if (cardIndex < 0 || cardIndex >= players[index].handSize) {
    sendError(socketID, "InvalidCard");
    return;
  }

  const char* playedCard = players[index].hand[cardIndex];
  JsonVariant wishVar = doc["wish"];

  if (letzteKartePlusKarte && playedCard[0] != '+') {
    sendError(socketID, "MustDraw");
    return;
  }

  const char* lastCard = discardPile[discardPileSize - 1];
  if (!isCardPlayable(lastCard, playedCard)) {
      sendError(socketID, "InvalidMove");
      return;
  }

  for (uint8_t i = cardIndex; i < players[index].handSize - 1; i++) {
    players[index].hand[i] = players[index].hand[i + 1];
  }
  players[index].handSize--;

  discardPile[discardPileSize++] = playedCard;

  if (playedCard[0] == 'S') {
    if (!wishVar.is<int>()) {
      sendError(socketID, "TargetMissing");
      return;
    }
    int targetIndex = wishVar.as<int>();
    if (targetIndex < 0 || targetIndex >= playerCount || targetIndex == index) {
      sendError(socketID, "InvalidTarget");
      return;
    }
    players[targetIndex].aussetzen = true;
  }

  if (playedCard[0] == 'R') {
    uhrzeigersinn = !uhrzeigersinn;
  }

  if (playedCard[0] == '+') {
    // +2 / +4
    letzteKartePlusKarte = true;
    aktuelleZiehenAnzahl += (playedCard[1] == '4') ? 4 : 2;
  } else {
    letzteKartePlusKarte = false;
    aktuelleZiehenAnzahl = 0;
  }

  // Wild / +4 → Wunschfarbe setzen
  if (strcmp(playedCard, "W") == 0 || strcmp(playedCard, "+4W") == 0) {
    if (!wishVar.is<const char*>()) {
      sendError(socketID, "ColorMissing");
      return;
    }
    const char* color = wishVar.as<const char*>();
    if (!(color[0] == 'R' || color[0] == 'G' || color[0] == 'B' || color[0] == 'Y')) {
      sendError(socketID, "InvalidColor");
      return;
    }
    aktuelleFarbe = color[0];
  } else {
    aktuelleFarbe = 0;
  }

  if (players[index].handSize == 0) {
    resetDeck();
    for (uint8_t i = 0; i < playerCount; i++) {
        players[i].handSize = 0; // reset hands
        players[i].isready = false;
    }
    broadcast(BROADCAST_END);
    return;
  }

  StaticJsonDocument<256> res;
  res["type"] = "aktuelleHand";
  JsonArray arr = res.createNestedArray("listHand");
  for (uint8_t i = 0; i < players[index].handSize; i++) {
    arr.add(players[index].hand[i]);
  }
  String msg;
  serializeJson(res, msg);
  webSocket.sendTXT(socketID, msg);

  nextMove();

  broadcast(BROADCAST_PLAYERHANDS);
  broadcast(BROADCAST_DECK);
}

bool isCardPlayable(const char* lastCard, const char* playedCard) {

  // +4 Wild can always be played
  if (strcmp(playedCard, "+4W") == 0) return true;

  // Standard Wild can always be played unless forced to draw
  if (strcmp(playedCard, "W") == 0) {
    return !letzteKartePlusKarte;
  }

  // Get last card color and type
  char lastType = lastCard[0]; // number or letter (S, R, +)
  char lastColor = lastCard[strlen(lastCard) - 1];

  // If a color has been chosen by Wild, that overrides last card color
  if (aktuelleFarbe != 0) lastColor = aktuelleFarbe;

  // Played card type and color
  char playType = playedCard[0];
  char playColor = playedCard[strlen(playedCard) - 1];

  // +2 stacking
  if (playType == '+' && playedCard[1] == '2') {
    if (letzteKartePlusKarte && lastCard[0] == '+' && lastCard[1] == '2') return true;
    if (playColor == lastColor) return true;
    if (lastCard[0] == '+' && lastCard[1] == '2') return true;
    return false;
  }

  // Skip / Reverse / number cards
  if (playColor == lastColor || playType == lastType) return true;

  return false;
}


void handleDraw(uint8_t socketID, StaticJsonDocument<256>& doc) {
  
  int8_t index = findPlayerBySocketID(socketID);
  if (index == -1) return;

  if (players[index].number == spielerAmZug) {
    if (players[index].aussetzen == true){
      sendError(socketID, "Aussetzen");
      players[index].aussetzen = false;
      nextMove();
    }
    else if (letzteKartePlusKarte == true) {
      DrawCard(socketID, aktuelleZiehenAnzahl);
      letzteKartePlusKarte = false;
      aktuelleZiehenAnzahl = 0;
      nextMove();
    }
    else {
      DrawCard(socketID, 1);
      nextMove();
    }
  }
  else {
    sendError(socketID, "NotYourTurn");
  }
}


void DrawCard(uint8_t socketID, uint8_t amount) {
  
  int8_t index = findPlayerBySocketID(socketID);
  if (index == -1) return;

  for (uint8_t i = 0; i < amount; i++) {
    if (players[index].handSize >= 24) break; // max Handgröße

    const char* card = drawRandomCard();
    if (!card) break;

    players[index].hand[players[index].handSize++] = card;
  }

  StaticJsonDocument<256> res;
  res["type"] = "aktuelleHand";

  JsonArray arr = res.createNestedArray("listHand");
  for (uint8_t k = 0; k < players[index].handSize; k++) {
    arr.add(players[index].hand[k]);
  }

  String msg;
  serializeJson(res, msg);
  webSocket.sendTXT(socketID, msg);

  broadcast(BROADCAST_PLAYERHANDS);
  broadcast(BROADCAST_DECK);
}


void handleReady(uint8_t socketID, StaticJsonDocument<256>& doc) {

  int8_t index = findPlayerBySocketID(socketID);
  if (index == -1) return;

  players[index].isready = true;

  broadcast(BROADCAST_READY);
  checkIfAllReady();
}


void checkIfAllReady(){
  uint8_t s = 0;
  for (uint8_t i = 0; i < playerCount; i++) {
    if (players[i].isready == true) {
      s++;
    }
  }

  if (s == playerCount){
    broadcast(BROADCAST_GAMESTART);
    StartGame();
  }
}


void handleJoin(uint8_t socketID, StaticJsonDocument<256>& doc) {

  uint8_t number = 0;
  const char* username = doc["username"];
  if (!username) {
    sendError(socketID, "Username-missing");
    return;
  }

  if (playerCount + viewerCount >= 4) {
    sendError(socketID, "Game-full");
    return;
  }

  if (gamestarted) {
    // Viewer
    viewers[viewerCount].socketID = socketID;
    strncpy(viewers[viewerCount].name, username, 15);
    viewers[viewerCount].name[15] = '\0';
    viewers[viewerCount].number = viewerCount;

    number = viewerCount;
    viewerCount++;
  }
  else {
    // Player
    players[playerCount].socketID = socketID;
    strncpy(players[playerCount].name, username, 15);
    players[playerCount].name[15] = '\0';
    players[playerCount].number = playerCount;

    number = playerCount;
    playerCount++;
  }

  StaticJsonDocument<128> res;
  res["type"] = "join_ok";
  res["playerNumber"] = number;
  res["socketID"] = socketID;
  res["gameStarted"] = gamestarted; 

  String msg;
  serializeJson(res, msg);
  webSocket.sendTXT(socketID, msg);

  broadcast(BROADCAST_PLAYERS);
}


void StartGame() {

  for (uint8_t i = drawDeckSize - 1; i > 0; i--) {
    uint8_t j = random(i + 1);
    const char* tmp = drawDeck[i];
    drawDeck[i] = drawDeck[j];
    drawDeck[j] = tmp;
  }

  for (uint8_t i = 0; i < playerCount; i++) {
    players[i].handSize = 0;
    for (uint8_t j = 0; j < 7; j++) {
      const char* card = drawRandomCard();
      players[i].hand[j] = card;
      players[i].handSize++;
    }

    StaticJsonDocument<256> res;
    res["type"] = "aktuelleHand";
    JsonArray arr = res.createNestedArray("listHand");
    for (uint8_t k = 0; k < players[i].handSize; k++) {
      arr.add(players[i].hand[k]);
    }

    String msg;
    serializeJson(res, msg);
    webSocket.sendTXT(players[i].socketID, msg);
  }

  const char* firstCard = nullptr;
  do {
    firstCard = drawRandomCard();
  } while (firstCard[0] == '+' || firstCard[0] == 'S' || firstCard[0] == 'R' || strcmp(firstCard, "W") == 0 || strcmp(firstCard, "+4W") == 0);

  discardPile[discardPileSize++] = firstCard;
  
  aktuelleFarbe = 0;

  broadcast(BROADCAST_PLAYERHANDS);

  spielerAmZug = 0;
  updateSpielerZug(spielerAmZug);

  broadcast(BROADCAST_DECK);
}


void updateSpielerZug(uint8_t num){
  
  int8_t index = -1;
  for (uint8_t i = 0; i < playerCount; i++) {
    if (players[i].number == num) {
      index = i;
      break;
    }
  }
  if (index == -1) return;
  for (uint8_t j = 0; j < playerCount; j++) {
    if (j == index){
      StaticJsonDocument<64> res;
      res["type"] = "YourT";

      String msg;
      serializeJson(res, msg);
      webSocket.sendTXT(players[j].socketID, msg);
    }
    else{
      StaticJsonDocument<64> res;
      res["type"] = "NotYourT";
      res["atTurn"] = index;

      String msg;
      serializeJson(res, msg);
      webSocket.sendTXT(players[j].socketID, msg);
    }
  }
}


const char* drawRandomCard() {

  if (drawDeckSize == 0) {
    uint8_t newDeckSize = discardPileSize - 1;
    for (uint8_t i = 0; i < newDeckSize; i++) {
      drawDeck[i] = discardPile[i];
    }

    drawDeckSize = newDeckSize;

    for (uint8_t i = drawDeckSize - 1; i > 0; i--) {
      uint8_t j = random(i + 1);
      const char* tmp = drawDeck[i];
      drawDeck[i] = drawDeck[j];
      drawDeck[j] = tmp;
    }


    discardPile[0] = discardPile[discardPileSize - 1];
    discardPileSize = 1;
  }
  const char* card = drawDeck[drawDeckSize - 1];
  drawDeckSize--;

  return card;
}


void resetDeck() {

  drawDeckSize = 108;
  discardPileSize = 0;

  for (uint8_t i = drawDeckSize - 1; i > 0; i--) {
    uint8_t j = random(i + 1);
    const char* tmp = drawDeck[i];
    drawDeck[i] = drawDeck[j];
    drawDeck[j] = tmp;
  }
}


void broadcast(BroadcastType type) {

  if (type == BROADCAST_PLAYERS) {
    StaticJsonDocument<256> doc;
    doc["type"] = "players";
    JsonArray arr = doc.createNestedArray("Playerlist");

    for (uint8_t i = 0; i < playerCount; i++) {
      arr.add(players[i].name);
    }

    JsonArray arrv = doc.createNestedArray("Viewerlist");

    for (uint8_t i = 0; i < viewerCount; i++) {
      arrv.add(viewers[i].name);
    }

    String msg;
    serializeJson(doc, msg);
    webSocket.broadcastTXT(msg);
  }
  else if (type == BROADCAST_READY) {
    StaticJsonDocument<256> doc;
    doc["type"] = "playersReady";
    JsonArray arr = doc.createNestedArray("listReady");

    for (uint8_t i = 0; i < playerCount; i++) {
      if(players[i].isready == true){
        arr.add(players[i].name);
      }
    }

    String msg;
    serializeJson(doc, msg);
    webSocket.broadcastTXT(msg);
  }
  else if (type == BROADCAST_GAMESTART){
    StaticJsonDocument<64> doc;
    doc["type"] = "gameStart";

    String msg;
    serializeJson(doc, msg);
    webSocket.broadcastTXT(msg);
  }
  else if (type == BROADCAST_PLAYERHANDS){
    StaticJsonDocument<256> res;
    res["type"] = "aktuelleHandeSpieler";

    JsonArray arr = res.createNestedArray("listHands");

    for (uint8_t i = 0; i < playerCount; i++) {
        JsonObject obj = arr.createNestedObject();
        obj["id"] = players[i].number;
        obj["handSize"] = players[i].handSize;
    }

    String msg;
    serializeJson(res, msg);
    webSocket.broadcastTXT(msg);
  }
  else if (type == BROADCAST_END) {
    int8_t index = -1;
    for (uint8_t i = 0; i < playerCount; i++) {
      if (players[i].handSize == 0) {
        index = i;
        break;
      }
    }
    if(index == -1) return;

    StaticJsonDocument<128> res;
    
    res["type"] = "gameEnd";
    res["winner"] = players[index].name;

    String msg;
    serializeJson(res, msg);
    webSocket.broadcastTXT(msg);
    saveStatsAfterGame();
  }
  else if (type == BROADCAST_DECK) {

    StaticJsonDocument<128> res;
      
    res["type"] = "CardPlayed";
    res["drawDeckSize"] = drawDeckSize;
    res["card"] = discardPile[discardPileSize -1];
    res["wishFarbe"] = String(aktuelleFarbe);
    uint8_t spieler;
    if(uhrzeigersinn){
      spieler = spielerAmZug - 1;
    }
    else {
      spieler = spielerAmZug + 1;
    }
    res["who"] = spieler; 

    String msg;
    serializeJson(res, msg);
    webSocket.broadcastTXT(msg);  
  }
  else if (type ==  BROADCAST_RESET) {

    StaticJsonDocument<64> res;
    res["type"] = "reset";

    String msg;
    serializeJson(res, msg);
    webSocket.broadcastTXT(msg);
  }
  else if (type ==  BROADCAST_RESTART) {

    StaticJsonDocument<64> res;
    res["type"] = "restartLobby";

    String msg;
    serializeJson(res, msg);
    webSocket.broadcastTXT(msg);
  }
}

//optimization
int8_t findPlayerBySocketID(uint8_t socketID) {
  for (uint8_t i = 0; i < playerCount; i++) {
    if (players[i].socketID == socketID) {
      return i;
    }
  }
  return -1;
}

bool loadStats(StaticJsonDocument<1024> &doc) {
  if (!LittleFS.begin(true)) return false;

  if (!LittleFS.exists("/stats.json")) return false;

  File file = LittleFS.open("/stats.json", "r");
  if (!file) return false;

  DeserializationError error = deserializeJson(doc, file);
  file.close();

  return !error;
}

bool saveStats(StaticJsonDocument<1024> &doc) {
  File file = LittleFS.open("/stats.json", "w");
  if (!file) return false;

  serializeJson(doc, file);
  file.close();
  return true;
}

void handleDATAREQ(uint8_t socketID, StaticJsonDocument<256>& doc) {

  int8_t index = findPlayerBySocketID(socketID);
  if (index == -1) return;

  if (!doc["usernumber"].is<int>()) return;
  int8_t playerNumber = doc["usernumber"];
  if (playerNumber < 0 || playerNumber >= playerCount) return;

  const char* playerName = players[playerNumber].name;
  if (!playerName || playerName[0] == '\0') return;

  StaticJsonDocument<1024> statsDoc;
  loadStats(statsDoc);

  const char* exists = "no";
  int wins = 0;
  int losses = 0;

  if (statsDoc.containsKey("players")) {
      JsonArray playersArr = statsDoc["players"].as<JsonArray>();
      for (JsonObject p : playersArr) {
          if (strcmp(p["name"], playerName) == 0) {
              exists = "yes";
              wins = p["wins"] | 0;
              losses = p["losses"] | 0;
              break;
          }
      }
  }

  StaticJsonDocument<256> res;
  res["type"] = "RequestedData";
  res["exists"] = exists;
  res["wins"] = wins;
  res["losses"] = losses;

  String msg;
  serializeJson(res, msg);
  webSocket.sendTXT(socketID, msg);
}


void saveStatsAfterGame() {
  
  StaticJsonDocument<1024> doc;

  if (!loadStats(doc)) {
    doc.clear();
  }

  JsonArray playersArr;
  if (doc.containsKey("players")) {
    playersArr = doc["players"].as<JsonArray>();
  } else {
    playersArr = doc.createNestedArray("players");
  }

  for (uint8_t i = 0; i < playerCount; i++) {

    const char* pname = players[i].name;
    if (!pname || pname[0] == '\0') continue;

    bool isWinner = (players[i].handSize == 0);
    bool found = false;

    for (JsonObject p : playersArr) {
      if (strcmp(p["name"], pname) == 0) {

        if (isWinner) {
          p["wins"] = (p["wins"] | 0) + 1;
        } else {
          p["losses"] = (p["losses"] | 0) + 1;
        }

        found = true;
        break;
      }
    }
    if (!found) {
      JsonObject newPlayer = playersArr.createNestedObject();
      newPlayer["name"] = pname;
      newPlayer["wins"] = isWinner ? 1 : 0;
      newPlayer["losses"] = isWinner ? 0 : 1;
    }
  }

  saveStats(doc);
}



// ===== HTTP Root Handler =====
void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

// ===== Setup =====
void setup() {
  randomSeed(esp_random());

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  if (!LittleFS.begin(true)) {
    return;
  }

  if (!LittleFS.exists("/stats.json")) {
    File file = LittleFS.open("/stats.json", "w");
    if (file) {
      file.println("{\"players\":[]}");
      file.close();
    }
  }
}

// ===== Loop =====
void loop() {
  server.handleClient();
  webSocket.loop();
}
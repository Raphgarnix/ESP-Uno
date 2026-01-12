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

  window.LookAtStatsOfAPlayer = function(cardIndex, targetPlayerIndex) {
    
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
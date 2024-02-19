var pollId = ""; // default value, hard-coded but not enough time to find a solution
let pollChoice = 0;
let pollResults = [];
let cssRoot = document.querySelector(':root');

async function getPoll() {
    const response = await fetch("http://localhost:3000/poll/plw2024/"); // Hard-coded, unsure if I'll have time to implement this properly.
    const polls = await response.json();
    const poll = polls[0];
    pollId = poll.pollId;
    sessionStorage.setItem("pollId", pollId);
    const pollOptions = poll.options;
    const optionLength = pollOptions.length;

    document.getElementById("optional1").style.display = "none";
    document.getElementById("optional2").style.display = "none";
    document.getElementById("optional3").style.display = "none";

    if(optionLength == 2) {
        cssRoot.style.setProperty('--portraitHeight', '15.25vh');
        cssRoot.style.setProperty('--landscapeHeight', '11.75vh');
    } else if(optionLength == 3) {
        cssRoot.style.setProperty('--portraitHeight', '13vh');
        cssRoot.style.setProperty('--landscapeHeight', '10vh');
        document.getElementById("optional1").style.display = "block";
    } else if(optionLength == 4) {
        cssRoot.style.setProperty('--portraitHeight', '10.75vh');
        cssRoot.style.setProperty('--landscapeHeight', '8.25vh');
        document.getElementById("optional1").style.display = "block";
        document.getElementById("optional2").style.display = "block";
    } else if(optionLength == 5) {
        cssRoot.style.setProperty('--portraitHeight', '8.5vh');
        cssRoot.style.setProperty('--landscapeHeight', '6.5vh');
        document.getElementById("optional1").style.display = "block";
        document.getElementById("optional2").style.display = "block";
        document.getElementById("optional3").style.display = "block";
    }

    document.title = "Dizplai - "+poll.pollName;
    document.getElementById("pollQuestion").innerHTML = poll.question;

    for(let i=0; i<optionLength; i++) {
        const option = pollOptions[i];
        const optionId = (option.optionId.charAt(pollId.length + 1));
        document.getElementById("poll"+optionId).innerHTML = option.optionText;
    }
}

async function submitCheck() {
    if(pollChoice == 0) {
        alert("Make a choice"); // probably don't make this an alert
    } else {
        console.log(pollId);
        console.log(pollChoice);
        const response = await fetch("http://localhost:3000/vote/", {
            method: "POST",
            headers: {
                'Accept': 'application/json',
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({"pollId": pollId, "optionId": pollChoice})
        });
        if(response.status == 200) {
            location.href="Confirm.html";
        } else {
            // Something is wrong, display some sort of error message here?
        }
    }
}

async function getVotes() {
    if(!(sessionStorage.getItem("pollId") !== null)) {
        location.href="http://localhost:3000/"
        return;
    }
    const response = await fetch("http://localhost:3000/vote/"+sessionStorage.getItem("pollId")+"/");
    results = await response.json();
    console.log(results);

    const resultsLength = results.length;

    document.getElementById("optional1").style.display = "none";
    document.getElementById("optional2").style.display = "none";
    document.getElementById("optional3").style.display = "none";

    console.log(resultsLength);

    if(resultsLength == 2) {
        cssRoot.style.setProperty('--portraitHeight', '15.25vh');
        cssRoot.style.setProperty('--landscapeHeight', '11.75vh');
    } else if(resultsLength == 3) {
        cssRoot.style.setProperty('--portraitHeight', '13vh');
        cssRoot.style.setProperty('--landscapeHeight', '10vh');
        document.getElementById("optional1").style.display = "block";
    } else if(resultsLength == 4) {
        cssRoot.style.setProperty('--portraitHeight', '10.75vh');
        cssRoot.style.setProperty('--landscapeHeight', '8.25vh');
        document.getElementById("optional1").style.display = "block";
        document.getElementById("optional2").style.display = "block";
    } else if(resultsLength == 5) {
        cssRoot.style.setProperty('--portraitHeight', '8.5vh');
        cssRoot.style.setProperty('--landscapeHeight', '6.5vh');
        document.getElementById("optional1").style.display = "block";
        document.getElementById("optional2").style.display = "block";
        document.getElementById("optional3").style.display = "block";
    }

    for(let i=0; i<resultsLength; i++) {
        console.log(i);
        document.getElementById("poll"+(i+1)).innerText = results[i][1];
        document.getElementById("perc"+(i+1)).style.width = results[i][0]+"%";
        document.getElementById("percText"+(i+1)).innerText = results[i][0]+"%";
    }
}

function vote(ele) {
    var id = ele.id;

    if(id == "poll1") {
        pollChoice = 1;
    } else if(id == "poll2") {
        pollChoice = 2;
    } else if(id == "poll3") {
        pollChoice = 3;
    } else if(id == "poll4") {
        pollChoice = 4;
    } else if(id == "poll5") {
        pollChoice = 5;
    } else {
        pollChoice = 0;
        console.log("Error");
    }

    updateHighlight();
}

function updateHighlight() {
    if(pollChoice == 0) {
        // Error
    } else {
        var selected = "poll"+pollChoice;
        var buttons = document.getElementsByClassName("clickable");
        for (var i=0; i<buttons.length; i++) {
            if(buttons.item(i).id == selected) {
                var buttonClasses = buttons.item(i).classList;
                buttonClasses.add("highlightedButton")
            } else {
                buttons.item(i).className = "pollButton clickable";
            }
        }

        var submitButton = document.getElementById("submit");
        var submitClasses = submitButton.classList;
        if(!submitClasses.contains("highlightedButton")) {
            submitClasses.add("highlightedButton");
        }
    }
}
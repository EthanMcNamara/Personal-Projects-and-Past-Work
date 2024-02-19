const express = require("express");
const bodyParser = require("body-parser");
const mongoose = require("mongoose");
const path = require("path");
const cors = require("cors");

const mongoDB = "mongodb://localhost:27017/dizplai_vote_api";
mongoose.connect(mongoDB, {useNewUrlParser: true, useUnifiedTopology: true});

const db = mongoose.connection;
db.on("error", console.error.bind(console, "MongoDB connection error:"));

const pollOption = new mongoose.Schema({
    "optionId": {
        type: String,
        required: true,
        unique: true
    },
    "optionText": {
        type: String,
        required: true
    }
});

const pollSchema = new mongoose.Schema({
    "pollId": {
        type: String,
        required: true,
        unique: true
    },
    "pollName": {
        type: String,
        required: true
    },
    "question": {
        type: String,
        required: true
    },
    "options": {
        type: [pollOption],
        required: true,
        validate: [arrayLimit, "Poll must have between 2 and 5 options"]
    }
});
const Poll = mongoose.model("Poll", pollSchema);

const voteSchema = new mongoose.Schema({
    "pollId": {
        type: String,
        required: true
    },
    "optionId": {
        type: Number,
        required: true
    }
})
const Vote = mongoose.model("Vote", voteSchema);

function arrayLimit(val) {
    return (val.length >= 2 && val.length <= 5);
}

async function createPoll(newPoll) {
    try{
        await newPoll.save();
    } catch(error) {
        if (error.name === "MongoServerError" && error.code === 11000) {
            // Already exists, no need to create poll
        } else if (error.name === "ValidationError"){
            // Poll has too few/too many options
            console.log("Poll validation failed. Make sure Poll has 2-5 options");
            console.log(error);
        } else {
            console.log(error);
            process.exit(1);
        }
    }
}

async function deleteVote(id, res) {
    var vote;
    try {
        vote = await Vote.findByIdAndRemove(id);
        if(vote == null) {
            res.status(400).send("Error 400 - Vote does not exist");
        }
    } catch(err) {
        res.status(400).send("Error 400 - Vote does not exist");
        return;
    }
    res.status(200).send();
}

async function getPollById(id, res) {
    const poll = await Poll.find({pollId: id});
    if(poll.length == 0) {
        res.status(400).send("Error 400 - Poll does not exist");
        return;
    } else {
        res.json(poll);
    }
}

async function getPolls(res) {
    const polls = await Poll.find({});
    res.json(polls);
}

async function getVotesById(id, res) {
    const votes = await Vote.find({pollId: id});

    if(JSON.stringify(votes) == "[]") {
        res.status(400).send("Error 400 - Poll not found");
        return;
    }

    const polls = await Poll.find({pollId: id});
    
    const poll = polls[0];
    const options = poll.options;
    var voteCount = 0;

    var totalVotes = [];
    for(let i=0; i<options.length; i++) {
        const option = options[i];
        totalVotes[i] = [0, option.optionText];
    }

    for(let i=0; i<votes.length; i++) {
        const vote = votes[i];
        totalVotes[vote.optionId - 1][0] += 1;
        voteCount++;
    }

    totalVotes = totalVotes.sort().reverse();

    for(let i=0; i<totalVotes.length; i++) {
        totalVotes[i][0] = Math.round(totalVotes[i][0]*100/voteCount);
    }

    res.json(totalVotes);
}

async function postVote(vote, res) {
    var validPoll = true;
    var validOption = true;
    var poll;

    const polls = await Poll.find({pollId: vote.pollId});
    if(polls.length != 0) {
        poll = polls[0];
        const pollOptions = poll.options;
        if(vote.optionId < 1 || vote.optionId > pollOptions.length) {
            // Return an error, invalid vote given.
            validOption = false;
            res.status(400).send("Error 400 - Option out of range");
            return;
        }        
    } else {
        // Return an error, invalid vote given.
        validPoll = false;
        res.status(400).send("Error 400 - Poll does not exist");
        return;
    }

    if(!validPoll || !validOption) {
        // Return an error, invalid vote given.
        // This error shouldn't occur, but there's no harm in a little insurance!
        res.status(400).send("Error 400 - Poll does not exist, or chosen Option out of range");
        return;
    } else {
        await vote.save();
        res.status(200).send(vote._id);
    }
}

const poll1 = new Poll({
    pollId: "plw2024",
    pollName: "Premier League Winner",
    question: "Who will win the <b>Premier League?</b>",
    options: [
        {
            optionId: "plw2024-1",
            optionText: "Manchester City"
        },
        {
            optionId: "plw2024-2",
            optionText: "Arsenal"
        },
        {
            optionId: "plw2024-3",
            optionText: "Liverpool"
        }
    ]
});
const poll2 = new Poll({
    pollId: "oscarAFF2024",
    pollName: "2024 Best Animated Feature Film Winner",
    question: "Who will win <b>Best Animated Feature Film</b> at The Oscars?",
    options: [
        {
            optionId: "oscarAFF2024-1",
            optionText: "The Boy and The Heron"
        },
        {
            optionId: "oscarAFF2024-2",
            optionText: "Elemental"
        },
        {
            optionId: "oscarAFF2024-3",
            optionText: "Nimona"
        },
        {
            optionId: "oscarAFF2024-4",
            optionText: "Robot Dreams"
        },
        {
            optionId: "oscarAFF2024-5",
            optionText: "Spider-Man: Acroos The Spider-Verse"
        }
    ]
});
createPoll(poll1);
createPoll(poll2);

const app = express();

app.use(cors());

app.use(bodyParser.urlencoded({extended:false}));

app.use(bodyParser.json());

app.use("/", express.static(path.join(__dirname, "/public")));

const port = 3000;

// CRUD = Create, Read, Update and Delete
// For HTML this is POST, GET, PUT and DELETE

app.get("/poll", (req, res) => {
    getPolls(res);
});
app.get("/poll/:id", (req, res) => {
    getPollById(req.params.id, res);
});
app.get("/vote", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.get("/vote/:id", (req, res) => {
    getVotesById(req.params.id, res);
});
app.get("*", (req, res) => {
    res.status(404).send("Error 404 - Not found");
});

app.post("/poll", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.post("/poll/:id", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.post("/vote", (req, res) => {
    const vote = new Vote({
        pollId: req.body.pollId,
        optionId: req.body.optionId
    });

    postVote(vote, res);
});
app.post("/vote/:id", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.post("*", (req, res) => {
    res.status(404).send("Error 404 - Not found");
});

app.put("/poll", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.put("/poll/:id", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.put("/vote", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.put("/vote/:id", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.put("*", (req, res) => {
    res.status(404).send("Error 404 - Not found");
});

app.patch("/poll", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.patch("/poll/:id", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.patch("/vote", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.patch("/vote/:id", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.patch("*", (req, res) => {
    res.status(404).send("Error 404 - Not found");
});

app.delete("/poll", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.delete("/poll/:id", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.delete("/vote", (req, res) => {
    res.status(405).send("Error 405 - Method Not Allowed");
});
app.delete("/vote/:id", (req, res) => {
    deleteVote(req.params.id, res);
});
app.delete("*", (req, res) => {
    res.status(404).send("Error 404 - Not found");
});

app.listen(port, () => {
    console.log(`Listening on port ${port}`);
});